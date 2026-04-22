#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OryxAIController.generated.h"

class UBehaviorTree;

UCLASS()
class ORYX_API AOryxAIController : public AAIController
{
	GENERATED_BODY()

public:
	AOryxAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Oryx|AI")
	UBehaviorTree* BehaviorTreeAsset;
};
