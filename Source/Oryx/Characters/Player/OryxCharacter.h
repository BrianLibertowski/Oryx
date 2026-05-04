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

	// --- Movement tuning ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float WalkSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float SprintSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float DashStrength = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Movement")
	float DashCooldown = 1.0f;

	float LastDashTime = -1000.f;

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

	/** Returns this character's currency component (lives on PlayerState — survives pawn death) */
	UFUNCTION(BlueprintPure, Category = "Oryx|Currency")
	UOryxCurrencyComponent* GetCurrencyComponent() const;
};