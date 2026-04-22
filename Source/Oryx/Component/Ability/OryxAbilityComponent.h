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

protected:
	virtual void BeginPlay() override;

	// Ability classes granted to the owner — configured per-Blueprint
	UPROPERTY(EditAnywhere, Category = "Oryx|Ability")
	TArray<TSubclassOf<UOryxAbility>> AbilityClasses;

	// Runtime instances, created in BeginPlay from AbilityClasses
	UPROPERTY(Transient)
	TArray<UOryxAbility*> Abilities;
};
