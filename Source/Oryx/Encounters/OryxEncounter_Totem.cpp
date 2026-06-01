#include "OryxEncounter_Totem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Characters/Player/OryxCharacter.h"
#include "Component/InteractPrompt/OryxInteractPromptComponent.h"

AOryxEncounter_Totem::AOryxEncounter_Totem()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = MeshComponent;

	InteractPromptComponent = CreateDefaultSubobject<UOryxInteractPromptComponent>(TEXT("InteractPromptComponent"));
}

void AOryxEncounter_Totem::BeginPlay()
{
	Super::BeginPlay();
	ConfigureForDifficulty();
}

void AOryxEncounter_Totem::ConfigureForDifficulty()
{
	const int32 Idx = static_cast<int32>(Difficulty);

	// Defensive: array could be edited/empty in BP. Fall back to current values if out of range.
	if (EnemyCountPerDifficulty.IsValidIndex(Idx))
	{
		EnemyCount = EnemyCountPerDifficulty[Idx];
	}
	if (GoldRewardPerDifficulty.IsValidIndex(Idx))
	{
		GoldReward = GoldRewardPerDifficulty[Idx];
	}
	if (XPRewardPerDifficulty.IsValidIndex(Idx))
	{
		XPReward = XPRewardPerDifficulty[Idx];
	}

	// Apply per-difficulty mesh if configured. Leaves the default mesh untouched otherwise.
	if (MeshComponent)
	{
		if (const TObjectPtr<UStaticMesh>* Found = MeshPerDifficulty.Find(Difficulty))
		{
			if (*Found)
			{
				MeshComponent->SetStaticMesh(*Found);
			}
		}
	}
}

void AOryxEncounter_Totem::Interact_Implementation(AOryxCharacter* Interactor)
{
	if (bActive || bCleared) return;
	StartEncounter(Interactor);
}
