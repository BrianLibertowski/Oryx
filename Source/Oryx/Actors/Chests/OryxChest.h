#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OryxInteractable.h"
#include "Items/OryxItemTypes.h"
#include "OryxChest.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UOryxItem;
class AOryxCharacter;
class UOryxInteractPromptComponent;

/**
 *  Stationary chest. Player interacts → spends gold (cost by stage per D10) →
 *  rolls a rarity from the stage's weighted table (luck-modified) → picks a random
 *  item of that rarity from ItemPool → grants to player's inventory.
 *
 *  One-shot: cleared chests do nothing on re-interact.
 *  Phase 3 scaffold — luck weighting and pool filtering are stubbed minimally;
 *  iterate when chest-only items vs vendor items get authored.
 *
 *  Costs (D10):  Stage 1 = 30g  |  Stage 2 = 150g  |  Stage 3 = 300g.
 */
UCLASS()
class ORYX_API AOryxChest : public AActor, public IOryxInteractable
{
	GENERATED_BODY()

public:
	AOryxChest();

	// IOryxInteractable
	virtual void Interact_Implementation(AOryxCharacter* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "Oryx|Chest")
	bool IsOpened() const { return bOpened; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Chest")
	int32 GetCost() const;

protected:
	/** Stage this chest belongs to. Drives cost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Chest")
	int32 Stage = 1;

	/** Cost per stage [Stage1, Stage2, Stage3]. Defaults per D10: 30 / 150 / 300. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Chest|Tuning")
	TArray<int32> CostPerStage = { 30, 150, 300 };

	/** Items the chest can drop. Same UOryxItem assets vendors use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Chest")
	TArray<TObjectPtr<UOryxItem>> ItemPool;

	/**
	 *  Multiplier applied to each Luck point when shifting rarity weights toward rare items.
	 *  Each 1.0 Luck adds this much weight to Uncommon+ tiers. 0 = ignore Luck (uniform per Stage).
	 *  Tune later — placeholder formula in cpp.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Chest|Tuning", meta = (ClampMin = "0.0"))
	float LuckWeightShift = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Oryx|Chest")
	TSubclassOf<class AOryxRewardPickup> RewardPickupClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* InteractSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	/** Floating "F" prompt — auto shows/hides based on player proximity. Set PromptWidgetClass in BP. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UOryxInteractPromptComponent* InteractPromptComponent;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Chest|State")
	bool bOpened = false;

	/** Roll the rarity table for this chest's stage, shifted by interactor's Luck stat. */
	EOryxItemRarity RollRarity(AOryxCharacter* Interactor) const;

	/** Pick a random item from ItemPool that matches the rolled rarity. Returns null if no match. */
	UOryxItem* PickItemOfRarity(EOryxItemRarity Rarity) const;
};
