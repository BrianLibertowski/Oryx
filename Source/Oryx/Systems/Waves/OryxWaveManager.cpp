#include "OryxWaveManager.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AOryxWaveManager::AOryxWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOryxWaveManager::BeginPlay()
{
	Super::BeginPlay();

	// Register any enemies already placed in the level so they count toward wave completion
	TArray<AActor*> PlacedEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOryxEnemy::StaticClass(), PlacedEnemies);
	for (AActor* Actor : PlacedEnemies)
	{
		RegisterEnemy(Cast<AOryxEnemy>(Actor));
	}

	if (bAutoStartFirstWave)
	{
		StartNextWave();
	}
}

void AOryxWaveManager::RegisterEnemy(AOryxEnemy* Enemy)
{
	if (!Enemy) return;

	UOryxHealthComponent* HealthComp = Enemy->FindComponentByClass<UOryxHealthComponent>();
	if (!HealthComp) return;

	HealthComp->OnDeath.AddDynamic(this, &AOryxWaveManager::HandleEnemyDeath);
	EnemiesAlive++;
}

void AOryxWaveManager::StartNextWave()
{
	if (EnemyPool.IsEmpty() || SpawnPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxWaveManager: EnemyPool or SpawnPoints is empty."));
		return;
	}

	CurrentWave++;
	const int32 EnemiesToSpawn = BaseEnemiesPerWave + (CurrentWave - 1);

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

		TSubclassOf<AOryxEnemy> PickedClass = EnemyPool[FMath::RandRange(0, EnemyPool.Num() - 1)];
		if (!PickedClass) continue;

		AOryxEnemy* Enemy = World->SpawnActor<AOryxEnemy>(PickedClass, SpawnTransform, SpawnParams);

		RegisterEnemy(Enemy);
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
