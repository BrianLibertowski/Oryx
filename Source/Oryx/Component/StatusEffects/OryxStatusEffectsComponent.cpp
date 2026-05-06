#include "OryxStatusEffectsComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Component/Health/OryxDamageTypes.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UOryxStatusEffectsComponent::UOryxStatusEffectsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
}

void UOryxStatusEffectsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		HealthComp = OwnerActor->FindComponentByClass<UOryxHealthComponent>();
		StatsComp  = OwnerActor->FindComponentByClass<UOryxStatsComponent>();

		// Defensive cleanup on death — prevents zombie stat-mods (e.g. Chill) lingering past death.
		if (HealthComp)
		{
			HealthComp->OnDeath.AddDynamic(this, &UOryxStatusEffectsComponent::HandleOwnerDeath);
		}

		// Fallback path for owners without Stats (enemies). Cache the base speed
		// so we can restore it after a Chill expires.
		if (!StatsComp)
		{
			if (ACharacter* OwnerChar = Cast<ACharacter>(OwnerActor))
			{
				MovementComp = OwnerChar->GetCharacterMovement();
				if (MovementComp)
				{
					BaseMaxWalkSpeed = MovementComp->MaxWalkSpeed;
				}
			}
		}
	}

	// Start with tick disabled — only run when there's something to tick.
	SetComponentTickEnabled(false);
}

void UOryxStatusEffectsComponent::HandleOwnerDeath(AActor* /*DeadActor*/)
{
	ClearAllEffects();
}

void UOryxStatusEffectsComponent::ApplyEffect(FOryxStatusEffectSpec Spec)
{
	if (Spec.Duration <= 0.f) return;
	if (!Spec.SourceId.IsValid())
	{
		Spec.SourceId = FGuid::NewGuid();
	}

	// Refresh path: same (SourceId, Type) already active -> just reset duration.
	for (FOryxActiveStatusEffect& Existing : ActiveEffects)
	{
		if (Existing.Spec.Type == Spec.Type && Existing.Spec.SourceId == Spec.SourceId)
		{
			Existing.TimeRemaining = Spec.Duration;
			return;
		}
	}

	// New effect.
	FOryxActiveStatusEffect Active;
	Active.Spec = Spec;
	Active.TimeRemaining = Spec.Duration;
	Active.TimeSinceTick = 0.f;

	if (Spec.Type == EOryxStatusEffectType::Chill)
	{
		if (StatsComp)
		{
			Active.AppliedModifierId = ApplyChillModifier(Spec);
		}
	}

	ActiveEffects.Add(Active);

	// Direct movement path: recompute speed after the entry is in the list.
	if (Spec.Type == EOryxStatusEffectType::Chill && !StatsComp)
	{
		RecalcDirectMovementSpeed();
	}

	// Wake the tick now that we have something to process.
	SetComponentTickEnabled(true);

	OnEffectApplied.Broadcast(Spec.Type);
}

void UOryxStatusEffectsComponent::RemoveEffect(FGuid SourceId, EOryxStatusEffectType Type)
{
	bool bRemovedChill = false;
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		const FOryxActiveStatusEffect& E = ActiveEffects[i];
		if (E.Spec.Type == Type && E.Spec.SourceId == SourceId)
		{
			if (E.Spec.Type == EOryxStatusEffectType::Chill && !StatsComp) bRemovedChill = true;
			HandleEffectExpired(E);
			ActiveEffects.RemoveAt(i);
		}
	}
	if (bRemovedChill) RecalcDirectMovementSpeed();
}

void UOryxStatusEffectsComponent::ClearAllEffects()
{
	for (const FOryxActiveStatusEffect& E : ActiveEffects)
	{
		HandleEffectExpired(E);
	}
	ActiveEffects.Reset();
	if (!StatsComp) RecalcDirectMovementSpeed();
}

bool UOryxStatusEffectsComponent::HasEffect(EOryxStatusEffectType Type) const
{
	for (const FOryxActiveStatusEffect& E : ActiveEffects)
	{
		if (E.Spec.Type == Type) return true;
	}
	return false;
}

void UOryxStatusEffectsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveEffects.Num() == 0) return;

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FOryxActiveStatusEffect& E = ActiveEffects[i];
		E.TimeRemaining -= DeltaTime;

		// DoT tick (Burn/Poison only — Chill is purely a stat-mod).
		const bool bIsDoT = (E.Spec.Type == EOryxStatusEffectType::Burn || E.Spec.Type == EOryxStatusEffectType::Poison);
		if (bIsDoT && E.Spec.TickInterval > 0.f)
		{
			E.TimeSinceTick += DeltaTime;
			while (E.TimeSinceTick >= E.Spec.TickInterval && E.TimeRemaining > -E.Spec.TickInterval)
			{
				TickDoT(E);
				E.TimeSinceTick -= E.Spec.TickInterval;
			}
		}

		if (E.TimeRemaining <= 0.f)
		{
			const bool bWasChill = (E.Spec.Type == EOryxStatusEffectType::Chill);
			HandleEffectExpired(E);
			ActiveEffects.RemoveAt(i);
			if (bWasChill && !StatsComp) RecalcDirectMovementSpeed();
		}
	}

	// Park the tick once the list drains.
	if (ActiveEffects.Num() == 0)
	{
		SetComponentTickEnabled(false);
	}
}

void UOryxStatusEffectsComponent::TickDoT(const FOryxActiveStatusEffect& Effect)
{
	if (!HealthComp) return;

	FOryxDamageEvent Event;
	Event.BaseAmount = Effect.Spec.Magnitude;
	Event.DamageType = (Effect.Spec.Type == EOryxStatusEffectType::Burn)
		? EOryxDamageType::Fire
		: EOryxDamageType::Poison;
	Event.Instigator = Effect.Spec.Instigator;
	Event.Causer = Effect.Spec.Instigator;
	Event.SourceId = Effect.Spec.SourceId;

	HealthComp->ApplyDamageEvent(Event);
}

FGuid UOryxStatusEffectsComponent::ApplyChillModifier(const FOryxStatusEffectSpec& Spec) const
{
	if (!StatsComp) return FGuid();

	// Magnitude is a slow fraction (e.g. 0.4 = -40% movement). Multiplicative ops in Stats
	// fold as (1 + Sum), so we push -Magnitude.
	FOryxStatModifier Mod;
	Mod.Stat  = EOryxStat::MovementSpeed;
	Mod.Op    = EOryxModOp::Multiplicative;
	Mod.Value = -FMath::Abs(Spec.Magnitude);
	return StatsComp->AddModifier(Mod);
}

void UOryxStatusEffectsComponent::RemoveStatModifier(const FGuid& ModifierId) const
{
	if (StatsComp && ModifierId.IsValid())
	{
		StatsComp->RemoveModifiersBySource(ModifierId);
	}
}

void UOryxStatusEffectsComponent::HandleEffectExpired(const FOryxActiveStatusEffect& Effect)
{
	if (Effect.Spec.Type == EOryxStatusEffectType::Chill)
	{
		if (StatsComp)
		{
			RemoveStatModifier(Effect.AppliedModifierId);
		}
		// Direct path is recomputed by the caller after this entry is dropped from ActiveEffects.
	}
	OnEffectExpired.Broadcast(Effect.Spec.Type);
}

void UOryxStatusEffectsComponent::RecalcDirectMovementSpeed()
{
	if (!MovementComp) return;

	float Multiplier = 1.f;
	for (const FOryxActiveStatusEffect& E : ActiveEffects)
	{
		if (E.Spec.Type == EOryxStatusEffectType::Chill)
		{
			Multiplier *= FMath::Max(0.f, 1.f - FMath::Abs(E.Spec.Magnitude));
		}
	}
	MovementComp->MaxWalkSpeed = BaseMaxWalkSpeed * Multiplier;
}
