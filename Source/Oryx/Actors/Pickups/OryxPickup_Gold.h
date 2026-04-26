#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OryxPickup_Gold.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class ORYX_API AOryxPickup_Gold : public AActor
{
	GENERATED_BODY()

public:
	AOryxPickup_Gold();

	virtual void Tick(float DeltaSeconds) override;

	// Amount of gold granted on pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Pickup")
	int32 Value = 1;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	// Distance at which pickup magnetizes toward the player
	UPROPERTY(EditAnywhere, Category = "Oryx|Pickup")
	float MagnetRadius = 250.f;

	// Speed of magnet pull (cm/s)
	UPROPERTY(EditAnywhere, Category = "Oryx|Pickup")
	float MagnetSpeed = 800.f;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
