#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OryxEnemy.generated.h"

class UOryxHealthComponent;

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

private:
    UFUNCTION()
    void HandleDeath(AActor* DeadActor);
};