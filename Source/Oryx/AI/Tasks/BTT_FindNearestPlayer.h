#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FindNearestPlayer.generated.h"

/**
 *  Behavior Tree task: finds the nearest alive AOryxCharacter and writes it
 *  to the configured blackboard key. Co-op proof — does not assume player 0.
 *  Returns Succeeded when a target is found, Failed otherwise.
 */
UCLASS()
class ORYX_API UBTT_FindNearestPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FindNearestPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** Blackboard key to write the chosen target into (must be Object, Base = Actor) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
