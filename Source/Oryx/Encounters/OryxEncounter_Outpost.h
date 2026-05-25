#pragma once

#include "CoreMinimal.h"
#include "Encounters/OryxEncounter.h"
#include "OryxEncounter_Outpost.generated.h"

class AOryxVendor;

/**
 *  Outpost encounter — large enemy presence (~30) that auto-engages when the player
 *  enters the area. On clear: spawn a vendor in the cleared zone + reward pickup.
 *
 *  Pattern matches Camp (auto-start, no Interact gate) but tuned bigger.
 *  Per plan Phase 3 — reward also includes card pick (handled BP-side via RewardPickup widget).
 */
UCLASS()
class ORYX_API AOryxEncounter_Outpost : public AOryxEncounter
{
	GENERATED_BODY()

public:
	AOryxEncounter_Outpost();

protected:
	virtual void BeginPlay() override;

	/** Auto-spawn enemies at level start. Match Camp behavior. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Outpost")
	bool bAutoStart = true;

	/** Vendor class spawned at this outpost's location after clear. Null = no vendor. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Outpost")
	TSubclassOf<AOryxVendor> VendorClassToSpawn;

	/** Local offset from outpost location where the vendor appears */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Outpost")
	FVector VendorSpawnOffset = FVector(0.f, 0.f, 0.f);

	UFUNCTION()
	void HandleOutpostCleared(AOryxEncounter* Encounter);
};
