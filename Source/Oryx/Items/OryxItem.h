#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Component/Stats/OryxStatTypes.h"
#include "OryxItemTypes.h"
#include "OryxItem.generated.h"

class UTexture2D;

/**
 *  Authoring data for a single item. Pure stat stick per D7 — trigger effects deferred.
 *  Items are PrimaryDataAssets so the AssetManager can stream them and we can use
 *  PrimaryAssetType-based filtering for shops/chests.
 *
 *  Authoring flow: right-click in Content/Items/ → Miscellaneous → Data Asset → UOryxItem.
 */
UCLASS(BlueprintType)
class ORYX_API UOryxItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Stable id used to key this item across save/load and inventories. Defaults to asset name on creation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item")
	EOryxItemRarity Rarity = EOryxItemRarity::Common;

	/**
	 *  Modifiers this item applies when in inventory. Each entry is pushed into the
	 *  player's StatsComponent via AddModifier; SourceId is generated at apply-time
	 *  and stored by the InventoryComponent for clean removal on sell.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Item")
	TArray<FOryxStatModifier> StatModifiers;

	/** Convenience — cost at given stage using economy helpers. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Item")
	int32 GetCost(int32 Stage) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Item")
	int32 GetSellValue(int32 Stage) const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
