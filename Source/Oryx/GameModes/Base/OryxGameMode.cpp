// Copyright Epic Games, Inc. All Rights Reserved.

#include "OryxGameMode.h"
#include "OryxGameInstance.h"

AOryxGameMode::AOryxGameMode()
{
	// stub
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
