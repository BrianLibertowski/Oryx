#include "OryxStatusEffectsComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Component/Health/OryxDamageTypes.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "GameFramework/Actor.h"

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
	}
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
		Active.AppliedModifierId = ApplyChillModifier(Spec);
	}

	ActiveEffects.Add(Active);
	OnEffectApplied.Broadcast(Spec.Type);
}

void UOryxStatusEffectsComponent::RemoveEffect(FGuid SourceId, EOryxStatusEffectType Type)
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		const FOryxActiveStatusEffect& E = ActiveEffects[i];
		if (E.Spec.Type == Type && E.Spec.SourceId == SourceId)
		{
			HandleEffectExpired(E);
			ActiveEffects.RemoveAt(i);
		}
	}
}

void UOryxStatusEffectsComponent::ClearAllEffects()
{
	for (const FOryxActiveStatusEffect& E : ActiveEffects)
	{
		HandleEffectExpired(E);
	}
	ActiveEffects.Reset();
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
			HandleEffectExpired(E);
			ActiveEffects.RemoveAt(i);
		}
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
		RemoveStatModifier(Effect.AppliedModifierId);
	}
	OnEffectExpired.Broadcast(Effect.Spec.Type);
}
