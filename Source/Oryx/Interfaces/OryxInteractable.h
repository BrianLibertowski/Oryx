#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OryxInteractable.generated.h"

class AOryxCharacter;

UINTERFACE(MinimalAPI, Blueprintable)
class UOryxInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Implemented by anything the player can interact with via the Interact input
 *  (totems, vendors, chests, doors). The character traces forward and calls
 *  Interact on the first hit that implements this interface.
 */
class ORYX_API IOryxInteractable
{
	GENERATED_BODY()

public:
	/** Called when the interactor presses the Interact key while looking at this actor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Oryx|Interact")
	void Interact(AOryxCharacter* Interactor);
	virtual void Interact_Implementation(AOryxCharacter* Interactor) {}
};
