#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OryxEnemy.generated.h"

class UOryxHealthComponent;
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

    // Gold pickup class to drop on death — set in BP_OryxEnemy
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    TSubclassOf<AOryxPickup_Gold> GoldPickupClass;

    // Number of gold coins dropped on death
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    int32 GoldDropCount = 1;

    // Per-coin value
    UPROPERTY(EditDefaultsOnly, Category = "Oryx|Loot")
    int32 GoldDropValue = 1;

private:
    UFUNCTION()
    void HandleDeath(AActor* DeadActor);
};