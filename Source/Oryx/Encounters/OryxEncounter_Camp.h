#pragma once

#include "CoreMinimal.h"
#include "Encounters/OryxEncounter.h"
#include "OryxEncounter_Camp.generated.h"

/**
 *  Camp encounter — enemies are present in the world from level start.
 *  Players approach, fight through, and a reward pickup spawns when all are dead.
 *  No trigger volume, no "activation" step.
 */
UCLASS()
class ORYX_API AOryxEncounter_Camp : public AOryxEncounter
{
	GENERATED_BODY()

public:
	AOryxEncounter_Camp();

protected:
	virtual void BeginPlay() override;

	/** If true, enemies spawn automatically when the level loads. Disable to control externally. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter")
	bool bAutoStart = true;
};
