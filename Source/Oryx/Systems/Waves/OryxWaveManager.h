#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OryxWaveManager.generated.h"

class AOryxEnemy;
class UOryxHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveComplete, int32, CompletedWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, NewWave);

UCLASS()
class ORYX_API AOryxWaveManager : public AActor
{
	GENERATED_BODY()

public:
	AOryxWaveManager();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Oryx|Waves")
	void StartNextWave();

	UFUNCTION(BlueprintPure, Category = "Oryx|Waves")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Waves")
	int32 GetEnemiesAlive() const { return EnemiesAlive; }

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Waves")
	FOnWaveComplete OnWaveComplete;

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Waves")
	FOnWaveStarted OnWaveStarted;

protected:
	// Enemy class to spawn each wave
	UPROPERTY(EditAnywhere, Category = "Oryx|Waves")
	TSubclassOf<AOryxEnemy> EnemyClass;

	// Spawn point actors placed in the level
	UPROPERTY(EditAnywhere, Category = "Oryx|Waves")
	TArray<AActor*> SpawnPoints;

	// Base number of enemies on wave 1 — scales by wave number
	UPROPERTY(EditAnywhere, Category = "Oryx|Waves")
	int32 BaseEnemiesPerWave = 3;

	// Auto-start wave 1 on BeginPlay
	UPROPERTY(EditAnywhere, Category = "Oryx|Waves")
	bool bAutoStartFirstWave = true;

private:
	int32 CurrentWave = 0;
	int32 EnemiesAlive = 0;

	UFUNCTION()
	void HandleEnemyDeath(AActor* DeadActor);
};
