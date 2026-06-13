#pragma once

#include "CoreMinimal.h"
#include "Component/Stats/OryxStatTypes.h"
#include "OryxSkillTreeTypes.generated.h"

/**
 *  Visual state of a node for the skill tree UI. Maps to the mockup legend
 *  (Allocated / Not Allocated / Locked + the free-grant center).
 */
UENUM(BlueprintType)
enum class EOryxNodeState : uint8
{
	Locked    UMETA(DisplayName = "Locked"),     // prereqs or point gate not satisfied
	Available UMETA(DisplayName = "Available"),   // 0 invested, can invest now
	Partial   UMETA(DisplayName = "Partial"),     // some ranks invested, more available
	Maxed     UMETA(DisplayName = "Maxed"),       // at MaxRanks
	Granted   UMETA(DisplayName = "Granted")      // free base-ability center node
};

/**
 *  Everything the skill tree UI needs to render ONE node — computed in C++ so the node widget
 *  is pure presentation (no state logic per widget). Returned by
 *  UOryxSkillTreeComponent::GetNodeDisplayInfo.
 */
USTRUCT(BlueprintType)
struct ORYX_API FOryxSkillNodeDisplayInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName NodeId;
	UPROPERTY(BlueprintReadOnly) FText DisplayName;
	UPROPERTY(BlueprintReadOnly) FText Description;
	UPROPERTY(BlueprintReadOnly) int32 CurrentRank = 0;
	UPROPERTY(BlueprintReadOnly) int32 MaxRanks = 1;
	UPROPERTY(BlueprintReadOnly) EOryxNodeState State = EOryxNodeState::Locked;

	/** True if the player can invest one more rank right now (drives Invest button enabled). */
	UPROPERTY(BlueprintReadOnly) bool bCanInvest = false;

	/** "2 / 3" rank display. */
	UPROPERTY(BlueprintReadOnly) FText RankText;

	/** Multi-line stat preview, "+10% → +20% Attack Speed" per modifier (via UOryxStatFormatLibrary). */
	UPROPERTY(BlueprintReadOnly) FText ModifierPreview;

	/** Why the node can't be invested ("Requires 4 points in tree", "Requires: Brute Force",
	 *  "No ability points"). Empty when investable or already maxed. Drives hover tooltips. */
	UPROPERTY(BlueprintReadOnly) FText LockReason;
};

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
 *  The full set of stat modifiers granted by ONE rank of a node. Wrapping the modifier list in a
 *  struct lets a single rank grant MULTIPLE stats (keystones: "Armor +10 AND MaxHealth +40";
 *  apex nodes: two stats per rank) — a flat per-rank single-modifier array couldn't express that.
 *
 *  Stepped design (D14): for a multi-rank single-stat node, every rank holds an identical Modifiers
 *  list (e.g. each rank = [AttackSpeed +0.10]); investing 3 ranks pushes that modifier 3×.
 */
USTRUCT(BlueprintType)
struct ORYX_API FOryxSkillRank
{
	GENERATED_BODY()

	/** Every modifier in this list is applied when the player invests this rank. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FOryxStatModifier> Modifiers;
};

/**
 *  A single node in a skill tree. Pure stat upgrade per D14 — no behavior changes for demo.
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
	 *  Per-rank modifier sets, indexed by rank (Ranks[0] = rank 1, Ranks[1] = rank 2, …).
	 *  Each entry can hold multiple modifiers (multi-stat keystones/apexes). Authoring patterns:
	 *    - Single-stat 3-rank: 3 entries, each Modifiers=[the same stat] (stepped).
	 *    - Keystone 1-rank multi: 1 entry, Modifiers=[stat A, stat B, stat C].
	 *    - Apex 3-rank multi: 3 entries, each Modifiers=[stat A, stat B].
	 *  Entries beyond MaxRanks are ignored; missing entries for a rank grant nothing (graceful).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FOryxSkillRank> Ranks;

	/**
	 *  True if this is the free base-ability node at the center of an ability cluster tree (D14).
	 *  Granted automatically — players don't spend a point to acquire. Leave Ranks empty.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsBaseAbility = false;
};
