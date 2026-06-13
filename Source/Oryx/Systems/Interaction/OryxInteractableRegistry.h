#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OryxInteractableRegistry.generated.h"

/**
 *  World-scoped registry of all active IOryxInteractable actors.
 *
 *  Why it exists: AOryxCharacter polls for the nearest interactable every InteractScanInterval
 *  (10Hz). The naive implementation iterated EVERY actor in the world and called
 *  ImplementsInterface on each — O(all actors) × 10Hz. With scatter content (dozens of chests +
 *  totems + enemies + pickups) that's thousands of interface checks per second for nothing.
 *
 *  This registry holds a pre-filtered list: interactables add themselves on spawn, remove on
 *  destroy. The character's poll becomes O(interactables only), no interface checks in the hot loop.
 *
 *  Registration: UOryxInteractPromptComponent registers/unregisters its owner automatically
 *  (every built-in interactable has that component). A bare IOryxInteractable without a prompt
 *  component can register itself via the BlueprintCallable Register/Unregister.
 *
 *  Co-op note: world-scoped, single instance — fine for listen server. Each client has its own
 *  registry mirroring its own world. No replication needed (it's a query accelerator, not state).
 */
UCLASS()
class ORYX_API UOryxInteractableRegistry : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Add an interactable. Safe to call twice (deduplicated). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Interact")
	void Register(AActor* Interactable);

	/** Remove an interactable. Safe to call on something never registered. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Interact")
	void Unregister(AActor* Interactable);

	/**
	 *  Closest registered interactable to FromLocation within Radius, or null if none.
	 *  Skips stale/destroyed entries (and compacts them out as it goes).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Interact")
	AActor* GetNearestInteractable(const FVector& FromLocation, float Radius);

	/** Convenience static accessor — null-safe through World. */
	static UOryxInteractableRegistry* Get(const UObject* WorldContext);

private:
	/** Weak refs so destroyed actors don't keep the registry alive or crash the poll. */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> Interactables;
};
