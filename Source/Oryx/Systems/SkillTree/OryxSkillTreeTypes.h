#pragma once

#include "CoreMinimal.h"
#include "Component/Stats/OryxStatTypes.h"
#include "OryxSkillTreeTypes.generated.h"

/**
 *  Tree type identity. Per D14:
 *    - Passive tree: linear, 20 nodes for demo / 60 for full release. 1 passive point per level.
 *    - Ability tree: cluster (Last-Epoch model), 10 nodes × 3 ranks each. 1 ability point per 2 levels.
 *      Center node = base ability (granted free). Max 15 invested per ability tree.
 */
UENUM(BlueprintType)
enum class EOryxSkillTreeType : uint8
{
	Passive UMETA(DisplayName = "Passive (linear)"),
	Ability UMETA(DisplayName = "Ability (cluster, 10 nodes x 3 ranks)")
};

/**
 *  A single node in a skill tree. Pure stat upgrade per D14 — no behavior changes for demo.
 *  Stepped rank values (each rank = same fixed value per the spec).
 */
USTRUCT(BlueprintType)
struct ORYX_API FOryxSkillNode
{
	GENERATED_BODY()

	/** Stable id for save/load + cross-node references */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;

	/** Max ranks the player can invest. Passive tree nodes typically 1; ability tree nodes 3 (per D14). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MaxRanks = 1;

	/** Hard prereqs — list of NodeIds that must have >=1 rank invested before this node can be invested. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> RequiredNodes;

	/** Soft gate — total points required in the same tree before this node unlocks (D14). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 RequiredPointsInTree = 0;

	/**
	 *  Modifiers applied per rank. Each rank applies its corresponding element on top of previous ranks
	 *  (stepped — so the per-rank stat granted at rank 2 is added to rank 1's, not replacing it).
	 *  For stepped consistency the typical pattern is identical entries: e.g. [+5%, +5%, +5%].
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FOryxStatModifier> ModifiersPerRank;

	/**
	 *  True if this is the free base-ability node at the center of an ability cluster tree (D14).
	 *  Granted automatically — players don't spend a point to acquire.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsBaseAbility = false;
};
