#pragma once

#include "CoreMinimal.h"
#include "OryxItemTypes.generated.h"

/** Item rarity tiers. Drives shop weighting, drop tables, cost. */
UENUM(BlueprintType)
enum class EOryxItemRarity : uint8
{
	Common    UMETA(DisplayName = "Common"),
	Uncommon  UMETA(DisplayName = "Uncommon"),
	Rare      UMETA(DisplayName = "Rare"),
	Epic      UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

/**
 *  Static helpers for item economy. Cost formula and rarity weights live here so
 *  shops, chests, and the inventory share the same source of truth.
 *
 *  Per plan:
 *    ItemCost = BaseRarityCost × StageMultiplier
 *    BaseRarityCost: Common 25 / Uncommon 60 / Rare 150 / Epic 400 / Legendary 1000
 *    StageMultiplier: Stage 1 = 1.0, Stage 2 = 3.0, Stage 3 = 7.0
 *    Stage 1 vault rates: 50/30/12/6/2 (C/U/R/E/L)
 */
UCLASS()
class ORYX_API UOryxItemEconomy : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Items")
	static int32 GetBaseRarityCost(EOryxItemRarity Rarity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Items")
	static float GetStageMultiplier(int32 Stage);

	/** Final purchase cost = Base × Stage. Stage is 1-indexed (1, 2, 3). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Items")
	static int32 GetItemCost(EOryxItemRarity Rarity, int32 Stage);

	/** Sell-back value at 50% of purchase cost (per plan). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Items")
	static int32 GetSellValue(EOryxItemRarity Rarity, int32 Stage);

	/** Stage 1 rarity weights for shops + chests (50/30/12/6/2). */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Items")
	static int32 GetStage1Weight(EOryxItemRarity Rarity);
};
