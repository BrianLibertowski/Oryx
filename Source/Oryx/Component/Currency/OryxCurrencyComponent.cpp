#include "OryxCurrencyComponent.h"

UOryxCurrencyComponent::UOryxCurrencyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxCurrencyComponent::AddGold(int32 Amount)
{
	if (Amount <= 0) return;
	Gold += Amount;
	OnCurrencyChanged.Broadcast(Gold, Onyx);
}

void UOryxCurrencyComponent::AddOnyx(int32 Amount)
{
	if (Amount <= 0) return;
	Onyx += Amount;
	OnCurrencyChanged.Broadcast(Gold, Onyx);
}
