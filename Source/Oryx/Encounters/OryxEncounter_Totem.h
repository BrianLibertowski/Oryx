#pragma once

#include "CoreMinimal.h"
#include "Encounters/OryxEncounter.h"
#include "Interfaces/OryxInteractable.h"
#include "OryxEncounter_Totem.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EOryxTotemDifficulty : uint8
{
	Easy   UMETA(DisplayName = "Easy"),
	Medium UMETA(DisplayName = "Medium"),
	Hard   UMETA(DisplayName = "Hard")
};

/**
 *  Totem-shaped interactable encounter. Player presses Interact while looking at it
 *  to start the spawn wave. One-shot: cleared totems do nothing.
 *
 *  Difficulty drives EnemyCount + GoldReward + XPReward via ConfigureForDifficulty(),
 *  called in BeginPlay. Set Difficulty per-instance in the editor; per-rarity tuning
 *  lives in the *PerDifficulty arrays below.
 */
UCLASS()
class ORYX_API AOryxEncounter_Totem : public AOryxEncounter, public IOryxInteractable
{
	GENERATED_BODY()

public:
	AOryxEncounter_Totem();

	// IOryxInteractable
	virtual void Interact_Implementation(AOryxCharacter* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Totem")
	EOryxTotemDifficulty Difficulty = EOryxTotemDifficulty::Easy;

	/** Enemy count per difficulty: [Easy, Medium, Hard]. Default 4/7/11. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Totem|Tuning")
	TArray<int32> EnemyCountPerDifficulty = { 4, 7, 11 };

	/** Gold reward per difficulty: [Easy, Medium, Hard]. Default 10/25/50 per plan D3. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Totem|Tuning")
	TArray<int32> GoldRewardPerDifficulty = { 10, 25, 50 };

	/** XP reward per difficulty: [Easy, Medium, Hard]. Default 25/60/150 — tune later. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Totem|Tuning")
	TArray<int32> XPRewardPerDifficulty = { 25, 60, 150 };

	/** Mesh per difficulty. Configure in BP defaults — applies on BeginPlay based on instance Difficulty. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Totem|Visual")
	TMap<EOryxTotemDifficulty, TObjectPtr<UStaticMesh>> MeshPerDifficulty;

protected:
	virtual void BeginPlay() override;

	/** Applies the per-difficulty values to the base encounter's EnemyCount/GoldReward/XPReward. */
	void ConfigureForDifficulty();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
};
