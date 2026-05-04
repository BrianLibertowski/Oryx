#include "BTService_LeashWatch.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/Enemies/Base/OryxEnemy.h"

UBTService_LeashWatch::UBTService_LeashWatch()
{
	NodeName = TEXT("Leash Watch");
	Interval = 0.2f;
	RandomDeviation = 0.0f;
	bNotifyTick = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_LeashWatch, TargetActorKey), AActor::StaticClass());
}

void UBTService_LeashWatch::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return;

	AOryxEnemy* SelfEnemy = Cast<AOryxEnemy>(AICon->GetPawn());
	if (!SelfEnemy) return;

	if (!SelfEnemy->IsBeyondLeash()) return;

	SelfEnemy->ClearAggro();

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->ClearValue(TargetActorKey.SelectedKeyName);
	}
}
