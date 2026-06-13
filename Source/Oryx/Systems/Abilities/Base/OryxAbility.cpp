#include "OryxAbility.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "Engine/World.h"

UOryxAbility::UOryxAbility()
{
}

void UOryxAbility::Activate_Implementation(AActor* Owner)
{
}

bool UOryxAbility::IsOnCooldown(float CurrentTime) const
{
	return (CurrentTime - LastActivatedTime) < Cooldown;
}

float UOryxAbility::GetEffectiveCooldown(const UOryxStatsComponent* Stats) const
{
	if (Cooldown <= 0.f) return 0.f;
	if (!Stats) return Cooldown;

	const float CDR = Stats->GetStat(EOryxStat::CooldownReduction);
	float Divisor = (1.f + CDR);

	// Basic attacks scale with AttackSpeed; spells scale with CastSpeed. Both default to 1.0 in
	// BaseStats, so multiplying through is safe. Basic-attack wins if somehow both flags are set.
	if (bIsBasicAttack)
	{
		const float AS = Stats->GetStat(EOryxStat::AttackSpeed);
		Divisor *= FMath::Max(KINDA_SMALL_NUMBER, AS);
	}
	else if (bIsSpell)
	{
		const float CastSpd = Stats->GetStat(EOryxStat::CastSpeed);
		Divisor *= FMath::Max(KINDA_SMALL_NUMBER, CastSpd);
	}

	return Cooldown / FMath::Max(KINDA_SMALL_NUMBER, Divisor);
}

UWorld* UOryxAbility::GetWorld() const
{
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}
