#include "OryxBoss.h"
#include "Component/Health/OryxHealthComponent.h"
#include "GameModes/RunManager/OryxRunManager.h"
#include "TimerManager.h"

AOryxBoss::AOryxBoss()
{
	// Default boss stats — override per-subclass.
	AttackDamage = 35.f;
	AttackRange = 250.f;
	LeashRange = 0.f; // Bosses don't leash by default; arena is the bound.
}

void AOryxBoss::BeginPlay()
{
	Super::BeginPlay();

	ThresholdsFired.Init(false, HealthThresholds.Num());

	if (UOryxHealthComponent* HC = FindComponentByClass<UOryxHealthComponent>())
	{
		HC->OnDamaged.AddDynamic(this, &AOryxBoss::HandleBossDamaged);
		// Note: Super (AOryxEnemy) also binds its own HandleDeath for the cleanup/loot path.
		// Boss-specific RunManager notification rides alongside it.
		HC->OnDeath.AddDynamic(this, &AOryxBoss::HandleBossDied);
	}
}

void AOryxBoss::HandleBossDied(AActor* /*DeadActor*/)
{
	if (UWorld* World = GetWorld())
	{
		if (AOryxRunManager* RM = World->GetGameState<AOryxRunManager>())
		{
			RM->HandleBossDefeated();
		}
	}
}

void AOryxBoss::HandleBossDamaged(float NewHealth, float /*DamageAmount*/)
{
	UOryxHealthComponent* HC = FindComponentByClass<UOryxHealthComponent>();
	if (!HC) return;

	const float Max = HC->GetMaxHealth();
	if (Max <= 0.f) return;

	const float Fraction = NewHealth / Max;

	for (int32 i = 0; i < HealthThresholds.Num(); ++i)
	{
		if (ThresholdsFired.IsValidIndex(i) && !ThresholdsFired[i] && Fraction <= HealthThresholds[i])
		{
			ThresholdsFired[i] = true;
			OnHealthThreshold.Broadcast(i, Fraction);

			if (FollowerSpawnClass && FollowersPerThreshold > 0)
			{
				SpawnFollowers();
			}
		}
	}
}

void AOryxBoss::TelegraphAttack(float WindupSeconds)
{
	if (bIsTelegraphingAttack) return;
	bIsTelegraphingAttack = true;
	OnTelegraphAttack.Broadcast(WindupSeconds);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TelegraphTimer, this,
			&AOryxBoss::FinishTelegraphedAttack, FMath::Max(0.01f, WindupSeconds), false);
	}
}

void AOryxBoss::FinishTelegraphedAttack()
{
	bIsTelegraphingAttack = false;
	// BT/BP reads bIsTelegraphingAttack=false as the cue to actually swing.
	// No delegate here — the swing is the BT's next normal melee attack.
}

void AOryxBoss::SpawnFollowers()
{
	UWorld* World = GetWorld();
	if (!World || !FollowerSpawnClass) return;

	const FVector Origin = GetActorLocation();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = this;

	for (int32 i = 0; i < FollowersPerThreshold; ++i)
	{
		const float Angle = FMath::FRandRange(0.f, 2.f * PI);
		const float R = FMath::Sqrt(FMath::FRand()) * FollowerSpawnRadius;
		const FVector SpawnLoc = Origin + FVector(FMath::Cos(Angle) * R, FMath::Sin(Angle) * R, 0.f);
		World->SpawnActor<AOryxEnemy>(FollowerSpawnClass, SpawnLoc, FRotator::ZeroRotator, Params);
	}
}
