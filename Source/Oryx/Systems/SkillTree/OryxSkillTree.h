#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Characters/OryxClassTypes.h"
#include "OryxSkillTreeTypes.h"
#include "OryxSkillTree.generated.h"

/**
 *  Authoring data for a single skill tree. Per D14 each class gets:
 *    - 1 passive tree (linear, 20 nodes demo / 60 full)
 *    - 5 ability trees (cluster, 10 nodes × 3 ranks each, max 15 points invested per tree)
 *
 *  4 classes × (1 passive + 5 ability) = 24 UOryxSkillTree DataAssets at full demo content.
 *
 *  Authoring flow: right-click Content/SkillTrees/ → Miscellaneous → Data Asset → UOryxSkillTree.
 *  Recommended path: /Game/SkillTrees/<Class>/<TreeType>_<Name>
 *    e.g. /Game/SkillTrees/Warrior/Passive_WarriorMain
 *         /Game/SkillTrees/Warrior/Ability_Slash
 *         /Game/SkillTrees/Warrior/Ability_Whirlwind
 *         /Game/SkillTrees/Warrior/Ability_Charge
 *         /Game/SkillTrees/Warrior/Ability_BerserkRoar
 *         /Game/SkillTrees/Warrior/Ability_<5th>
 */
UCLASS(BlueprintType)
class ORYX_API UOryxSkillTree : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	EOryxClass OwningClass = EOryxClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	EOryxSkillTreeType TreeType = EOryxSkillTreeType::Passive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	FText TreeName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	TArray<FOryxSkillNode> Nodes;

	/**
	 *  Per D14: ability trees cap at 15 points invested per tree (passive trees cap at full size — 20 demo).
	 *  Leave 0 for "no cap beyond MaxRanks per node".
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	int32 MaxInvestedInTree = 15;

	/**
	 *  Helper: look up a node by id. Returns nullptr if not found.
	 *  C++-only (UHT forbids exposing raw USTRUCT pointers to BP). If WBP_SkillTreeScreen needs
	 *  BP node lookup later, add a sibling `bool FindNodeBP(FName, FOryxSkillNode& OutNode)` accessor.
	 */
	const FOryxSkillNode* FindNode(FName NodeId) const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
