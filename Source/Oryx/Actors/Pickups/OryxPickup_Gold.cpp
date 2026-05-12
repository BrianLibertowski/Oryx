#include "OryxPickup_Gold.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/Player/OryxCharacter.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "EngineUtils.h"

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

	// Co-op proof: magnet to NEAREST player, not player index 0.
	AOryxCharacter* Nearest = nullptr;
	float NearestDistSq = MagnetRadius * MagnetRadius;
	for (TActorIterator<AOryxCharacter> It(GetWorld()); It; ++It)
	{
		AOryxCharacter* Candidate = *It;
		if (!IsValid(Candidate)) continue;

		const float DistSq = FVector::DistSquared(Candidate->GetActorLocation(), GetActorLocation());
		if (DistSq <= NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Candidate;
		}
	}
	if (!Nearest) return;

	const FVector Dir = (Nearest->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	SetActorLocation(GetActorLocation() + Dir * MagnetSpeed * DeltaSeconds);
}

void AOryxPickup_Gold::OnSphereOverlap(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	AOryxCharacter* Player = Cast<AOryxCharacter>(OtherActor);
	if (!Player) return;

	// Routes through PlayerState — survives pawn death, replicates per-player in co-op.
	if (UOryxCurrencyComponent* Currency = Player->GetCurrencyComponent())
	{
		float GoldGain = 0.f;
		if (UOryxStatsComponent* Stats = Player->GetStatsComponent())
		{
			GoldGain = Stats->GetStat(EOryxStat::GoldGain);
		}
		const int32 Granted = FMath::Max(0, FMath::RoundToInt(Value * (1.f + GoldGain)));
		Currency->AddGold(Granted);
		Destroy();
	}
	// If currency is null (e.g. PlayerState not yet replicated), don't destroy — let next overlap retry.
}
