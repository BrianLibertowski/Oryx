#include "OryxLevelComponent.h"

UOryxLevelComponent::UOryxLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxLevelComponent::AddXP(int32 Amount)
{
	if (Amount <= 0 || IsAtMaxLevel()) return;

	CurrentXP += Amount;

	// Cascade through any level boundaries (handle large XP grants e.g. boss kill).
	while (!IsAtMaxLevel())
	{
		const int32 Threshold = GetXPCostForLevel(CurrentLevel);
		if (Threshold <= 0 || CurrentXP < Threshold) break;

		CurrentXP -= Threshold;
		CurrentLevel++;

		OnLevelUp.Broadcast(CurrentLevel, OnyxPerLevel);

		// At cap: clamp residual XP to 0 so the HUD shows full bar / no overflow.
		if (IsAtMaxLevel())
		{
			CurrentXP = 0;
			break;
		}
	}

	OnXPChanged.Broadcast(CurrentXP, GetXPForNextLevel());
}

int32 UOryxLevelComponent::GetXPForNextLevel() const
{
	if (IsAtMaxLevel()) return 0;
	return GetXPCostForLevel(CurrentLevel);
}

int32 UOryxLevelComponent::GetXPCostForLevel(int32 Level) const
{
	if (Level < 1) return 0;
	const float Cost = BaseXPForLevel * FMath::Pow(XPCurveScale, FMath::Max(0, Level - 1));
	return FMath::RoundToInt(Cost);
}

float UOryxLevelComponent::GetXPProgressFraction() const
{
	if (IsAtMaxLevel()) return 1.f;
	const int32 Threshold = GetXPForNextLevel();
	if (Threshold <= 0) return 1.f;
	return FMath::Clamp(static_cast<float>(CurrentXP) / static_cast<float>(Threshold), 0.f, 1.f);
}
