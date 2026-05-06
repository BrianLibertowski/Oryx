#include "OryxItem.h"

int32 UOryxItem::GetCost(int32 Stage) const
{
	return UOryxItemEconomy::GetItemCost(Rarity, Stage);
}

int32 UOryxItem::GetSellValue(int32 Stage) const
{
	return UOryxItemEconomy::GetSellValue(Rarity, Stage);
}

FPrimaryAssetId UOryxItem::GetPrimaryAssetId() const
{
	const FName AssetType = TEXT("OryxItem");
	const FName AssetName = ItemId.IsNone() ? GetFName() : ItemId;
	return FPrimaryAssetId(AssetType, AssetName);
}
