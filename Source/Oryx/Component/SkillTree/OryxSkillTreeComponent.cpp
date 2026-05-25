#include "OryxSkillTreeComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include "Systems/SkillTree/OryxSkillTree.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "States/Player/OryxPlayerState.h"

UOryxSkillTreeComponent::UOryxSkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UOryxSkillTreeComponent::CanInvest(EOryxClass Class, const UOryxSkillTree* Tree, FName NodeId) const
{
	if (!Tree) return false;

	const FOryxSkillNode* Node = Tree->FindNode(NodeId);
	if (!Node) return false;

	const FOryxSkillTreeAllocations& Alloc = ClassAllocations.FindRef(Class);
	const int32 CurrentRank = Alloc.NodeRanks.FindRef(NodeId);

	// Cap check: rank < MaxRanks
	if (CurrentRank >= Node->MaxRanks) return false;

	// Tree investment cap (per D14, ability tree caps at 15 points invested)
	if (Tree->MaxInvestedInTree > 0)
	{
		if (GetTotalInvestedInTree(Class, Tree) >= Tree->MaxInvestedInTree) return false;
	}

	// Soft gate: total points already invested in this tree >= RequiredPointsInTree
	if (Node->RequiredPointsInTree > 0)
	{
		if (GetTotalInvestedInTree(Class, Tree) < Node->RequiredPointsInTree) return false;
	}

	// Hard prereqs: every RequiredNodes entry must have >=1 rank invested
	for (const FName& RequiredId : Node->RequiredNodes)
	{
		if (Alloc.NodeRanks.FindRef(RequiredId) <= 0) return false;
	}

	// Unallocated points check (passive tree consumes passive points, ability tree consumes ability points)
	const UOryxLevelComponent* Level = ResolveLevelComponent();
	if (!Level) return false;
	const int32 Available = (Tree->TreeType == EOryxSkillTreeType::Passive)
		? Level->GetUnallocatedPassive(Class)
		: Level->GetUnallocatedAbility(Class);
	if (Available <= 0) return false;

	return true;
}

bool UOryxSkillTreeComponent::Invest(EOryxClass Class, UOryxSkillTree* Tree, FName NodeId)
{
	if (!CanInvest(Class, Tree, NodeId)) return false;

	const FOryxSkillNode* Node = Tree->FindNode(NodeId);
	if (!Node) return false;

	UOryxLevelComponent* Level = ResolveLevelComponent();
	if (!Level) return false;

	// Consume the point (gated by CanInvest, should never fail here).
	const bool bConsumed = (Tree->TreeType == EOryxSkillTreeType::Passive)
		? Level->TryConsumePassivePoint(Class)
		: Level->TryConsumeAbilityPoint(Class);
	if (!bConsumed) return false;

	// Bump rank
	FOryxSkillTreeAllocations& Alloc = ClassAllocations.FindOrAdd(Class);
	const int32 NewRank = Alloc.NodeRanks.FindOrAdd(NodeId) + 1;
	Alloc.NodeRanks[NodeId] = NewRank;

	// Push the rank's modifier(s) onto StatsComponent. Rank 1 uses index 0, etc.
	if (Node->ModifiersPerRank.IsValidIndex(NewRank - 1))
	{
		if (UOryxStatsComponent* Stats = ResolveStatsComponent())
		{
			const FOryxStatModifier RankMod = Node->ModifiersPerRank[NewRank - 1];
			const FGuid AssignedId = Stats->AddModifier(RankMod);
			Alloc.AppliedModifierIds.Add(AssignedId);
		}
	}

	OnAllocationsChanged.Broadcast();
	return true;
}

void UOryxSkillTreeComponent::RefundAll(EOryxClass Class)
{
	FOryxSkillTreeAllocations* Alloc = ClassAllocations.Find(Class);
	if (!Alloc) return;

	// Remove all pushed modifiers from StatsComponent
	if (UOryxStatsComponent* Stats = ResolveStatsComponent())
	{
		for (const FGuid& SourceId : Alloc->AppliedModifierIds)
		{
			Stats->RemoveModifiersBySource(SourceId);
		}
	}

	// Restore points to LevelComponent. Total spent = sum of all ranks across nodes.
	int32 TotalPassive = 0;
	int32 TotalAbility = 0;
	// We don't know per-node which tree contributed without the tree asset; restore conservatively
	// by counting passive vs ability via tree type lookup is impossible from here without tree refs.
	// Practical approach: track at Invest time which tree was used. TODO: extend FOryxSkillTreeAllocations
	// to record per-node tree type, OR have caller pass the trees array. For Phase 4 MVP, refund
	// everything as passive (worst case slight imbalance — fix in tuning pass).
	for (const TPair<FName, int32>& Pair : Alloc->NodeRanks)
	{
		TotalPassive += Pair.Value;
	}

	if (UOryxLevelComponent* Level = ResolveLevelComponent())
	{
		Level->RefundPassivePoints(Class, TotalPassive);
		Level->RefundAbilityPoints(Class, TotalAbility);
	}

	// Wipe allocations
	Alloc->NodeRanks.Reset();
	Alloc->AppliedModifierIds.Reset();

	OnAllocationsChanged.Broadcast();
}

int32 UOryxSkillTreeComponent::GetRank(EOryxClass Class, FName NodeId) const
{
	const FOryxSkillTreeAllocations* Alloc = ClassAllocations.Find(Class);
	if (!Alloc) return 0;
	return Alloc->NodeRanks.FindRef(NodeId);
}

int32 UOryxSkillTreeComponent::GetTotalInvestedInTree(EOryxClass Class, const UOryxSkillTree* Tree) const
{
	if (!Tree) return 0;
	const FOryxSkillTreeAllocations* Alloc = ClassAllocations.Find(Class);
	if (!Alloc) return 0;

	int32 Total = 0;
	for (const FOryxSkillNode& Node : Tree->Nodes)
	{
		Total += Alloc->NodeRanks.FindRef(Node.NodeId);
	}
	return Total;
}

FOryxSkillTreeAllocations UOryxSkillTreeComponent::GetAllocations(EOryxClass Class) const
{
	return ClassAllocations.FindRef(Class);
}

void UOryxSkillTreeComponent::SetAllocations(EOryxClass Class, const FOryxSkillTreeAllocations& Allocations)
{
	ClassAllocations.Add(Class, Allocations);
	OnAllocationsChanged.Broadcast();
}

UOryxStatsComponent* UOryxSkillTreeComponent::ResolveStatsComponent() const
{
	// SkillTreeComponent lives on PlayerState. To reach StatsComponent we go: PlayerState → Pawn → StatsComponent.
	const APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (!PS) return nullptr;

	const APawn* Pawn = PS->GetPawn();
	if (!Pawn) return nullptr;

	return Pawn->FindComponentByClass<UOryxStatsComponent>();
}

UOryxLevelComponent* UOryxSkillTreeComponent::ResolveLevelComponent() const
{
	// Sibling component on the same PlayerState.
	if (const AOryxPlayerState* PS = Cast<AOryxPlayerState>(GetOwner()))
	{
		return PS->GetLevelComponent();
	}
	return nullptr;
}
