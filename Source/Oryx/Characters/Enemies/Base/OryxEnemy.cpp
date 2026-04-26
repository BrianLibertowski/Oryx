#include "OryxEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Actors/Pickups/OryxPickup_Gold.h"

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

    if (GoldPickupClass && GoldDropCount > 0)
    {
        const FVector SpawnBase = GetActorLocation();
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        for (int32 i = 0; i < GoldDropCount; ++i)
        {
            const FVector Offset = FVector(FMath::FRandRange(-40.f, 40.f), FMath::FRandRange(-40.f, 40.f), 0.f);
            if (AOryxPickup_Gold* Pickup = GetWorld()->SpawnActor<AOryxPickup_Gold>(GoldPickupClass, SpawnBase + Offset, FRotator::ZeroRotator, Params))
            {
                Pickup->Value = GoldDropValue;
            }
        }
    }

    Destroy();
}