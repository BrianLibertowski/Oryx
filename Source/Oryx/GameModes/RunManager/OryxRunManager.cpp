#include "OryxRunManager.h"
#include "EngineUtils.h"
#include "States/Player/OryxPlayerState.h"
#include "GameModes/Base/OryxGameInstance.h"
#include "Characters/Player/OryxCharacter.h"
#include "Component/Health/OryxHealthComponent.h"
#include "GameFramework/PlayerController.h"

AOryxRunManager::AOryxRunManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

float AOryxRunManager::GetRunDurationSeconds() const
{
	const UWorld* World = GetWorld();
	if (!World) return 0.f;
	return World->GetTimeSeconds() - RunStartTime;
}

void AOryxRunManager::StartRun(int32 Stage)
{
	CurrentStage = FMath::Max(1, Stage);

	if (const UWorld* World = GetWorld())
	{
		RunStartTime = World->GetTimeSeconds();
	}

	PropagateStageToPlayers();
	SetRunState(EOryxRunState::InRun);
}

void AOryxRunManager::HandleBossDefeated()
{
	if (RunState != EOryxRunState::InRun) return;
	SetRunState(EOryxRunState::BossDefeated);
}

void AOryxRunManager::StartWaveMode()
{
	if (RunState != EOryxRunState::BossDefeated) return;
	SetRunState(EOryxRunState::WaveMode);
}

void AOryxRunManager::HandleExtract()
{
	if (RunState != EOryxRunState::BossDefeated && RunState != EOryxRunState::WaveMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunManager: Extract called from invalid state — ignored."));
		return;
	}
	PersistProfileAndCleanupRun();
	SetRunState(EOryxRunState::RunComplete);
}

void AOryxRunManager::HandlePlayerDeath()
{
	// Co-op proofing: count alive players. Only end the run when ALL are down.
	// Single-player: trivially 1 player → 0 alive after this call → run ends. Behavior unchanged.
	// Co-op: a single player's death keeps the run going (downed-state / revive-shrine work is D19 follow-up).
	UWorld* World = GetWorld();
	if (!World)
	{
		// Defensive — without a world we can't validate; mark dead so we don't hang.
		SetRunState(EOryxRunState::Dead);
		return;
	}

	int32 AlivePlayerCount = 0;
	for (TActorIterator<AOryxCharacter> It(World); It; ++It)
	{
		AOryxCharacter* Player = *It;
		if (!IsValid(Player)) continue;
		if (UOryxHealthComponent* HC = Player->GetHealthComponent())
		{
			if (!HC->IsDead())
			{
				++AlivePlayerCount;
			}
		}
	}

	if (AlivePlayerCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("RunManager::HandlePlayerDeath — %d player(s) still alive, run continues."), AlivePlayerCount);
		return;
	}

	// All players down: persist profile(s) and end the run.
	PersistProfileAndCleanupRun();
	SetRunState(EOryxRunState::Dead);
}

void AOryxRunManager::PersistProfileAndCleanupRun()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UOryxGameInstance* GI = Cast<UOryxGameInstance>(World->GetGameInstance());
	if (!GI) return;

	// Co-op proof: save the LOCAL machine's player profile(s). On a host, that's host's PS only;
	// remote clients save their own profiles on their own machines when their own RunManager fires.
	// Single-player: one PS, always local, saves correctly. Single iterator hit; same behavior.
	for (TActorIterator<AOryxPlayerState> It(World); It; ++It)
	{
		AOryxPlayerState* PS = *It;
		if (!PS) continue;

		const APlayerController* PC = PS->GetPlayerController();
		if (PC && PC->IsLocalController())
		{
			GI->SaveProfile(PS);
		}
	}

	GI->DeleteRunSave();
}

void AOryxRunManager::PropagateStageToPlayers()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<AOryxPlayerState> It(World); It; ++It)
	{
		if (AOryxPlayerState* PS = *It)
		{
			PS->CurrentStage = CurrentStage;
		}
	}
}

void AOryxRunManager::SetRunState(EOryxRunState NewState)
{
	if (RunState == NewState) return;
	const EOryxRunState OldState = RunState;
	RunState = NewState;
	OnRunStateChanged.Broadcast(NewState, OldState);
}
