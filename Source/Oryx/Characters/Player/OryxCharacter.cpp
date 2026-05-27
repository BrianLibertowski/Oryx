#include "OryxCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

#include "Component/Health/OryxHealthComponent.h"
#include "Component/Ability/OryxAbilityComponent.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "Component/StatusEffects/OryxStatusEffectsComponent.h"
#include "States/Player/OryxPlayerState.h"
#include "GameModes/RunManager/OryxRunManager.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/OryxInteractable.h"
#include "DrawDebugHelpers.h"

AOryxCharacter::AOryxCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement (character rotates toward movement direction)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	// MaxWalkSpeed is set in BeginPlay from Stats (MovementSpeed).
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true; // boom follows controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // camera follows boom, not controller directly

	AbilityComponent = CreateDefaultSubobject<UOryxAbilityComponent>(TEXT("AbilityComponent"));
	HealthComponent = CreateDefaultSubobject<UOryxHealthComponent>(TEXT("HealthComponent"));
	StatsComponent = CreateDefaultSubobject<UOryxStatsComponent>(TEXT("StatsComponent"));
	StatusEffectsComponent = CreateDefaultSubobject<UOryxStatusEffectsComponent>(TEXT("StatusEffectsComponent"));
	// Currency lives on AOryxPlayerState (co-op proof: persists across pawn destruction)
}

UOryxCurrencyComponent* AOryxCharacter::GetCurrencyComponent() const
{
	if (AOryxPlayerState* PS = GetPlayerState<AOryxPlayerState>())
	{
		return PS->GetCurrencyComponent();
	}
	return nullptr;
}

float AOryxCharacter::GetMaxStamina() const
{
	return StatsComponent ? StatsComponent->GetStat(EOryxStat::MaxStamina) : MaxStamina;
}

float AOryxCharacter::GetStaminaFraction() const
{
	const float Max = GetMaxStamina();
	if (Max <= 0.f) return 1.f;
	return FMath::Clamp(CurrentStamina / Max, 0.f, 1.f);
}

float AOryxCharacter::GetMaxMana() const
{
	return StatsComponent ? StatsComponent->GetStat(EOryxStat::MaxMana) : MaxMana;
}

float AOryxCharacter::GetManaFraction() const
{
	const float Max = GetMaxMana();
	if (Max <= 0.f) return 1.f;
	return FMath::Clamp(CurrentMana / Max, 0.f, 1.f);
}

void AOryxCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const UWorld* World = GetWorld();
	if (!World) return;

	const float Now = World->GetTimeSeconds();

	// Sprint drains stamina; auto-cancel when empty.
	if (bIsSprinting && SprintStaminaCostPerSec > 0.f)
	{
		const float Drain = SprintStaminaCostPerSec * DeltaSeconds;
		CurrentStamina = FMath::Max(0.f, CurrentStamina - Drain);
		LastStaminaUseTime = Now;

		if (CurrentStamina <= 0.f)
		{
			bIsSprinting = false;
			RefreshMovementSpeed();
		}
	}

	// Stamina regen — gated by delay since last use. Stat StaminaRegen adds to base.
	const float MaxSt = GetMaxStamina();
	const float StaminaRate = StaminaRegenPerSec
		+ (StatsComponent ? StatsComponent->GetStat(EOryxStat::StaminaRegen) : 0.f);
	if (CurrentStamina < MaxSt && StaminaRate > 0.f && !bIsSprinting
	    && (Now - LastStaminaUseTime) >= StaminaRegenDelay)
	{
		CurrentStamina = FMath::Min(MaxSt, CurrentStamina + StaminaRate * DeltaSeconds);
	}

	// Mana regen — gated by delay since last spend. Stat ManaRegen adds to base.
	const float MaxMn = GetMaxMana();
	const float ManaRate = ManaRegenPerSec
		+ (StatsComponent ? StatsComponent->GetStat(EOryxStat::ManaRegen) : 0.f);
	if (CurrentMana < MaxMn && ManaRate > 0.f
	    && (Now - LastManaUseTime) >= ManaRegenDelay)
	{
		CurrentMana = FMath::Min(MaxMn, CurrentMana + ManaRate * DeltaSeconds);
	}

	// Health regen — purely stat-driven (no base UPROPERTY). HP/sec applied while alive and below max.
	if (StatsComponent && HealthComponent && !HealthComponent->IsDead())
	{
		const float HealthRate = StatsComponent->GetStat(EOryxStat::HealthRegen);
		if (HealthRate > 0.f
		    && HealthComponent->GetCurrentHealth() < HealthComponent->GetMaxHealth())
		{
			HealthComponent->ApplyHealing(HealthRate * DeltaSeconds);
		}
	}
}

bool AOryxCharacter::TryConsumeStamina(float Amount)
{
	if (Amount <= 0.f) return true;
	if (CurrentStamina < Amount) return false;
	CurrentStamina -= Amount;
	if (const UWorld* World = GetWorld())
	{
		LastStaminaUseTime = World->GetTimeSeconds();
	}
	return true;
}

void AOryxCharacter::SetCurrentStamina(float Amount)
{
	CurrentStamina = FMath::Clamp(Amount, 0.f, GetMaxStamina());
}

bool AOryxCharacter::TryConsumeMana(float Amount)
{
	if (Amount <= 0.f) return true;
	if (CurrentMana < Amount) return false;
	CurrentMana -= Amount;
	if (const UWorld* World = GetWorld())
	{
		LastManaUseTime = World->GetTimeSeconds();
	}
	return true;
}

void AOryxCharacter::SetCurrentMana(float Amount)
{
	CurrentMana = FMath::Clamp(Amount, 0.f, GetMaxMana());
}

// --- Reward request layer ---
// Wrappers stay single-line in single-player. When co-op lands, mark these as
// Server, Reliable RPCs and add HasAuthority gates — call sites don't change.

void AOryxCharacter::RequestApplyVitality()
{
	if (!StatsComponent) return;

	FOryxStatModifier Mod;
	Mod.Stat = EOryxStat::MaxHealth;
	Mod.Op = EOryxModOp::Additive;
	Mod.Value = VitalityBoost;
	StatsComponent->AddModifier(Mod);
	// HealthComponent listens to OnStatsChanged and re-clamps CurrentHealth.
	// Vitality reward heals to full as a thank-you bump:
	if (HealthComponent)
	{
		HealthComponent->ApplyHealing(VitalityBoost);
	}
}

void AOryxCharacter::RequestApplySwiftness()
{
	if (!StatsComponent) return;

	FOryxStatModifier Mod;
	Mod.Stat = EOryxStat::MovementSpeed;
	Mod.Op = EOryxModOp::Additive;
	Mod.Value = SwiftnessBoost;
	StatsComponent->AddModifier(Mod);
	// HandleStatsChanged refreshes MaxWalkSpeed automatically.
}

void AOryxCharacter::RequestApplyRestore()
{
	if (HealthComponent)
	{
		HealthComponent->ApplyHealing(HealthComponent->GetMaxHealth());
	}
}

void AOryxCharacter::BeginPlay()
{
	Super::BeginPlay();

	AController* Ctr = GetController();

	UE_LOG(LogTemp, Warning,
		TEXT("AOryxCharacter::BeginPlay on %s, Controller = %s"),
		*GetName(),
		Ctr ? *Ctr->GetName() : TEXT("NONE"));

	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxCharacter: I am the local player pawn!"));
	}

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AOryxCharacter::HandleDeath);
	}

	if (StatsComponent)
	{
		StatsComponent->OnStatsChanged.AddDynamic(this, &AOryxCharacter::HandleStatsChanged);
	}

	// Seed MaxWalkSpeed from Stats now that components are wired
	RefreshMovementSpeed();
}

void AOryxCharacter::HandleStatsChanged()
{
	RefreshMovementSpeed();
}

void AOryxCharacter::RefreshMovementSpeed()
{
	if (!StatsComponent) return;
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	const float Base = StatsComponent->GetStat(EOryxStat::MovementSpeed);
	MoveComp->MaxWalkSpeed = bIsSprinting ? Base * SprintMultiplier : Base;
}

void AOryxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &AOryxCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &AOryxCharacter::Move);

		// Mouse look (e.g. IA_MouseLook in IMC_MouseLook)
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered,
			this, &AOryxCharacter::Look);

		// Gamepad look (e.g. IA_Look in IMC_Default)
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &AOryxCharacter::Look);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,
			this, &AOryxCharacter::DoSprintEnd);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoDash);

		// Interact (totems, vendors, chests)
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoInteract);

		// Toggle Stats Screen (Tab) — BP implements OnToggleStatsScreen
		if (ToggleStatsAction)
		{
			EnhancedInputComponent->BindAction(ToggleStatsAction, ETriggerEvent::Started,
				this, &AOryxCharacter::OnToggleStatsScreen);
		}

		// Toggle in-game pause menu (Esc) — BP implements OnToggleMenu
		if (ToggleMenuAction)
		{
			EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started,
				this, &AOryxCharacter::OnToggleMenu);
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AOryxCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOryxCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AOryxCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AOryxCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(-Pitch);
	}
}

void AOryxCharacter::DoJumpStart()
{
	Jump();
}

void AOryxCharacter::DoJumpEnd()
{
	StopJumping();
}

void AOryxCharacter::DoSprintStart()
{
	// Gate: need at least some stamina to start sprinting (avoids 1-frame sprint flicker on empty bar).
	if (CurrentStamina <= 0.f) return;

	bIsSprinting = true;
	RefreshMovementSpeed();
}

void AOryxCharacter::DoSprintEnd()
{
	bIsSprinting = false;
	RefreshMovementSpeed();
}

void AOryxCharacter::DoDash()
{
	if (!GetCharacterMovement() || !GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDashTime < DashCooldown)
	{
		// Still on cooldown
		return;
	}

	// Stamina gate (configurable in BP defaults via DashStaminaCost)
	if (!TryConsumeStamina(DashStaminaCost))
	{
		return;
	}

	// Determine dash direction: prefer last movement input, otherwise forward
	FVector DashDirection = GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}
	DashDirection.Z = 0.f;
	DashDirection.Normalize();

	const FVector LaunchVelocity = DashDirection * DashStrength;

	// LaunchCharacter: XY dash, keep current vertical movement
	LaunchCharacter(LaunchVelocity, true, false);

	LastDashTime = CurrentTime;
}

void AOryxCharacter::DoInteract()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Trace forward from camera/eyes — feels more intuitive than from feet
	const FVector TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector TraceEnd = TraceStart + GetActorForwardVector() * InteractTraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(OryxInteract), false, this);

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

#if WITH_EDITOR
	DrawDebugLine(World, TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 1.5f, 0, 1.f);
#endif

	if (!bHit) return;

	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;

	// Direct C++ interface implementer
	if (IOryxInteractable* AsInteractable = Cast<IOryxInteractable>(HitActor))
	{
		IOryxInteractable::Execute_Interact(HitActor, this);
		return;
	}

	// Blueprint implementer
	if (HitActor->GetClass()->ImplementsInterface(UOryxInteractable::StaticClass()))
	{
		IOryxInteractable::Execute_Interact(HitActor, this);
	}
}

void AOryxCharacter::HandleDeath(AActor* DeadActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Oryx has died!"));

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->DisableInput(nullptr);
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}

	if (GameOverWidgetClass && PC)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
	}

	// Notify the RunManager so it can transition to Dead state + trigger profile save / RunSave cleanup.
	// Per D19 (solo death = run ends); co-op revive shrine deferred.
	// Gate on player-control: a future AI-controlled AOryxCharacter (e.g. companion subclass)
	// dying must NOT end the run.
	if (IsPlayerControlled())
	{
		if (UWorld* World = GetWorld())
		{
			if (AOryxRunManager* RM = World->GetGameState<AOryxRunManager>())
			{
				RM->HandlePlayerDeath();
			}
		}
	}
}