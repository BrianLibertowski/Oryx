// Copyright Epic Games, Inc. All Rights Reserved.

#include "OryxGameMode.h"
#include "OryxGameInstance.h"
#include "States/Player/OryxPlayerState.h"
#include "GameModes/RunManager/OryxRunManager.h"

AOryxGameMode::AOryxGameMode()
{
	// Co-op proof: player-persistent state (currency, etc.) lives on PlayerState
	PlayerStateClass = AOryxPlayerState::StaticClass();

	// Phase 3: GameState owns run lifecycle state machine (RunState, CurrentStage, OnRunStateChanged).
	GameStateClass = AOryxRunManager::StaticClass();
}

UClass* AOryxGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (UOryxGameInstance* GI = GetGameInstance<UOryxGameInstance>())
	{
		if (GI->SelectedPawnClass)
		{
			return GI->SelectedPawnClass;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
