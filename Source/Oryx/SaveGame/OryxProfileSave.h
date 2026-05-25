#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Characters/OryxClassTypes.h"
#include "Component/SkillTree/OryxSkillTreeComponent.h"
#include "OryxProfileSave.generated.h"

/**
 *  Permanent profile save (D16). Persists across game restarts. ONE slot per player.
 *
 *  Holds:
 *    - Per-class XP / Level / unallocated points (FOryxClassProgression × N classes)
 *    - Per-class skill tree allocations (FOryxSkillTreeAllocations × N classes)
 *    - Lifetime stats (total runs, total kills)
 *    - Version int for migration
 *
 *  Save flow:
 *    UGameplayStatics::CreateSaveGameObject(UOryxProfileSave::StaticClass())
 *    → fill fields from LevelComponent + SkillTreeComponent
 *    → UGameplayStatics::SaveGameToSlot(..., "ProfileSave", 0)
 *
 *  Load flow:
 *    UGameplayStatics::LoadGameFromSlot("ProfileSave", 0)
 *    → cast to UOryxProfileSave
 *    → check Version, run migration if needed
 *    → push ClassProgression to LevelComponent.SetProgression(...)
 *    → push SkillAllocations to SkillTreeComponent.SetAllocations(...)
 */
UCLASS()
class ORYX_API UOryxProfileSave : public USaveGame
{
	GENERATED_BODY()

public:
	/** Schema version. Bump on breaking changes; add migration logic in caller's load path. */
	UPROPERTY()
	int32 Version = 1;

	/** Per-class progression records (XP / Level / unallocated points). */
	UPROPERTY()
	TMap<EOryxClass, FOryxClassProgression> ClassProgression;

	/** Per-class skill tree allocations (which nodes invested + applied modifier GUIDs). */
	UPROPERTY()
	TMap<EOryxClass, FOryxSkillTreeAllocations> SkillAllocations;

	/** Lifetime telemetry (informational, not gameplay-affecting). */
	UPROPERTY()
	int32 TotalRunsCompleted = 0;

	UPROPERTY()
	int32 TotalKills = 0;

	UPROPERTY()
	int32 TotalGoldEarned = 0;

	/** Last-played class — restored as ActiveClass on game start. */
	UPROPERTY()
	EOryxClass LastPlayedClass = EOryxClass::Warrior;

	/** Slot name convention. */
	static const FString GetSlotName() { return TEXT("ProfileSave"); }
	static const int32 GetUserIndex() { return 0; }
};
