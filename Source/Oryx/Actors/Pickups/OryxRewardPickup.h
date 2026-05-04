#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OryxInteractable.h"
#include "OryxRewardPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UUserWidget;

/**
 *  Spawned by AOryxEncounter on completion. Sits in the world as an interactable.
 *  Player presses Interact → the configured reward widget is shown to that player only
 *  (Game and UI input mode, no pause — co-op can keep playing).
 *  Single-use: pickup destroys itself when interacted with.
 */
UCLASS()
class ORYX_API AOryxRewardPickup : public AActor, public IOryxInteractable
{
	GENERATED_BODY()

public:
	AOryxRewardPickup();

	// IOryxInteractable
	virtual void Interact_Implementation(AOryxCharacter* Interactor) override;

	/** Widget shown to the interacting player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Reward")
	TSubclassOf<UUserWidget> RewardWidgetClass;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
