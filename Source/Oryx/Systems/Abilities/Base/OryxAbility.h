#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OryxAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class ORYX_API UOryxAbility : public UObject
{
	GENERATED_BODY()

public:
	UOryxAbility();

	// Cooldown in seconds between activations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Ability")
	float Cooldown = 1.f;

	// Called by UOryxAbilityComponent after the cooldown gate passes
	UFUNCTION(BlueprintNativeEvent, Category = "Oryx|Ability")
	void Activate(AActor* Owner);
	virtual void Activate_Implementation(AActor* Owner);

	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	bool IsOnCooldown(float CurrentTime) const;

	// Stamps activation time — called by the component post-Activate
	void MarkActivated(float CurrentTime) { LastActivatedTime = CurrentTime; }

	// Needed so Blueprint world-context nodes resolve correctly on this UObject
	virtual UWorld* GetWorld() const override;

private:
	UPROPERTY(Transient)
	float LastActivatedTime = -1000.f;
};
