#pragma once

#include "CoreMinimal.h"
#include "Encounters/OryxEncounter.h"
#include "Interfaces/OryxInteractable.h"
#include "OryxEncounter_Totem.generated.h"

class UStaticMeshComponent;

/**
 *  Totem-shaped interactable encounter. Player presses Interact while looking at it
 *  to start the spawn wave. One-shot: cleared totems do nothing.
 */
UCLASS()
class ORYX_API AOryxEncounter_Totem : public AOryxEncounter, public IOryxInteractable
{
	GENERATED_BODY()

public:
	AOryxEncounter_Totem();

	// IOryxInteractable
	virtual void Interact_Implementation(AOryxCharacter* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
