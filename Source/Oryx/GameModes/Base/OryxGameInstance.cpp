#include "OryxGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/OryxProfileSave.h"
#include "SaveGame/OryxRunSave.h"
#include "States/Player/OryxPlayerState.h"
#include "Component/Level/OryxLevelComponent.h"
#include "Component/SkillTree/OryxSkillTreeComponent.h"

void UOryxGameInstance::Init()
{
	Super::Init();
	LoadProfile();
}

UOryxProfileSave* UOryxGameInstance::LoadProfile()
{
	if (CachedProfile) return CachedProfile;

	const FString SlotName = UOryxProfileSave::GetSlotName();
	const int32 UserIndex = UOryxProfileSave::GetUserIndex();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex))
		{
			CachedProfile = Cast<UOryxProfileSave>(Loaded);
		}
	}

	if (!CachedProfile)
	{
		CachedProfile = Cast<UOryxProfileSave>(
			UGameplayStatics::CreateSaveGameObject(UOryxProfileSave::StaticClass()));
	}

	// Sync the volatile SelectedClass to the persisted LastPlayedClass so a relaunch defaults
	// to the user's most recent class. WBP_CharacterSelection overwrites this if the user picks a different one.
	if (CachedProfile)
	{
		SelectedClass = CachedProfile->LastPlayedClass;
	}

	return CachedProfile;
}

bool UOryxGameInstance::SaveProfile(AOryxPlayerState* FromPlayerState)
{
	if (!FromPlayerState) return false;
	if (!CachedProfile) LoadProfile();
	if (!CachedProfile) return false;

	if (UOryxLevelComponent* LC = FromPlayerState->GetLevelComponent())
	{
		CachedProfile->ClassProgression = LC->GetAllProgression();
		CachedProfile->LastPlayedClass = LC->ActiveClass;
	}

	if (UOryxSkillTreeComponent* STC = FromPlayerState->GetSkillTreeComponent())
	{
		CachedProfile->SkillAllocations = STC->GetAllAllocations();
	}

	const bool bOK = UGameplayStatics::SaveGameToSlot(
		CachedProfile,
		UOryxProfileSave::GetSlotName(),
		UOryxProfileSave::GetUserIndex());

	UE_LOG(LogTemp, Log, TEXT("OryxGameInstance::SaveProfile → %s"), bOK ? TEXT("OK") : TEXT("FAILED"));
	return bOK;
}

void UOryxGameInstance::ApplyProfileToPlayerState(AOryxPlayerState* PlayerState)
{
	if (!PlayerState) return;
	if (!CachedProfile) LoadProfile();
	if (!CachedProfile) return;

	if (UOryxLevelComponent* LC = PlayerState->GetLevelComponent())
	{
		for (const TPair<EOryxClass, FOryxClassProgression>& Pair : CachedProfile->ClassProgression)
		{
			LC->SetProgression(Pair.Key, Pair.Value);
		}
		// Prefer the volatile SelectedClass (set by character select) over the saved LastPlayedClass.
		// This ensures XP routes to the class the player just picked, not the one from the last session.
		LC->SetActiveClass(SelectedClass);
	}

	if (UOryxSkillTreeComponent* STC = PlayerState->GetSkillTreeComponent())
	{
		for (const TPair<EOryxClass, FOryxSkillTreeAllocations>& Pair : CachedProfile->SkillAllocations)
		{
			STC->SetAllocations(Pair.Key, Pair.Value);
		}
		// Stat-modifier rehydration happens later, at AOryxCharacter::PossessedBy →
		// SkillTreeComponent::ReapplyAllocations(ActiveClass, GI->RegisteredSkillTrees).
		// It can't happen here: at PlayerState BeginPlay the pawn isn't possessed yet, so the
		// run-scope StatsComponent isn't reachable.
	}
}

bool UOryxGameInstance::HasRunSave() const
{
	return UGameplayStatics::DoesSaveGameExist(
		UOryxRunSave::GetSlotName(),
		UOryxRunSave::GetUserIndex());
}

void UOryxGameInstance::DeleteRunSave()
{
	UGameplayStatics::DeleteGameInSlot(
		UOryxRunSave::GetSlotName(),
		UOryxRunSave::GetUserIndex());
}
