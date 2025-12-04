#include "OryxCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Oryx.h"
#include "OryxProjectile.h"

AOryxCharacter::AOryxCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

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
		// Jump
		EnhancedInputComponent->BindAction(JumpAction,
			ETriggerEvent::Started, this, &AOryxCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,
			ETriggerEvent::Completed, this, &AOryxCharacter::StopJumping);

		// Movement
		EnhancedInputComponent->BindAction(MoveAction,
			ETriggerEvent::Triggered, this, &AOryxCharacter::Move);

		// Look
		EnhancedInputComponent->BindAction(LookAction,
			ETriggerEvent::Triggered, this, &AOryxCharacter::Look);

		// Fire
		EnhancedInputComponent->BindAction(FireAction,
			ETriggerEvent::Started, this, &AOryxCharacter::Fire);
	}
}

void AOryxCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOryxCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AOryxCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AOryxCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
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
		return;

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
