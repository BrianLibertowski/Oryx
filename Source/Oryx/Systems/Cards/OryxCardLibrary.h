#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OryxCardTypes.h"
#include "OryxCardLibrary.generated.h"

class UOryxStatsComponent;

/**
 *  Static helpers for the card system (D15).
 *  - GetCardsForCategory: returns the curated pool for a category.
 *  - RollCards: draws one random card from each of the three pools.
 *  - ApplyCardToStats: pushes the card's modifier onto a StatsComponent, returns the GUID
 *    so callers (run state, save) can remove it on run-end.
 *
 *  Card pools are defined in OryxCardLibrary.cpp. Edit there to tune magnitude/balance.
 *  Run-only persistence — RunSave clears all card-sourced modifiers on death/extract.
 */
UCLASS()
class ORYX_API UOryxCardLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the pool of cards available in a given category. Const, safe to call freely. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Cards")
	static TArray<FOryxCard> GetCardsForCategory(EOryxCardCategory Category);

	/**
	 *  Draws one card from each category — returns exactly 3 cards in the order
	 *  [Strength, Magic, Agility]. Empty array if any pool is empty (shouldn't happen in shipped game).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Cards")
	static TArray<FOryxCard> RollCards();

	/**
	 *  Pushes the card's modifier onto the target StatsComponent.
	 *  Returns the assigned GUID — store this somewhere (e.g. RunSave) so the modifier
	 *  can be cleanly removed on run end via StatsComponent->RemoveModifiersBySource.
	 *  Returns an invalid GUID if Stats is null or Card has no meaningful modifier.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Cards")
	static FGuid ApplyCardToStats(const FOryxCard& Card, UOryxStatsComponent* Stats);
};
