#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OryxProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ORYX_API AOryxProjectile : public AActor
{
    GENERATED_BODY()

public:
    AOryxProjectile();

protected:
    // collision
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComp;

    // movement
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Combat")
    float Damage = 20.f;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

public:
    FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
    FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};