#include "BTT_ReturnToSpawn.h"

#include "AIController.h"
#include "Characters/Enemies/Base/OryxEnemy.h"

UBTT_ReturnToSpawn::UBTT_ReturnToSpawn()
{
	NodeName = TEXT("Return To Spawn");
}

EBTNodeResult::Type UBTT_ReturnToSpawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AOryxEnemy* SelfEnemy = Cast<AOryxEnemy>(AICon->GetPawn());
	if (!SelfEnemy) return EBTNodeResult::Failed;

	const FVector Home = SelfEnemy->GetSpawnLocation();
	const float DistSq = FVector::DistSquared(SelfEnemy->GetActorLocation(), Home);

	// Already home — clear forced aggro state (we've successfully disengaged) and succeed
	if (DistSq <= (AcceptanceRadius * AcceptanceRadius))
	{
		SelfEnemy->ClearAggro();
		return EBTNodeResult::Succeeded;
	}

	AICon->MoveToLocation(Home, AcceptanceRadius, /*bStopOnOverlap*/ false, /*bUsePathfinding*/ true);
	return EBTNodeResult::Succeeded;
}
