#include "OryxItemTypes.h"

int32 UOryxItemEconomy::GetBaseRarityCost(EOryxItemRarity Rarity)
{
	switch (Rarity)
	{
	case EOryxItemRarity::Common:    return 25;
	case EOryxItemRarity::Uncommon:  return 60;
	case EOryxItemRarity::Rare:      return 150;
	case EOryxItemRarity::Epic:      return 400;
	case EOryxItemRarity::Legendary: return 1000;
	default:                         return 0;
	}
}

float UOryxItemEconomy::GetStageMultiplier(int32 Stage)
{
	switch (Stage)
	{
	case 1:  return 1.0f;
	case 2:  return 3.0f;
	case 3:  return 7.0f;
	default: return 1.0f;
	}
}

int32 UOryxItemEconomy::GetItemCost(EOryxItemRarity Rarity, int32 Stage)
{
	const int32 Base = GetBaseRarityCost(Rarity);
	const float Mult = GetStageMultiplier(Stage);
	return FMath::RoundToInt(Base * Mult);
}

int32 UOryxItemEconomy::GetSellValue(EOryxItemRarity Rarity, int32 Stage)
{
	return GetItemCost(Rarity, Stage) / 2;
}

int32 UOryxItemEconomy::GetStage1Weight(EOryxItemRarity Rarity)
{
	switch (Rarity)
	{
	case EOryxItemRarity::Common:    return 50;
	case EOryxItemRarity::Uncommon:  return 30;
	case EOryxItemRarity::Rare:      return 12;
	case EOryxItemRarity::Epic:      return 6;
	case EOryxItemRarity::Legendary: return 2;
	default:                         return 0;
	}
}
