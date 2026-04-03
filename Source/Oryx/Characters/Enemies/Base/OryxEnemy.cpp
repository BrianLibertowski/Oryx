#include "OryxEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/Health/OryxHealthComponent.h"

AOryxEnemy::AOryxEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<UOryxHealthComponent>(TEXT("HealthComponent"));

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = 300.f;
    }
}

void AOryxEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AOryxEnemy::HandleDeath);
    }
}

void AOryxEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AOryxEnemy::HandleDeath(AActor* DeadActor)
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died."), *GetName());

    // Disable collision and movement immediately
    SetActorEnableCollision(false);

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->DisableMovement();
    }

    // Later: play death anim, notify wave manager, drop loot
    Destroy();
}