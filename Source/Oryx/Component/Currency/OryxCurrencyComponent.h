#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxCurrencyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyChanged, int32, NewGold, int32, NewOnyx);

UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxCurrencyComponent();

	UFUNCTION(BlueprintCallable, Category = "Oryx|Currency")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Currency")
	void AddOnyx(int32 Amount);

	/** Atomically spend gold. Returns true if Amount <= current Gold and was deducted; false otherwise (no partial spend). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Currency")
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Oryx|Currency")
	bool CanAfford(int32 Amount) const { return Amount <= 0 || Gold >= Amount; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Currency")
	int32 GetGold() const { return Gold; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Currency")
	int32 GetOnyx() const { return Onyx; }

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Currency")
	FOnCurrencyChanged OnCurrencyChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Currency")
	int32 Gold = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Currency")
	int32 Onyx = 0;
};
