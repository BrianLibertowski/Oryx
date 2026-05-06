#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxStatusEffectTypes.h"
#include "OryxStatusEffectsComponent.generated.h"

class UOryxHealthComponent;
class UOryxStatsComponent;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectApplied, EOryxStatusEffectType, Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffectExpired, EOryxStatusEffectType, Type);

/**
 *  Tracks active status effects on the owner. Burn/Poison tick damage through the owner's
 *  HealthComponent (so DoTs scale with the instigator's stats via the shared damage formula).
 *  Chill pushes a Multiplicative MovementSpeed modifier into the owner's StatsComponent.
 *
 *  Uniqueness: (SourceId, Type) — re-applying refreshes duration; different sources stack.
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxStatusEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxStatusEffectsComponent();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Status")
	FOnStatusEffectApplied OnEffectApplied;

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Status")
	FOnStatusEffectExpired OnEffectExpired;

	/** Applies an effect. If (SourceId, Type) already active, refreshes duration only. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Status")
	void ApplyEffect(FOryxStatusEffectSpec Spec);

	/** Removes a specific (SourceId, Type) instance. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Status")
	void RemoveEffect(FGuid SourceId, EOryxStatusEffectType Type);

	/** Wipes everything (death, respawn, new run). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Status")
	void ClearAllEffects();

	UFUNCTION(BlueprintPure, Category = "Oryx|Status")
	bool HasEffect(EOryxStatusEffectType Type) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Status")
	int32 GetActiveCount() const { return ActiveEffects.Num(); }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandleOwnerDeath(AActor* DeadActor);

private:
	/** Routes a DoT tick through the owner's HealthComponent using FOryxDamageEvent. */
	void TickDoT(const FOryxActiveStatusEffect& Effect);

	/** Pushes the MovementSpeed modifier for Chill (Stats path); returns the modifier GUID. */
	FGuid ApplyChillModifier(const FOryxStatusEffectSpec& Spec) const;

	/** Removes a stat-mod by GUID from the target's StatsComponent. */
	void RemoveStatModifier(const FGuid& ModifierId) const;

	/** Recomputes MaxWalkSpeed for owners with no StatsComponent (enemies). */
	void RecalcDirectMovementSpeed();

	UPROPERTY(Transient)
	UCharacterMovementComponent* MovementComp = nullptr;

	/** Cached base MaxWalkSpeed for the direct (no-Stats) path. Captured at BeginPlay. */
	float BaseMaxWalkSpeed = 0.f;

	/** Final cleanup hook — broadcast + free up stat-mod resources. */
	void HandleEffectExpired(const FOryxActiveStatusEffect& Effect);

	UPROPERTY(Transient)
	UOryxHealthComponent* HealthComp = nullptr;

	UPROPERTY(Transient)
	UOryxStatsComponent* StatsComp = nullptr;

	TArray<FOryxActiveStatusEffect> ActiveEffects;
};
