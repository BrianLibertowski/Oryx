#include "OryxAbility.h"
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

UWorld* UOryxAbility::GetWorld() const
{
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}
