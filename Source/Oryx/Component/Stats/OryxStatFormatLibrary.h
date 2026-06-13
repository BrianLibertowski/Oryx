#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OryxStatTypes.h"
#include "OryxStatFormatLibrary.generated.h"

/**
 *  Central stat → display-text formatting. ONE source of truth for how stats read in the UI,
 *  used by every tooltip surface: item cards, run-buff cards, skill tree nodes, the stats screen.
 *
 *  Why centralize: stats split into "percentage" (stored as 0.10, shown as "10%") vs "flat"
 *  (stored as 25, shown as "25"). Without one library, every widget re-implements this split and
 *  they drift. Add a stat once here, every tooltip formats it correctly.
 *
 *  All functions are static BlueprintPure — call from any widget without an instance.
 */
UCLASS()
class ORYX_API UOryxStatFormatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** True if this stat is displayed as a percentage (stored 0.10 → "10%"). False = flat integer. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats|Format")
	static bool IsPercentageStat(EOryxStat Stat);

	/** Player-facing name, e.g. "Attack Speed", "Critical Chance". */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats|Format")
	static FText GetStatDisplayName(EOryxStat Stat);

	/**
	 *  Formats a raw stat value for display: percentage stats → "26%", flat stats → "150".
	 *  Used by the stats screen to render each row's value.
	 */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats|Format")
	static FText FormatStatValue(EOryxStat Stat, float Value);

	/**
	 *  Formats a single modifier as a signed delta line: "+10% Attack Speed", "+25 Max Health",
	 *  "-5% Damage". Multiplicative ops get a "(more)" suffix to distinguish stacking type.
	 *  This is THE node/item/card tooltip line.
	 */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats|Format")
	static FText FormatModifier(const FOryxStatModifier& Modifier);

	/**
	 *  Formats one skill-node rank line showing current → next: "+10% → +20% Attack Speed".
	 *  CurrentRank = ranks already invested, modifiers are stepped (per-rank value × rank).
	 *  Pass the node's single per-rank modifier + the per-rank magnitude.
	 */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats|Format")
	static FText FormatRankPreview(EOryxStat Stat, float PerRankValue, int32 CurrentRank, int32 MaxRanks);
};
