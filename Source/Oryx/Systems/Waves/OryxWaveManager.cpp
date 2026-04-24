#include "OryxWaveManager.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Engine/World.h"

AOryxWaveManager::AOryxWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOryxWaveManager::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStartFirstWave)
	{
		StartNextWave();
	}
}

void AOryxWaveManager::StartNextWave()
{
	if (!EnemyClass || SpawnPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxWaveManager: No EnemyClass or SpawnPoints set."));
		return;
	}

	CurrentWave++;
	const int32 EnemiesToSpawn = BaseEnemiesPerWave + (CurrentWave - 1);
	EnemiesAlive = 0;

	OnWaveStarted.Broadcast(CurrentWave);

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < EnemiesToSpawn; i++)
	{
		// Cycle through available spawn points
		AActor* SpawnPoint = SpawnPoints[i % SpawnPoints.Num()];
		if (!SpawnPoint) continue;

		FTransform SpawnTransform = SpawnPoint->GetActorTransform();
		AOryxEnemy* Enemy = World->SpawnActor<AOryxEnemy>(EnemyClass, SpawnTransform, SpawnParams);

		if (Enemy)
		{
			UOryxHealthComponent* HealthComp = Enemy->FindComponentByClass<UOryxHealthComponent>();
			if (HealthComp)
			{
				HealthComp->OnDeath.AddDynamic(this, &AOryxWaveManager::HandleEnemyDeath);
				EnemiesAlive++;
			}
		}
	}
}

void AOryxWaveManager::HandleEnemyDeath(AActor* DeadActor)
{
	EnemiesAlive = FMath::Max(0, EnemiesAlive - 1);

	if (EnemiesAlive <= 0)
	{
		OnWaveComplete.Broadcast(CurrentWave);
	}
}
