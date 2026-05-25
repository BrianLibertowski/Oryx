#include "OryxRunManager.h"
#include "EngineUtils.h"
#include "States/Player/OryxPlayerState.h"

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
	SetRunState(EOryxRunState::RunComplete);
}

void AOryxRunManager::HandlePlayerDeath()
{
	// Solo death = run ends per D19. Co-op revive shrine deferred — for now any death transitions.
	SetRunState(EOryxRunState::Dead);
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
