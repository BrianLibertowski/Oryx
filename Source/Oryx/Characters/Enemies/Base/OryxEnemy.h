#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OryxEnemy.generated.h"

class UOryxHealthComponent;
class UOryxStatusEffectsComponent;
class AOryxPickup_Gold;

UCLASS()
class ORYX_API AOryxEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AOryxEnemy();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Components")
    UOryxHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Components")
    UOryxStatusEffectsComponent* StatusEffectsComponent;

    // Gold pickup class to drop on death — set in BP_OryxEnemy
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    TSubclassOf<AOryxPickup_Gold> GoldPickupClass;

    // Number of gold coins dropped on death
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    int32 GoldDropCount = 1;

    // Per-coin value
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    int32 GoldDropValue = 1;

    // --- Combat ---

    /** Damage dealt per melee swing. Read by BTT_MeleeAttack. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Combat")
    float AttackDamage = 15.f;

    /** Max distance (cm) at which the enemy can land a melee hit. Read by BTT_MeleeAttack. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Combat")
    float AttackRange = 150.f;

    // --- Leash ---

    /** How far this enemy can wander from its spawn before dropping aggro (cm). 0 = no leash. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|AI")
    float LeashRange = 2000.f;

    /** World location captured at BeginPlay — the "post" this enemy returns to */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|AI")
    FVector SpawnLocation = FVector::ZeroVector;

    // --- Shared aggro ---

    /** When this enemy takes damage, allies within this radius (cm) also aggro. 0 = no spread. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|AI")
    float AlertRadius = 1500.f;

    /** True when forced into aggro by an external trigger (damage spread, camp proximity).
     *  BT uses ForcedTarget instead of normal aggro-range search while this is true. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|AI")
    bool bForcedAggro = false;

    /** Target the enemy is forcibly aggroed onto (set by MarkAlerted). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|AI")
    AActor* ForcedTarget = nullptr;

public:
    UFUNCTION(BlueprintPure, Category = "Oryx|Combat")
    float GetAttackDamage() const { return AttackDamage; }

    UFUNCTION(BlueprintPure, Category = "Oryx|Combat")
    float GetAttackRange() const { return AttackRange; }

    UFUNCTION(BlueprintPure, Category = "Oryx|AI")
    FVector GetSpawnLocation() const { return SpawnLocation; }

    UFUNCTION(BlueprintPure, Category = "Oryx|AI")
    float GetLeashRange() const { return LeashRange; }

    UFUNCTION(BlueprintPure, Category = "Oryx|AI")
    bool IsBeyondLeash() const;

    UFUNCTION(BlueprintPure, Category = "Oryx|AI")
    bool IsForcedAggro() const { return bForcedAggro; }

    UFUNCTION(BlueprintPure, Category = "Oryx|AI")
    AActor* GetForcedTarget() const { return ForcedTarget; }

    /** Called by encounter / damage spread / proximity volumes to lock aggro onto a target. */
    UFUNCTION(BlueprintCallable, Category = "Oryx|AI")
    void MarkAlerted(AActor* InTarget);

    /** Drops forced aggro state. Called by ReturnToSpawn on arrival home and by FindNearestPlayer when leashed. */
    UFUNCTION(BlueprintCallable, Category = "Oryx|AI")
    void ClearAggro();

private:
    UFUNCTION()
    void HandleDeath(AActor* DeadActor);

    UFUNCTION()
    void HandleDamaged(float NewHealth, float DamageAmount);
};