#include "OryxPickup_Gold.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/Player/OryxCharacter.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Kismet/GameplayStatics.h"

AOryxPickup_Gold::AOryxPickup_Gold()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(60.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AOryxPickup_Gold::OnSphereOverlap);
	RootComponent = CollisionSphere;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AOryxPickup_Gold::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) return;

	const FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	const float Dist = ToPlayer.Size();
	if (Dist <= MagnetRadius)
	{
		const FVector Dir = ToPlayer.GetSafeNormal();
		SetActorLocation(GetActorLocation() + Dir * MagnetSpeed * DeltaSeconds);
	}
}

void AOryxPickup_Gold::OnSphereOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	AOryxCharacter* Player = Cast<AOryxCharacter>(OtherActor);
	if (!Player) return;

	if (UOryxCurrencyComponent* Currency = Player->GetCurrencyComponent())
	{
		Currency->AddGold(Value);
	}

	Destroy();
}
