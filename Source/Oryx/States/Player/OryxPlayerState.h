#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OryxPlayerState.generated.h"

class UOryxCurrencyComponent;
class UOryxLevelComponent;
class UOryxInventoryComponent;

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
