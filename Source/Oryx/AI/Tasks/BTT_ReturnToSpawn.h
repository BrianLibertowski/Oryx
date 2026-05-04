#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ReturnToSpawn.generated.h"

/**
 *  Walks an AOryxEnemy back to its cached SpawnLocation. Fires AAIController::MoveToLocation
 *  and returns Succeeded immediately — the BT will tick again and pick this branch only
 *  while no aggro target exists, so the move continues until aggro re-acquires or the
 *  enemy reaches home.
 */
UCLASS()
class ORYX_API UBTT_ReturnToSpawn : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_ReturnToSpawn();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** How close to spawn the enemy needs to get to count as "home" (cm) */
	UPROPERTY(EditAnywhere, Category = "ReturnToSpawn", meta = (ClampMin = "1"))
	float AcceptanceRadius = 100.f;
};
