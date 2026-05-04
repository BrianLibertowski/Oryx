#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTService_LeashWatch.generated.h"

/**
 *  Ticks while attached. If the controlled AOryxEnemy is beyond its leash,
 *  clears forced aggro and the configured TargetActor blackboard key.
 *  Pair with a Blackboard "TargetActor Is Set" decorator (Observer Aborts: Self)
 *  on Chase/Attack sequences so the latent MoveTo aborts immediately on leash trip.
 */
UCLASS()
class ORYX_API UBTService_LeashWatch : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_LeashWatch();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
