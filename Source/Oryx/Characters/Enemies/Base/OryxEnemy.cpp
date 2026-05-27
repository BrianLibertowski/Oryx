#include "OryxEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Component/StatusEffects/OryxStatusEffectsComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "Actors/Pickups/OryxPickup_Gold.h"
#include "Characters/Player/OryxCharacter.h"
#include "States/Player/OryxPlayerState.h"
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
    UWorld* World = GetWorld();
    if (!World) return;

    // Co-op proof: prefer the actual damage dealer (HealthComponent::GetLastInstigator) over a
    // guessed "nearest player" — multiple players hitting me must each be valid aggro candidates.
    // Local var renamed from "Instigator" → "DamageDealer" to avoid C4458 shadow of AActor::Instigator.
    AActor* AggroTarget = nullptr;
    if (HealthComponent)
    {
        AActor* DamageDealer = HealthComponent->GetLastInstigator();
        if (IsValid(DamageDealer))
        {
            AggroTarget = DamageDealer;
        }
    }

    // Fallback for unknown source (e.g. environmental damage, DoT after instigator died).
    if (!AggroTarget)
    {
        float NearestDistSq = TNumericLimits<float>::Max();
        for (TActorIterator<APawn> It(World); It; ++It)
        {
            APawn* P = *It;
            if (!IsValid(P) || !P->IsPlayerControlled()) continue;
            const float DistSq = FVector::DistSquared(P->GetActorLocation(), GetActorLocation());
            if (DistSq < NearestDistSq)
            {
                NearestDistSq = DistSq;
                AggroTarget = P;
            }
        }
    }

    if (!AggroTarget) return;

    MarkAlerted(AggroTarget);

    // Spread aggro to nearby allies
    if (AlertRadius <= 0.f) return;
    const float AlertRadiusSq = AlertRadius * AlertRadius;
    for (TActorIterator<AOryxEnemy> It(World); It; ++It)
    {
        AOryxEnemy* Ally = *It;
        if (!IsValid(Ally) || Ally == this) continue;
        if (FVector::DistSquared(Ally->GetActorLocation(), GetActorLocation()) <= AlertRadiusSq)
        {
            Ally->MarkAlerted(AggroTarget);
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

    // XP grant per D17 — constant per-enemy, routes to the active class on the actual killer's LevelComponent.
    // Co-op proof: HealthComponent::GetLastInstigator() captures the damage dealer who landed the killing blow.
    // Fallback: if instigator is unknown (e.g. fell off the world / DoT with no source), grants to first
    // AOryxCharacter found so single-player still rewards player.
    if (XPReward > 0)
    {
        AOryxCharacter* Killer = nullptr;

        if (HealthComponent)
        {
            Killer = Cast<AOryxCharacter>(HealthComponent->GetLastInstigator());
        }

        if (!Killer)
        {
            // Fallback path — keeps single-player working when damage source is unknown.
            if (UWorld* World = GetWorld())
            {
                for (TActorIterator<AOryxCharacter> It(World); It; ++It)
                {
                    AOryxCharacter* Player = *It;
                    if (!IsValid(Player)) continue;
                    Killer = Player;
                    break;
                }
            }
        }

        if (Killer)
        {
            if (AOryxPlayerState* PS = Killer->GetPlayerState<AOryxPlayerState>())
            {
                if (UOryxLevelComponent* LC = PS->GetLevelComponent())
                {
                    LC->AddXPToActiveClass(XPReward);
                }
            }
        }
    }

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