#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OryxPlayerState.generated.h"

class UOryxCurrencyComponent;
class UOryxLevelComponent;
class UOryxInventoryComponent;
class UOryxItem;
class AOryxVendor;

/**
 *  Per-player persistent run state.
 *  Owns currency (gold/onyx) so it survives pawn destruction/respawn
 *  and replicates cleanly to all clients in co-op.
 */
UCLASS()
class ORYX_API AOryxPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOryxPlayerState();

	/** Returns the currency component owned by this player state */
	UFUNCTION(BlueprintPure, Category = "Oryx|PlayerState")
	UOryxCurrencyComponent* GetCurrencyComponent() const { return CurrencyComponent; }

	/** Returns the level component (XP / level / level-up). MVP placeholder; per-class tracking arrives in Phase 4. */
	UFUNCTION(BlueprintPure, Category = "Oryx|PlayerState")
	UOryxLevelComponent* GetLevelComponent() const { return LevelComponent; }

	/** Returns the inventory component. Holds owned items + tracks applied stat modifiers for clean removal. */
	UFUNCTION(BlueprintPure, Category = "Oryx|PlayerState")
	UOryxInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	/**
	 *  Current stage of the run (1/2/3). Drives item cost/sell value via UOryxItemEconomy.
	 *  Written by OryxRunManager when it lands in Phase 3; defaults to 1 for Phase 2 Stage-1 work.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Run")
	int32 CurrentStage = 1;

	// --- Transaction request layer (UI -> gameplay; future Server, Reliable RPCs in co-op) ---

	/**
	 *  Buy `Item` from `Vendor`. Validates stock + cost, deducts gold, adds to inventory,
	 *  consumes one vendor stock unit. Returns true on success, false if out of stock or unaffordable.
	 *  Atomic — partial spend never happens (gold guarded by CanAfford check first).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Vendor")
	bool RequestBuyItem(UOryxItem* Item, AOryxVendor* Vendor);

	/**
	 *  Sell `Item` for 50% of its current-stage cost (D6/D7). Removes from inventory + refunds gold.
	 *  Returns true on success, false if the player doesn't own this item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Vendor")
	bool RequestSellItem(UOryxItem* Item);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleLevelUp(int32 NewLevel, int32 OnyxAwarded);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxCurrencyComponent* CurrencyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxLevelComponent* LevelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxInventoryComponent* InventoryComponent;
};
