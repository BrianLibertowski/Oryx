#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/OryxClassTypes.h"
#include "OryxLevelComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnXPChanged, EOryxClass, ClassEnum, int32, NewXP, int32, NewLevel, int32, XPForNextLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUp, EOryxClass, ClassEnum, int32, NewLevel);

/**
 *  Per-class XP + Level tracking (D17). Lives on PlayerState — survives pawn destruction.
 *
 *  D14: 1 passive skill point per level + 1 ability skill point per 2 levels (granted via
 *  FOryxClassProgression.UnallocatedPassive/Ability).
 *  D13 REVOKED: Onyx no longer awarded on level-up.
 *
 *  Curve: BaseXPForLevel × XPCurveScale^(CurrentLevel - 1).
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

	/**
	 *  The currently-played class. XP grants via AddXPToActiveClass route here.
	 *  Set when player chooses class on character select screen.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	EOryxClass ActiveClass = EOryxClass::Warrior;

	/** Grant XP to the active class. Convenience for kill-XP / encounter-clear paths that don't track class explicitly. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void AddXPToActiveClass(int32 Amount);

	/** Grant XP to a specific class. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void AddXPToClass(EOryxClass Class, int32 Amount);

	/** Sets which class is active — call from character select. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void SetActiveClass(EOryxClass Class) { ActiveClass = Class; }

	/** Per-class progression accessors. */

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetCurrentLevel(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetCurrentXP(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetUnallocatedPassive(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetUnallocatedAbility(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetMaxLevel() const { return MaxLevel; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetXPForNextLevel(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	int32 GetXPCostForLevel(int32 Level) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	float GetXPProgressFraction(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level")
	bool IsAtMaxLevel(EOryxClass Class) const;

	/** Spend an unallocated passive point. Called by UOryxSkillTreeComponent.Invest. Returns true on success. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	bool TryConsumePassivePoint(EOryxClass Class);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	bool TryConsumeAbilityPoint(EOryxClass Class);

	/** Restore points (called by SkillTreeComponent.RefundAll for free respec per D14). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void RefundPassivePoints(EOryxClass Class, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Oryx|Level")
	void RefundAbilityPoints(EOryxClass Class, int32 Count);

	/** Direct-set progression — used by save-load to restore state. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Level|Save")
	void SetProgression(EOryxClass Class, const FOryxClassProgression& Progression);

	UFUNCTION(BlueprintPure, Category = "Oryx|Level|Save")
	FOryxClassProgression GetProgression(EOryxClass Class) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Level|Save")
	const TMap<EOryxClass, FOryxClassProgression>& GetAllProgression() const { return ClassProgression; }

protected:
	/** Base XP cost for level 1 → 2. Subsequent levels scale by XPCurveScale. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level|Curve")
	int32 BaseXPForLevel = 6000;

	/** Multiplier applied per level. 1.05 = +5% per level. Set 1.0 for fully linear. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level|Curve")
	float XPCurveScale = 1.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	int32 MaxLevel = 60;

	/**
	 *  Per-class progression records. Initialized lazily — GetProgressionRef ensures
	 *  the entry exists for the requested class.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Level")
	TMap<EOryxClass, FOryxClassProgression> ClassProgression;

private:
	/** Returns a writable reference, creating the entry if absent. */
	FOryxClassProgression& GetProgressionRef(EOryxClass Class);
};
