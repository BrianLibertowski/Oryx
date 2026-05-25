#include "OryxEncounter_EliteArena.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

#include "Characters/Player/OryxCharacter.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "Component/Health/OryxHealthComponent.h"

AOryxEncounter_EliteArena::AOryxEncounter_EliteArena()
{
	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	TriggerVolume->InitSphereRadius(TriggerRadius);
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = TriggerVolume;

	// Elite arena defaults: 1 elite, large reward.
	EnemyCount = 1;
	SpawnRadius = 600.f;
	GoldReward = 200;
	XPReward = 400;
}

void AOryxEncounter_EliteArena::BeginPlay()
{
	Super::BeginPlay();

	// Update the trigger radius in case it was changed in editor after construct.
	if (TriggerVolume)
	{
		TriggerVolume->SetSphereRadius(TriggerRadius);
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AOryxEncounter_EliteArena::HandleTriggerOverlap);
	}
}

void AOryxEncounter_EliteArena::HandleTriggerOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (bActive || bCleared) return;

	AOryxCharacter* Player = Cast<AOryxCharacter>(OtherActor);
	if (!Player) return;

	StartEncounter(Player);
	ApplyEliteMultipliers();
}

void AOryxEncounter_EliteArena::ApplyEliteMultipliers()
{
	for (AOryxEnemy* Enemy : SpawnedEnemies)
	{
		if (!IsValid(Enemy)) continue;

		// HP buff via HealthComponent::IncreaseMaxHealth (routes through Stats if available).
		if (UOryxHealthComponent* HC = Enemy->FindComponentByClass<UOryxHealthComponent>())
		{
			const float CurrentMax = HC->GetMaxHealth();
			const float Bonus = CurrentMax * (EliteHealthMultiplier - 1.f);
			if (Bonus > 0.f) HC->IncreaseMaxHealth(Bonus);
		}

		// Visual scale via root mesh component. Enemy is ACharacter; scale the SkeletalMesh root.
		if (USceneComponent* Mesh = Enemy->GetRootComponent())
		{
			const FVector S = Mesh->GetRelativeScale3D();
			Mesh->SetRelativeScale3D(S * EliteScaleMultiplier);
		}

		// Damage scaling via SetAttackDamage (added to AOryxEnemy 2026-05-25).
		const float CurrentDamage = Enemy->GetAttackDamage();
		Enemy->SetAttackDamage(CurrentDamage * EliteDamageMultiplier);
	}
}
