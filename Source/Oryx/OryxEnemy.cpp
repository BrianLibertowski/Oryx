#include "OryxEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AOryxEnemy::AOryxEnemy()
{
    // Tick if you need it (can turn off later if unused)
    PrimaryActorTick.bCanEverTick = true;

    // Example movement tuning – make enemies a bit slower than player
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = 300.f;
    }

    // Start at full health
    CurrentHealth = MaxHealth;
}

void AOryxEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Ensure health is initialized correctly
    CurrentHealth = MaxHealth;
}

void AOryxEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // AI logic will go here later
}

void AOryxEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Enemies are AI-controlled for now, so no input bindings
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOryxEnemy::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
    {
        return;
    }

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Max(CurrentHealth, 0.f);

    UE_LOG(LogTemp, Log,
        TEXT("Enemy %s took %.1f damage. Health = %.1f"),
        *GetName(), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        HandleDeath();
    }
}

void AOryxEnemy::HandleDeath()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died."), *GetName());

    // Later: play death animation, drop loot, etc.
    Destroy();
}