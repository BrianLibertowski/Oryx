#include "OryxLevelComponent.h"

UOryxLevelComponent::UOryxLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxLevelComponent::AddXPToActiveClass(int32 Amount)
{
	AddXPToClass(ActiveClass, Amount);
}

void UOryxLevelComponent::AddXPToClass(EOryxClass Class, int32 Amount)
{
	if (Amount <= 0) return;
	if (IsAtMaxLevel(Class)) return;

	FOryxClassProgression& Prog = GetProgressionRef(Class);
	Prog.CurrentXP += Amount;

	// Cascade through level boundaries (big grants e.g. boss kill can skip multiple levels).
	while (Prog.CurrentLevel < MaxLevel)
	{
		const int32 Threshold = GetXPCostForLevel(Prog.CurrentLevel);
		if (Threshold <= 0 || Prog.CurrentXP < Threshold) break;

		Prog.CurrentXP -= Threshold;
		Prog.CurrentLevel++;

		// Skill points per D14: 1 passive per level, 1 ability per 2 levels (granted on even-numbered levels).
		Prog.UnallocatedPassive += 1;
		if (Prog.CurrentLevel % 2 == 0)
		{
			Prog.UnallocatedAbility += 1;
		}

		OnLevelUp.Broadcast(Class, Prog.CurrentLevel);

		// At cap: clamp residual XP to 0 so HUD shows full bar / no overflow.
		if (Prog.CurrentLevel >= MaxLevel)
		{
			Prog.CurrentXP = 0;
			break;
		}
	}

	OnXPChanged.Broadcast(Class, Prog.CurrentXP, Prog.CurrentLevel, GetXPForNextLevel(Class));
}

int32 UOryxLevelComponent::GetCurrentLevel(EOryxClass Class) const
{
	if (const FOryxClassProgression* Found = ClassProgression.Find(Class))
	{
		return Found->CurrentLevel;
	}
	return 1; // Default for never-played class.
}

int32 UOryxLevelComponent::GetCurrentXP(EOryxClass Class) const
{
	if (const FOryxClassProgression* Found = ClassProgression.Find(Class))
	{
		return Found->CurrentXP;
	}
	return 0;
}

int32 UOryxLevelComponent::GetUnallocatedPassive(EOryxClass Class) const
{
	if (const FOryxClassProgression* Found = ClassProgression.Find(Class))
	{
		return Found->UnallocatedPassive;
	}
	return 0;
}

int32 UOryxLevelComponent::GetUnallocatedAbility(EOryxClass Class) const
{
	if (const FOryxClassProgression* Found = ClassProgression.Find(Class))
	{
		return Found->UnallocatedAbility;
	}
	return 0;
}

int32 UOryxLevelComponent::GetXPForNextLevel(EOryxClass Class) const
{
	if (IsAtMaxLevel(Class)) return 0;
	return GetXPCostForLevel(GetCurrentLevel(Class));
}

int32 UOryxLevelComponent::GetXPCostForLevel(int32 Level) const
{
	if (Level < 1) return 0;
	const float Cost = BaseXPForLevel * FMath::Pow(XPCurveScale, FMath::Max(0, Level - 1));
	return FMath::RoundToInt(Cost);
}

float UOryxLevelComponent::GetXPProgressFraction(EOryxClass Class) const
{
	if (IsAtMaxLevel(Class)) return 1.f;
	const int32 Threshold = GetXPForNextLevel(Class);
	if (Threshold <= 0) return 1.f;
	const int32 CurXP = GetCurrentXP(Class);
	return FMath::Clamp(static_cast<float>(CurXP) / static_cast<float>(Threshold), 0.f, 1.f);
}

bool UOryxLevelComponent::IsAtMaxLevel(EOryxClass Class) const
{
	return GetCurrentLevel(Class) >= MaxLevel;
}

bool UOryxLevelComponent::TryConsumePassivePoint(EOryxClass Class)
{
	FOryxClassProgression& Prog = GetProgressionRef(Class);
	if (Prog.UnallocatedPassive <= 0) return false;
	Prog.UnallocatedPassive--;
	return true;
}

bool UOryxLevelComponent::TryConsumeAbilityPoint(EOryxClass Class)
{
	FOryxClassProgression& Prog = GetProgressionRef(Class);
	if (Prog.UnallocatedAbility <= 0) return false;
	Prog.UnallocatedAbility--;
	return true;
}

void UOryxLevelComponent::RefundPassivePoints(EOryxClass Class, int32 Count)
{
	if (Count <= 0) return;
	FOryxClassProgression& Prog = GetProgressionRef(Class);
	Prog.UnallocatedPassive += Count;
}

void UOryxLevelComponent::RefundAbilityPoints(EOryxClass Class, int32 Count)
{
	if (Count <= 0) return;
	FOryxClassProgression& Prog = GetProgressionRef(Class);
	Prog.UnallocatedAbility += Count;
}

void UOryxLevelComponent::SetProgression(EOryxClass Class, const FOryxClassProgression& Progression)
{
	ClassProgression.Add(Class, Progression);
}

FOryxClassProgression UOryxLevelComponent::GetProgression(EOryxClass Class) const
{
	if (const FOryxClassProgression* Found = ClassProgression.Find(Class))
	{
		return *Found;
	}
	return FOryxClassProgression();
}

FOryxClassProgression& UOryxLevelComponent::GetProgressionRef(EOryxClass Class)
{
	return ClassProgression.FindOrAdd(Class);
}
