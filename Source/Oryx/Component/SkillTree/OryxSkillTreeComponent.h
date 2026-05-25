#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/OryxClassTypes.h"
#include "Component/Stats/OryxStatTypes.h"
#include "OryxSkillTreeComponent.generated.h"

class UOryxSkillTree;
class UOryxStatsComponent;
class UOryxLevelComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillAllocationsChanged);

/**
 *  Allocated ranks for one class. NodeId → current invested rank (0 = not invested).
 *  Persisted via UOryxProfileSave.
 */
USTRUCT(BlueprintType)
struct ORYX_API FOryxSkillTreeAllocations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, int32> NodeRanks;

	/** SourceIds of all stat modifiers currently pushed by this class's skill nodes. For clean revoke on respec. */
	UPROPERTY()
	TArray<FGuid> AppliedModifierIds;
};

/**
 *  Tracks per-class skill tree allocations (D14). Lives on PlayerState alongside LevelComponent.
 *
 *  Invest() consumes a skill point from LevelComponent (passive or ability based on tree type),
 *  bumps NodeRanks[NodeId], and pushes the node's per-rank modifier onto the controlled pawn's StatsComponent.
 *  Tracks the modifier's GUID in AppliedModifierIds so RefundAll can clean them up.
 *
 *  Free respec per D14: RefundAll() removes all pushed modifiers + restores all points back to LevelComponent.
 *  Restricted to main menu edit only — runtime callers should gate on a "bIsInRun" flag (TODO when RunManager wires).
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxSkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxSkillTreeComponent();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|SkillTree")
	FOnSkillAllocationsChanged OnAllocationsChanged;

	/**
	 *  Can the player invest one more rank into this node right now?
	 *  Checks: node exists, current rank < MaxRanks, soft+hard prereqs satisfied, MaxInvestedInTree not exceeded,
	 *  player has unallocated points of the correct type (passive vs ability).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|SkillTree")
	bool CanInvest(EOryxClass Class, const UOryxSkillTree* Tree, FName NodeId) const;

	/**
	 *  Invest one rank. Consumes a skill point + pushes the rank's modifier to StatsComponent.
	 *  Returns true on success, false on validation failure (call CanInvest first to know why).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|SkillTree")
	bool Invest(EOryxClass Class, UOryxSkillTree* Tree, FName NodeId);

	/**
	 *  Free respec for a class (D14). Removes all pushed stat modifiers + restores all spent points
	 *  back to LevelComponent's unallocated pools. Wipes allocations to empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|SkillTree")
	void RefundAll(EOryxClass Class);

	UFUNCTION(BlueprintPure, Category = "Oryx|SkillTree")
	int32 GetRank(EOryxClass Class, FName NodeId) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|SkillTree")
	int32 GetTotalInvestedInTree(EOryxClass Class, const UOryxSkillTree* Tree) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|SkillTree")
	FOryxSkillTreeAllocations GetAllocations(EOryxClass Class) const;

	/** Save/load integration — direct-set used by UOryxProfileSave restore. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|SkillTree|Save")
	void SetAllocations(EOryxClass Class, const FOryxSkillTreeAllocations& Allocations);

	UFUNCTION(BlueprintPure, Category = "Oryx|SkillTree|Save")
	const TMap<EOryxClass, FOryxSkillTreeAllocations>& GetAllAllocations() const { return ClassAllocations; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|SkillTree")
	TMap<EOryxClass, FOryxSkillTreeAllocations> ClassAllocations;

private:
	/** Resolve the StatsComponent of the controlled pawn for the PlayerState that owns this component. */
	UOryxStatsComponent* ResolveStatsComponent() const;

	/** Resolve the LevelComponent on the same PlayerState (sibling component). */
	UOryxLevelComponent* ResolveLevelComponent() const;
};
