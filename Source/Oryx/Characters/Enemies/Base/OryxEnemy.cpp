#include "OryxEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Component/StatusEffects/OryxStatusEffectsComponent.h"
#include "Actors/Pickups/OryxPickup_Gold.h"
#include "EngineUtils.h"

AOryxEnemy::AOryxEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<UOryxHealthComponent>(TEXT("HealthComponent"));
    StatusEffectsComponent = CreateDefaultSubobject<UOryxStatusEffectsComponent>(TEXT("StatusEffectsComponent"));

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = 300.f;
    }
}

void AOryxEnemy::BeginPlay()
{
    Super::BeginPlay();

    SpawnLocation = GetActorLocation();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AOryxEnemy::HandleDeath);
        HealthComponent->OnDamaged.AddDynamic(this, &AOryxEnemy::HandleDamaged);
    }
}

bool AOryxEnemy::IsBeyondLeash() const
{
    if (LeashRange <= 0.f) return false;
    return FVector::DistSquared(GetActorLocation(), SpawnLocation) > (LeashRange * LeashRange);
}

void AOryxEnemy::MarkAlerted(AActor* InTarget)
{
    if (!IsValid(InTarget)) return;
    bForcedAggro = true;
    ForcedTarget = InTarget;
}

void AOryxEnemy::ClearAggro()
{
    bForcedAggro = false;
    ForcedTarget = nullptr;
}

void AOryxEnemy::HandleDamaged(float /*NewHealth*/, float /*DamageAmount*/)
{
    // We don't know who hit us from the delegate signature — alert toward nearest player.
    UWorld* World = GetWorld();
    if (!World) return;

    // Self alerts immediately
    AActor* NearestPlayer = nullptr;
    float NearestDistSq = TNumericLimits<float>::Max();
    for (TActorIterator<APawn> It(World); It; ++It)
    {
        APawn* P = *It;
        if (!IsValid(P) || !P->IsPlayerControlled()) continue;
        const float DistSq = FVector::DistSquared(P->GetActorLocation(), GetActorLocation());
        if (DistSq < NearestDistSq)
        {
            NearestDistSq = DistSq;
            NearestPlayer = P;
        }
    }
    if (!NearestPlayer) return;

    MarkAlerted(NearestPlayer);

    // Spread aggro to nearby allies
    if (AlertRadius <= 0.f) return;
    const float AlertRadiusSq = AlertRadius * AlertRadius;
    for (TActorIterator<AOryxEnemy> It(World); It; ++It)
    {
        AOryxEnemy* Ally = *It;
        if (!IsValid(Ally) || Ally == this) continue;
        if (FVector::DistSquared(Ally->GetActorLocation(), GetActorLocation()) <= AlertRadiusSq)
        {
            Ally->MarkAlerted(NearestPlayer);
        }
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