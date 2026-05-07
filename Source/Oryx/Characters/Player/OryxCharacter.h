#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OryxCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class AOryxProjectile;
class UOryxAbilityComponent;
class UOryxHealthComponent;
class UOryxStatsComponent;
class UOryxStatusEffectsComponent;
class UOryxCurrencyComponent;
class UUserWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AOryxCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Manages this character's abilities and their cooldowns */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxAbilityComponent* AbilityComponent;

	/** Holds this character's health state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxHealthComponent* HealthComponent;

	/** Tracks all stats (per-class base + runtime modifiers from items, buffs, level-ups) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxStatsComponent* StatsComponent;

	/** Tracks active status effects (Burn/Poison/Chill). Ticks DoTs, applies stat-mod debuffs. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxStatusEffectsComponent* StatusEffectsComponent;

protected:

	// Input Action

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action (gamepad) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action (mouse XY) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* FireAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;

	/** Dash / Dodge Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;

	/** Interact Input Action (totems, vendors, chests, doors) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/** Toggle Stats Screen Input Action (default Tab) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleStatsAction;

	/** Toggle in-game pause menu Input Action (default Esc). BP handles the widget hub + pause logic. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleMenuAction;

	/** Forward distance the interact trace reaches (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Interact")
	float InteractTraceDistance = 250.f;

	// Stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Stats")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Stats")
	float CurrentStamina = 100.f;

	// Mana
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Stats")
	float MaxMana = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Stats")
	float CurrentMana = 100.f;

	// Widget class to show on death — set in BP_OryxCharacter
	UPROPERTY(EditDefaultsOnly, Category = "Oryx|UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	// Bound to HealthComponent->OnDeath in BeginPlay
	UFUNCTION()
	void HandleDeath(AActor* DeadActor);

public:
	AOryxCharacter();

	virtual void BeginPlay() override;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input (Enhanced Input) */
	void Move(const FInputActionValue& Value);

	/** Called for looking input (Enhanced Input) */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump released inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	/** Handles sprint pressed inputs */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSprintStart();

	/** Handles sprint released inputs */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSprintEnd();

	/** Handles dash / dodge input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDash();

	/** Handles interact input — line-traces forward and calls IOryxInteractable::Interact on the hit */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoInteract();

	/** Fires when ToggleStatsAction is pressed. BP_OryxCharacter implements widget show/hide. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Oryx|UI")
	void OnToggleStatsScreen();

	/**
	 *  Fires when ToggleMenuAction is pressed. BP_OryxCharacter implements:
	 *    - Create/destroy the in-game pause menu hub (WBP_PauseMenu) — Resume / Settings / Save / Leave / Quit.
	 *    - In standalone (NetMode == Standalone) toggle Set Game Paused.
	 *    - In networked play, do not pause; widget overlays while game runs.
	 *
	 *  BlueprintCallable so external widgets (Resume button, Settings close, etc) can invoke
	 *  the same toggle path the Esc input does.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Oryx|UI")
	void OnToggleMenu();

	// --- Movement tuning ---
	/** Walk speed comes from Stats (MovementSpeed). Sprint = MovementSpeed * SprintMultiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float SprintMultiplier = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float DashStrength = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float DashCooldown = 1.0f;

	float LastDashTime = -1000.f;

	/** Tracks sprinting state so Stats-driven speed refreshes pick the correct multiplier */
	bool bIsSprinting = false;

	/** Re-applies MaxWalkSpeed from current Stats × sprint state. Called on stats changes and sprint toggles. */
	void RefreshMovementSpeed();

	UFUNCTION()
	void HandleStatsChanged();

	// --- Reward tuning (read by RequestApply* functions; configurable per-class in BP) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Rewards")
	float VitalityBoost = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Rewards")
	float SwiftnessBoost = 50.f;

public:

	// --- Reward request layer (UI -> gameplay; future-proofed for Server RPCs in co-op) ---

	/** Increase MaxHealth by VitalityBoost */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Rewards")
	void RequestApplyVitality();

	/** Increase WalkSpeed + SprintSpeed by SwiftnessBoost */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Rewards")
	void RequestApplySwiftness();

	/** Heal to full */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Rewards")
	void RequestApplyRestore();

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns AbilityComponent subobject **/
	FORCEINLINE UOryxAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }

	/** Returns HealthComponent subobject **/
	FORCEINLINE UOryxHealthComponent* GetHealthComponent() const { return HealthComponent; }

	/** Returns StatsComponent subobject **/
	FORCEINLINE UOryxStatsComponent* GetStatsComponent() const { return StatsComponent; }

	/** Returns StatusEffectsComponent subobject **/
	FORCEINLINE UOryxStatusEffectsComponent* GetStatusEffectsComponent() const { return StatusEffectsComponent; }

	/** Returns this character's currency component (lives on PlayerState — survives pawn death) */
	UFUNCTION(BlueprintPure, Category = "Oryx|Currency")
	UOryxCurrencyComponent* GetCurrencyComponent() const;

	// --- Resource getters (HUD-bound; consumers wire in later) ---

	/** Max stamina, sourced from Stats (EOryxStat::MaxStamina) with fallback to MaxStamina UPROPERTY. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetCurrentStamina() const { return CurrentStamina; }

	/** 0..1 fraction for HUD stamina bar. Returns 1 if MaxStamina <= 0. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetStaminaFraction() const;

	/** Max mana, sourced from Stats (EOryxStat::MaxMana) with fallback to MaxMana UPROPERTY. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetCurrentMana() const { return CurrentMana; }

	/** 0..1 fraction for HUD mana bar. Returns 1 if MaxMana <= 0. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Resources")
	float GetManaFraction() const;
};