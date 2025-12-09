#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OryxEnemy.generated.h"

/**
 * Basic enemy character for Oryx
 * - Has health
 * - Can take damage from the player
 */
UCLASS()
class ORYX_API AOryxEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AOryxEnemy();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Maximum health of the enemy */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Enemy|Stats")
    float MaxHealth = 50.f;

    /** Current health of the enemy */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Enemy|Stats")
    float CurrentHealth;

public:
    /** Apply damage from weapons / abilities */
    UFUNCTION(BlueprintCallable, Category = "Oryx|Enemy|Stats")
    void ApplyDamage(float DamageAmount);

protected:
    /** Called when health reaches zero */
    void HandleDeath();
};