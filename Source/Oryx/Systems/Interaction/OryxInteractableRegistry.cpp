#include "OryxInteractableRegistry.h"
#include "Engine/World.h"

void UOryxInteractableRegistry::Register(AActor* Interactable)
{
	if (!IsValid(Interactable)) return;
	// Dedup — AddUnique on TWeakObjectPtr compares the underlying object.
	Interactables.AddUnique(Interactable);
}

void UOryxInteractableRegistry::Unregister(AActor* Interactable)
{
	if (!Interactable) return;
	Interactables.RemoveAll([Interactable](const TWeakObjectPtr<AActor>& Entry)
	{
		return Entry.Get() == Interactable;
	});
}

AActor* UOryxInteractableRegistry::GetNearestInteractable(const FVector& FromLocation, float Radius)
{
	const float RadiusSq = Radius * Radius;

	AActor* Nearest = nullptr;
	float NearestDistSq = TNumericLimits<float>::Max();

	// Single reverse pass: measure live entries, compact out dead ones.
	for (int32 i = Interactables.Num() - 1; i >= 0; --i)
	{
		AActor* Actor = Interactables[i].Get();
		if (!IsValid(Actor))
		{
			Interactables.RemoveAtSwap(i, 1, EAllowShrinking::No);
			continue;
		}

		const float DistSq = FVector::DistSquared(Actor->GetActorLocation(), FromLocation);
		if (DistSq > RadiusSq) continue;

		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			Nearest = Actor;
		}
	}

	return Nearest;
}

UOryxInteractableRegistry* UOryxInteractableRegistry::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	const UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	return World->GetSubsystem<UOryxInteractableRegistry>();
}
