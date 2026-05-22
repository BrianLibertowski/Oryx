#include "OryxEncounter.h"

#include "EngineUtils.h"

#include "Characters/Player/OryxCharacter.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "States/Player/OryxPlayerState.h"
#include "Actors/Pickups/OryxRewardPickup.h"

AOryxEncounter::AOryxEncounter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOryxEncounter::StartEncounter(AOryxCharacter* InstigatingPlayer)
{
	if (bActive || bCleared) return;
	if (EnemyPool.Num() == 0 || EnemyCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AOryxEncounter '%s': StartEncounter called with empty EnemyPool or EnemyCount<=0"), *GetName());
		return;
	}

	// Co-op note: when bReplicates is added, gate this whole function on HasAuthority().
	// All spawning + tracking happens server-side; clients see the result via replicated state.

	UWorld* World = GetWorld();
	if (!World) return;

	bActive = true;
	TriggeringPlayer = InstigatingPlayer;
	RemainingEnemies = 0;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = this;

	for (int32 i = 0; i < EnemyCount; ++i)
	{
		TSubclassOf<AOryxEnemy> EnemyClass = PickEnemyClass();
		if (!EnemyClass) continue;

		const FVector SpawnLoc = GetNextSpawnLocation(i);
		const FRotator SpawnRot = FRotator::ZeroRotator;

		AOryxEnemy* Spawned = World->SpawnActor<AOryxEnemy>(EnemyClass, SpawnLoc, SpawnRot, Params);
		if (!Spawned) continue;

		SpawnedEnemies.Add(Spawned);

		// Bind OnDeath so we count down as they die
		if (UOryxHealthComponent* HC = Spawned->FindComponentByClass<UOryxHealthComponent>())
		{
			HC->OnDeath.AddDynamic(this, &AOryxEncounter::HandleEnemyDeath);
			++RemainingEnemies;
		}
		else
		{
			// Enemy with no health component is unkillable — don't track it
			UE_LOG(LogTemp, Warning, TEXT("AOryxEncounter '%s': Spawned enemy '%s' has no UOryxHealthComponent — not tracked"), *GetName(), *Spawned->GetName());
		}
	}

	if (RemainingEnemies == 0)
	{
		// Nothing to track — complete immediately so we don't softlock
		CompleteEncounter();
	}
}

void AOryxEncounter::BroadcastAlert(AActor* Target)
{
	if (!IsValid(Target)) return;

	for (AOryxEnemy* Enemy : SpawnedEnemies)
	{
		if (!IsValid(Enemy)) continue;
		if (UOryxHealthComponent* HC = Enemy->FindComponentByClass<UOryxHealthComponent>())
		{
			if (HC->IsDead()) continue;
		}
		Enemy->MarkAlerted(Target);
	}
}

void AOryxEncounter::HandleEnemyDeath(AActor* /*DeadActor*/)
{
	if (!bActive || bCleared) return;

	--RemainingEnemies;
	if (RemainingEnemies <= 0)
	{
		CompleteEncounter();
	}
}

void AOryxEncounter::CompleteEncounter()
{
	bActive = false;
	bCleared = true;

	// --- Gold reward ---
	if (GoldReward > 0)
	{
		AOryxCharacter* Recipient = IsValid(TriggeringPlayer) ? TriggeringPlayer : nullptr;

		// No triggering player (e.g. auto-start camp) → give to nearest alive player
		if (!Recipient)
		{
			float BestDistSq = TNumericLimits<float>::Max();
			const FVector SelfLoc = GetActorLocation();
			for (TActorIterator<AOryxCharacter> It(GetWorld()); It; ++It)
			{
				AOryxCharacter* Candidate = *It;
				if (!IsValid(Candidate)) continue;
				if (UOryxHealthComponent* HC = Candidate->GetHealthComponent())
				{
					if (HC->IsDead()) continue;
				}
				const float DistSq = FVector::DistSquared(Candidate->GetActorLocation(), SelfLoc);
				if (DistSq < BestDistSq)
				{
					BestDistSq = DistSq;
					Recipient = Candidate;
				}
			}
		}

		if (Recipient)
		{
			if (UOryxCurrencyComponent* Currency = Recipient->GetCurrencyComponent())
			{
				Currency->AddGold(GoldReward);
			}
		}
	}

	// --- XP reward ---
	if (XPReward > 0 && IsValid(TriggeringPlayer))
	{
		if (AOryxPlayerState* PS = TriggeringPlayer->GetPlayerState<AOryxPlayerState>())
		{
			if (UOryxLevelComponent* Level = PS->GetLevelComponent())
			{
				Level->AddXP(XPReward);
			}
		}
	}

	// --- Reward pickup (interactable, non-blocking) ---
	if (RewardPickupClass)
	{
		if (UWorld* World = GetWorld())
		{
			const FVector SpawnLoc = GetActorLocation() + RewardPickupOffset;
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			World->SpawnActor<AOryxRewardPickup>(RewardPickupClass, SpawnLoc, FRotator::ZeroRotator, Params);
		}
	}

	OnEncounterComplete.Broadcast(this);
}

FVector AOryxEncounter::GetNextSpawnLocation(int32 SlotIndex) const
{
	const FVector Origin = GetActorLocation();

	// Explicit offsets win when set
	if (SpawnOffsets.Num() > 0)
	{
		return Origin + SpawnOffsets[SlotIndex % SpawnOffsets.Num()];
	}

	// Otherwise random within SpawnRadius (XY only — no vertical scatter)
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float R = FMath::Sqrt(FMath::FRand()) * SpawnRadius; // sqrt for uniform area distribution
	return Origin + FVector(FMath::Cos(Angle) * R, FMath::Sin(Angle) * R, 0.f);
}

TSubclassOf<AOryxEnemy> AOryxEncounter::PickEnemyClass() const
{
	if (EnemyPool.Num() == 0) return nullptr;
	const int32 Idx = FMath::RandRange(0, EnemyPool.Num() - 1);
	return EnemyPool[Idx];
}
