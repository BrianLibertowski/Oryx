#include "OryxCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

#include "OryxProjectile.h"

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
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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

	// Start at full health
	CurrentHealth = MaxHealth;
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

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started,
			this, &AOryxCharacter::Fire);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed,
			this, &AOryxCharacter::DoSprintEnd);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started,
			this, &AOryxCharacter::DoDash);
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
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = SprintSpeed;
	}
}

void AOryxCharacter::DoSprintEnd()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
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

void AOryxCharacter::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire() called"));

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector MuzzleLocation =
		GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0.f, 0.f, 50.f);
	const FRotator MuzzleRotation = GetControlRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.Owner = this;

	World->SpawnActor<AOryxProjectile>(MuzzleLocation, MuzzleRotation, SpawnParams);
}

void AOryxCharacter::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
	{
		return;
	}

	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Max(CurrentHealth, 0.f);

	UE_LOG(LogTemp, Log,
		TEXT("Oryx took %.1f damage. CurrentHealth = %.1f"),
		DamageAmount, CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		HandleDeath();
	}
}

void AOryxCharacter::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Oryx has died!"));

	if (AController* MyController = GetController())
	{
		MyController->DisableInput(nullptr);
	}
}