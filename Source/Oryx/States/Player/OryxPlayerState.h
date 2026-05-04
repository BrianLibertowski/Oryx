#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OryxPlayerState.generated.h"

class UOryxCurrencyComponent;

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxCurrencyComponent* CurrencyComponent;
};
