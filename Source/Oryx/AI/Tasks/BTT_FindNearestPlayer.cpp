#include "BTT_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

#include "Characters/Player/OryxCharacter.h"
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

	if (!Best) return EBTNodeResult::Failed;

	BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Best);
	return EBTNodeResult::Succeeded;
}
