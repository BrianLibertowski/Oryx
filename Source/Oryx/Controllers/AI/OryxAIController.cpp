#include "OryxAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AOryxAIController::AOryxAIController()
{
}

void AOryxAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
	{
		UBlackboardComponent* BB = nullptr;
		UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BB);
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
