#include "BTT_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

#include "Characters/Player/OryxCharacter.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "Component/Health/OryxHealthComponent.h"

UBTT_FindNearestPlayer::UBTT_FindNearestPlayer()
{
	NodeName = TEXT("Find Nearest Player");

	// Restrict the dropdown to Object keys (TargetActor)
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FindNearestPlayer, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTT_FindNearestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* SelfPawn = AICon->GetPawn();
	if (!SelfPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	UWorld* World = SelfPawn->GetWorld();
	if (!World) return EBTNodeResult::Failed;

	AOryxEnemy* SelfEnemy = Cast<AOryxEnemy>(SelfPawn);

	// Leash: past range → drop aggro, fail. Selector falls to ReturnToSpawn branch.
	if (SelfEnemy && SelfEnemy->IsBeyondLeash())
	{
		SelfEnemy->ClearAggro();
		BB->ClearValue(TargetActorKey.SelectedKeyName);
		return EBTNodeResult::Failed;
	}

	// Forced aggro (from damage spread or camp proximity) bypasses normal range search
	if (SelfEnemy && SelfEnemy->IsForcedAggro())
	{
		AActor* Forced = SelfEnemy->GetForcedTarget();
		if (IsValid(Forced))
		{
			// Still respect death — if the forced target is dead, fall through to range search
			if (AOryxCharacter* AsCharacter = Cast<AOryxCharacter>(Forced))
			{
				if (UOryxHealthComponent* HC = AsCharacter->GetHealthComponent())
				{
					if (!HC->IsDead())
					{
						BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Forced);
						return EBTNodeResult::Succeeded;
					}
				}
			}
		}
	}

	const FVector SelfLoc = SelfPawn->GetActorLocation();

	AOryxCharacter* Best = nullptr;
	// Cap search to MaxAggroRange (squared) — outside this, nothing is a valid target.
	// MaxAggroRange == 0 means unlimited.
	float BestDistSq = (MaxAggroRange > 0.f) ? (MaxAggroRange * MaxAggroRange) : TNumericLimits<float>::Max();

	for (TActorIterator<AOryxCharacter> It(World); It; ++It)
	{
		AOryxCharacter* Candidate = *It;
		if (!IsValid(Candidate)) continue;

		// Skip dead players
		if (UOryxHealthComponent* HC = Candidate->GetHealthComponent())
		{
			if (HC->IsDead()) continue;
		}

		const float DistSq = FVector::DistSquared(Candidate->GetActorLocation(), SelfLoc);
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate;
		}
	}

	if (!Best)
	{
		BB->ClearValue(TargetActorKey.SelectedKeyName);
		return EBTNodeResult::Failed;
	}

	BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Best);
	return EBTNodeResult::Succeeded;
}
