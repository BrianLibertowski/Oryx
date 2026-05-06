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

	if (bIsBasicAttack)
	{
		const float AS = Stats->GetStat(EOryxStat::AttackSpeed);
		// AttackSpeed defaults to 1.0 in BaseStats — multiply through.
		Divisor *= FMath::Max(KINDA_SMALL_NUMBER, AS);
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
