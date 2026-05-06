#include "OryxInventoryComponent.h"
#include "Items/OryxItem.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

UOryxInventoryComponent::UOryxInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UOryxInventoryComponent::AddItem(UOryxItem* Item)
{
	if (!Item) return false;

	FOryxOwnedItem Owned;
	Owned.Item = Item;

	// Push each modifier on the controlled pawn's StatsComponent and remember the GUID.
	if (UOryxStatsComponent* Stats = ResolveStatsComponent())
	{
		Owned.AppliedModifierIds.Reserve(Item->StatModifiers.Num());
		for (const FOryxStatModifier& Template : Item->StatModifiers)
		{
			FOryxStatModifier Mod = Template;
			// Force a fresh GUID per application — even if the asset has one stamped.
			Mod.SourceId = FGuid::NewGuid();
			const FGuid AppliedId = Stats->AddModifier(Mod);
			Owned.AppliedModifierIds.Add(AppliedId);
		}
	}
	// If no Stats yet (e.g. pawn unspawned), modifiers are NOT applied.
	// Acceptable for MVP — items are added on the live run, pawn always exists.

	OwnedItems.Add(Owned);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UOryxInventoryComponent::RemoveItem(UOryxItem* Item)
{
	if (!Item) return false;

	for (int32 i = 0; i < OwnedItems.Num(); ++i)
	{
		if (OwnedItems[i].Item == Item)
		{
			if (UOryxStatsComponent* Stats = ResolveStatsComponent())
			{
				for (const FGuid& Id : OwnedItems[i].AppliedModifierIds)
				{
					Stats->RemoveModifiersBySource(Id);
				}
			}
			OwnedItems.RemoveAt(i);
			OnInventoryChanged.Broadcast();
			return true;
		}
	}
	return false;
}

void UOryxInventoryComponent::ClearInventory()
{
	if (UOryxStatsComponent* Stats = ResolveStatsComponent())
	{
		for (const FOryxOwnedItem& Owned : OwnedItems)
		{
			for (const FGuid& Id : Owned.AppliedModifierIds)
			{
				Stats->RemoveModifiersBySource(Id);
			}
		}
	}
	OwnedItems.Reset();
	OnInventoryChanged.Broadcast();
}

int32 UOryxInventoryComponent::CountItem(const UOryxItem* Item) const
{
	if (!Item) return 0;
	int32 Count = 0;
	for (const FOryxOwnedItem& Owned : OwnedItems)
	{
		if (Owned.Item && Owned.Item->ItemId == Item->ItemId) ++Count;
	}
	return Count;
}

UOryxStatsComponent* UOryxInventoryComponent::ResolveStatsComponent() const
{
	const APlayerState* PS = Cast<APlayerState>(GetOwner());
	if (!PS) return nullptr;
	const APawn* Pawn = PS->GetPawn();
	if (!Pawn) return nullptr;
	return Pawn->FindComponentByClass<UOryxStatsComponent>();
}
