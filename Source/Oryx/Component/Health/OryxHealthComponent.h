#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxHealthComponent.generated.h"

// Broadcast when damage is received
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, float, NewHealth, float, DamageAmount);

// Broadcast when health reaches zero
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);

UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxHealthComponent();

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "Oryx|Health")
	FOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Health")
	FOnDeath OnDeath;

	// --- Interface ---
	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void ApplyHealing(float HealAmount);

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Oryx|Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Health")
	float CurrentHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Health")
	bool bIsDead = false;
};