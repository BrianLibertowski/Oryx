#pragma once

#include "CoreMinimal.h"
#include "OryxClassTypes.generated.h"

/**
 *  Class identity per D12. 4 classes unlocked at start; Berserker = future Warrior subclass post-demo.
 *  Drives per-class XP routing (D17), skill tree allocations (D14), and class-specific gameplay tuning.
 */
UENUM(BlueprintType)
enum class EOryxClass : uint8
{
	Warrior  UMETA(DisplayName = "Warrior"),
	Archer   UMETA(DisplayName = "Archer"),
	Assassin UMETA(DisplayName = "Assassin"),
	Mage     UMETA(DisplayName = "Mage")
};

/**
 *  Per-class progression record. One per class on UOryxLevelComponent's TMap.
 *  Tracks XP earned, current level, and unallocated skill points (passive + ability).
 *
 *  Skill points granted per D14:
 *    - 1 passive point per level
 *    - 1 ability point per 2 levels
 *
 *  UOryxSkillTreeComponent tracks WHICH nodes are invested separately
 *  (UnallocatedPassive/Ability here is the *available-to-spend* count).
 */
USTRUCT(BlueprintType)
struct FOryxClassProgression
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentLevel = 1;

	/** XP within the current level (resets to 0 on level-up). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentXP = 0;

	/** Skill points granted from level-ups, not yet spent in the passive tree. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnallocatedPassive = 0;

	/** Skill points granted from level-ups, not yet spent in any ability tree. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnallocatedAbility = 0;
};
