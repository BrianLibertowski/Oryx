#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxLevelComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChanged, int32, NewXP, int32, XPForNextLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUp, int32, NewLevel, int32, OnyxAwarded);

/**
 *  Tracks player XP + level. Lives on PlayerState — survives pawn destruction.
 *  Phase 4 (per plan) will extend this to per-class tracking; this MVP placeholder
 *  is a single XP pool / single level so HUD bindings can be wired now.
 *
 *  Curve: BaseXPForLevel × XPCurveScale^(CurrentLevel - 1) — gentle exponential.
 *  Defaults: 6000 base, 1.05 scale, cap 60. Tune in BP defaults.
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxLevelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxLevelComponent();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Level")
	FOnXPChanged OnXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Level")
	FOnLevelUp OnLevelUp;

	/** Adds XP, levels up if threshold crossed. Awards Onyx per level-up via OnLevelUp delegate. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void AddXP(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetCurrentXP() const { return CurrentXP; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetMaxLevel() const { return MaxLevel; }

	/** XP cost to advance from CurrentLevel → CurrentLevel+1. 0 if at MaxLevel. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetXPForNextLevel() const;

	/** Returns the XP cost for the transition Level → Level+1. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetXPCostForLevel(int32 Level) const;

	/** 0..1 progress through the current level's XP bar. Returns 1.f if at MaxLevel. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	float GetXPProgressFraction() const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	bool IsAtMaxLevel() const { return CurrentLevel >= MaxLevel; }

protected:
	/** Base XP cost for level 1 → 2. Subsequent levels scale by XPCurveScale. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level|Curve")
	int32 BaseXPForLevel = 6000;

	/** Multiplier applied per level. 1.05 = +5% per level. Set 1.0 for fully linear. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level|Curve")
	float XPCurveScale = 1.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	int32 MaxLevel = 60;

	/** Per plan D13: 1 Onyx per level. Override per-class later if needed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	int32 OnyxPerLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	int32 CurrentLevel = 1;

	/** XP within the current level (resets to 0 on level-up). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	int32 CurrentXP = 0;
};
