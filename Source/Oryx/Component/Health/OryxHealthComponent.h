#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxDamageTypes.h"
#include "OryxHealthComponent.generated.h"

class UOryxStatsComponent;

// Broadcast when damage is received (legacy — used by HUD bars). Healing fires this with negative DamageAmount.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, float, NewHealth, float, DamageAmount);

// Broadcast after the damage formula resolves. Carries the full event + final number + crit flag.
// Used by floating damage numbers, hit reactions, on-hit triggers.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageReceived, FOryxDamageEvent, Event, float, FinalDamage, bool, bWasCrit);

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
	FOnDamageReceived OnDamageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Health")
	FOnDeath OnDeath;

	// --- Interface ---
	/** Preferred entry point. Carries instigator / damage type / source so the formula can scale properly. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void ApplyDamageEvent(FOryxDamageEvent Event);

	/** Compat wrapper. Wraps the float into a Physical FOryxDamageEvent with no instigator. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void ApplyHealing(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Health")
	void IncreaseMaxHealth(float Amount);

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Health")
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleStatsChanged();

private:
	/** Fallback when owner has no UOryxStatsComponent (e.g. enemies). Players read MaxHealth from Stats. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Health")
	float CurrentHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Health")
	bool bIsDead = false;

	/** Cached on BeginPlay; null for owners without stats (enemies). */
	UPROPERTY(Transient)
	UOryxStatsComponent* StatsComp = nullptr;
};