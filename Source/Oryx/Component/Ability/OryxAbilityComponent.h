#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxAbilityComponent.generated.h"

class UOryxAbility;

UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxAbilityComponent();

	UFUNCTION(BlueprintCallable, Category = "Oryx|Ability")
	bool TryActivateAbility(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	UOryxAbility* GetAbility(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	int32 GetAbilityCount() const { return Abilities.Num(); }

	/**
	 *  Returns 0..1 cooldown progress for the ability at Index.
	 *  1 = ready to fire, 0 = just activated, intermediate = filling.
	 *  HUD radial pips bind to this.
	 */
	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	float GetCooldownFraction(int32 Index) const;

	/** Returns the ability's effective cooldown duration after stat scaling, or 0 if invalid. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	float GetEffectiveCooldown(int32 Index) const;

protected:
	virtual void BeginPlay() override;

	// Ability classes granted to the owner — configured per-Blueprint
	UPROPERTY(EditAnywhere, Category = "Oryx|Ability")
	TArray<TSubclassOf<UOryxAbility>> AbilityClasses;

	// Runtime instances, created in BeginPlay from AbilityClasses
	UPROPERTY(Transient)
	TArray<UOryxAbility*> Abilities;
};
