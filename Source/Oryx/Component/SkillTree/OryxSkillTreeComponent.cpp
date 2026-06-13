#include "OryxSkillTreeComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include "Systems/SkillTree/OryxSkillTree.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "Component/Stats/OryxStatFormatLibrary.h"
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
	const bool bIsPassive = (Tree->TreeType == EOryxSkillTreeType::Passive);
	const bool bConsumed = bIsPassive
		? Level->TryConsumePassivePoint(Class)
		: Level->TryConsumeAbilityPoint(Class);
	if (!bConsumed) return false;

	// Bump rank + record which pool the point came from (RefundAll restores exact counts).
	FOryxSkillTreeAllocations& Alloc = ClassAllocations.FindOrAdd(Class);
	if (bIsPassive) { Alloc.SpentPassivePoints++; } else { Alloc.SpentAbilityPoints++; }
	const int32 NewRank = Alloc.NodeRanks.FindOrAdd(NodeId) + 1;
	Alloc.NodeRanks[NodeId] = NewRank;

	// Push ALL modifiers for this rank onto StatsComponent. Rank 1 uses Ranks[0], etc.
	// A rank can grant multiple stats (keystones/apexes) — every modifier in the entry applies.
	if (Node->Ranks.IsValidIndex(NewRank - 1))
	{
		if (UOryxStatsComponent* Stats = ResolveStatsComponent())
		{
			for (const FOryxStatModifier& RankMod : Node->Ranks[NewRank - 1].Modifiers)
			{
				const FGuid AssignedId = Stats->AddModifier(RankMod);
				Alloc.AppliedModifierIds.Add(AssignedId);
			}
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

	// Restore exact per-pool counts recorded at Invest time.
	if (UOryxLevelComponent* Level = ResolveLevelComponent())
	{
		Level->RefundPassivePoints(Class, Alloc->SpentPassivePoints);
		Level->RefundAbilityPoints(Class, Alloc->SpentAbilityPoints);
	}

	// Wipe allocations
	Alloc->NodeRanks.Reset();
	Alloc->AppliedModifierIds.Reset();
	Alloc->SpentPassivePoints = 0;
	Alloc->SpentAbilityPoints = 0;

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

FOryxSkillNodeDisplayInfo UOryxSkillTreeComponent::GetNodeDisplayInfo(EOryxClass Class, const UOryxSkillTree* Tree, FName NodeId) const
{
	FOryxSkillNodeDisplayInfo Info;
	Info.NodeId = NodeId;

	if (!Tree) return Info;
	const FOryxSkillNode* Node = Tree->FindNode(NodeId);
	if (!Node) return Info;

	Info.DisplayName = Node->DisplayName;
	Info.Description = Node->Description;
	Info.MaxRanks = Node->MaxRanks;
	Info.CurrentRank = GetRank(Class, NodeId);

	// --- Free base ability ---
	if (Node->bIsBaseAbility)
	{
		Info.State = EOryxNodeState::Granted;
		Info.bCanInvest = false;
		Info.RankText = NSLOCTEXT("Oryx", "Node_Granted", "Granted");
		return Info;
	}

	Info.RankText = FText::FromString(FString::Printf(TEXT("%d / %d"), Info.CurrentRank, Info.MaxRanks));

	// --- Modifier preview (current → next per modifier in the per-rank set) ---
	// Stepped design: Ranks[0] is the canonical per-rank modifier set.
	if (Node->Ranks.Num() > 0)
	{
		TArray<FString> Lines;
		for (const FOryxStatModifier& Mod : Node->Ranks[0].Modifiers)
		{
			Lines.Add(UOryxStatFormatLibrary::FormatRankPreview(Mod.Stat, Mod.Value, Info.CurrentRank, Info.MaxRanks).ToString());
		}
		Info.ModifierPreview = FText::FromString(FString::Join(Lines, TEXT("\n")));
	}

	// --- State + lock reason ---
	if (Info.CurrentRank >= Info.MaxRanks)
	{
		Info.State = EOryxNodeState::Maxed;
		Info.bCanInvest = false;
		return Info;
	}

	Info.bCanInvest = CanInvest(Class, Tree, NodeId);
	Info.State = Info.bCanInvest
		? (Info.CurrentRank > 0 ? EOryxNodeState::Partial : EOryxNodeState::Available)
		: (Info.CurrentRank > 0 ? EOryxNodeState::Partial : EOryxNodeState::Locked);

	if (!Info.bCanInvest)
	{
		Info.LockReason = BuildLockReason(Class, Tree, *Node);
	}

	return Info;
}

FText UOryxSkillTreeComponent::BuildLockReason(EOryxClass Class, const UOryxSkillTree* Tree, const FOryxSkillNode& Node) const
{
	const FOryxSkillTreeAllocations& Alloc = ClassAllocations.FindRef(Class);

	// Order mirrors CanInvest so the surfaced reason matches the actual blocker.
	if (Tree->MaxInvestedInTree > 0 && GetTotalInvestedInTree(Class, Tree) >= Tree->MaxInvestedInTree)
	{
		return NSLOCTEXT("Oryx", "Lock_TreeCap", "Tree investment limit reached");
	}

	if (Node.RequiredPointsInTree > 0 && GetTotalInvestedInTree(Class, Tree) < Node.RequiredPointsInTree)
	{
		return FText::FromString(FString::Printf(TEXT("Requires %d points in this tree"), Node.RequiredPointsInTree));
	}

	for (const FName& RequiredId : Node.RequiredNodes)
	{
		if (Alloc.NodeRanks.FindRef(RequiredId) <= 0)
		{
			FString ReqName = RequiredId.ToString();
			if (const FOryxSkillNode* ReqNode = Tree->FindNode(RequiredId))
			{
				ReqName = ReqNode->DisplayName.ToString();
			}
			return FText::FromString(FString::Printf(TEXT("Requires: %s"), *ReqName));
		}
	}

	// Last check in CanInvest is available points.
	if (const UOryxLevelComponent* Level = ResolveLevelComponent())
	{
		const bool bPassive = (Tree->TreeType == EOryxSkillTreeType::Passive);
		const int32 Available = bPassive ? Level->GetUnallocatedPassive(Class) : Level->GetUnallocatedAbility(Class);
		if (Available <= 0)
		{
			return bPassive
				? NSLOCTEXT("Oryx", "Lock_NoPassive", "No passive points")
				: NSLOCTEXT("Oryx", "Lock_NoAbility", "No ability points");
		}
	}

	return FText::GetEmpty();
}

void UOryxSkillTreeComponent::SetAllocations(EOryxClass Class, const FOryxSkillTreeAllocations& Allocations)
{
	ClassAllocations.Add(Class, Allocations);
	OnAllocationsChanged.Broadcast();
}

void UOryxSkillTreeComponent::ReapplyAllocations(EOryxClass Class, const TArray<UOryxSkillTree*>& Trees)
{
	FOryxSkillTreeAllocations* Alloc = ClassAllocations.Find(Class);
	if (!Alloc || Alloc->NodeRanks.Num() == 0) return;

	UOryxStatsComponent* Stats = ResolveStatsComponent();
	if (!Stats)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeComponent::ReapplyAllocations — no StatsComponent reachable, skipped."));
		return;
	}

	// Saved GUIDs reference modifiers on a StatsComponent that no longer exists. Drop and rebuild.
	Alloc->AppliedModifierIds.Reset();

	int32 ReappliedRanks = 0;
	for (const TPair<FName, int32>& Pair : Alloc->NodeRanks)
	{
		// Locate the node across this class's registered trees.
		const FOryxSkillNode* Node = nullptr;
		for (const UOryxSkillTree* Tree : Trees)
		{
			if (!Tree || Tree->OwningClass != Class) continue;
			if (const FOryxSkillNode* Found = Tree->FindNode(Pair.Key))
			{
				Node = Found;
				break;
			}
		}
		if (!Node)
		{
			// Node id saved but no longer exists in any registered tree (renamed/deleted during authoring,
			// or the tree asset isn't in RegisteredSkillTrees). Skip — points stay spent; respec recovers them.
			UE_LOG(LogTemp, Warning, TEXT("ReapplyAllocations: saved node '%s' not found in registered trees — skipped."),
				*Pair.Key.ToString());
			continue;
		}

		const int32 InvestedRanks = FMath::Min(Pair.Value, Node->MaxRanks);
		for (int32 R = 0; R < InvestedRanks; ++R)
		{
			if (Node->Ranks.IsValidIndex(R))
			{
				for (const FOryxStatModifier& Mod : Node->Ranks[R].Modifiers)
				{
					const FGuid AssignedId = Stats->AddModifier(Mod);
					Alloc->AppliedModifierIds.Add(AssignedId);
				}
				++ReappliedRanks;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SkillTreeComponent: reapplied %d rank modifier(s) for class %d."),
		ReappliedRanks, static_cast<int32>(Class));
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
