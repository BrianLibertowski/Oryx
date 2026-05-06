#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxInventoryComponent.generated.h"

class UOryxItem;
class UOryxStatsComponent;
class AOryxCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/** Per-item record: which item, plus the GUIDs of every modifier we pushed for it. */
USTRUCT(BlueprintType)
struct FOryxOwnedItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UOryxItem> Item = nullptr;

	/** GUIDs returned by StatsComponent.AddModifier — used to remove them cleanly on sell/drop. */
	UPROPERTY()
	TArray<FGuid> AppliedModifierIds;
};

/**
 *  Tracks the player's owned items and pushes their stat modifiers onto the
 *  controlled character's StatsComponent. Lives on AOryxPlayerState — survives
 *  pawn destruction so loot is preserved across respawn (when respawn lands).
 *
 *  Phase 2 scaffold — UI + drop logic come with the vendor implementation.
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Inventory")
	FOnInventoryChanged OnInventoryChanged;

	/**
	 *  Adds an item to inventory and applies its stat modifiers.
	 *  Returns true if added (currently always true — no slot limit per D5 flat inventory).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Inventory")
	bool AddItem(UOryxItem* Item);

	/** Removes the FIRST matching item by ItemId, removes its modifiers. Returns true if removed. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Inventory")
	bool RemoveItem(UOryxItem* Item);

	/** Wipes everything (run end / death / debug). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Inventory")
	void ClearInventory();

	UFUNCTION(BlueprintPure, Category = "Oryx|Inventory")
	int32 GetItemCount() const { return OwnedItems.Num(); }

	UFUNCTION(BlueprintPure, Category = "Oryx|Inventory")
	const TArray<FOryxOwnedItem>& GetOwnedItems() const { return OwnedItems; }

	/** Counts how many copies of this item we own (by ItemId). */
	UFUNCTION(BlueprintPure, Category = "Oryx|Inventory")
	int32 CountItem(const UOryxItem* Item) const;

private:
	/** Resolves the StatsComponent of the controlled pawn associated with this PlayerState's owner. */
	UOryxStatsComponent* ResolveStatsComponent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FOryxOwnedItem> OwnedItems;
};
