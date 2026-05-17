#include "OryxPlayerState.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "Component/Inventory/OryxInventoryComponent.h"
#include "Items/OryxItem.h"
#include "Actors/Vendors/OryxVendor.h"

AOryxPlayerState::AOryxPlayerState()
{
	CurrencyComponent = CreateDefaultSubobject<UOryxCurrencyComponent>(TEXT("CurrencyComponent"));
	LevelComponent = CreateDefaultSubobject<UOryxLevelComponent>(TEXT("LevelComponent"));
	InventoryComponent = CreateDefaultSubobject<UOryxInventoryComponent>(TEXT("InventoryComponent"));
}

void AOryxPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// D13: each level-up grants Onyx. LevelComponent broadcasts the amount; we route to currency.
	if (LevelComponent)
	{
		LevelComponent->OnLevelUp.AddDynamic(this, &AOryxPlayerState::HandleLevelUp);
	}
}

void AOryxPlayerState::HandleLevelUp(int32 /*NewLevel*/, int32 OnyxAwarded)
{
	if (CurrencyComponent && OnyxAwarded > 0)
	{
		CurrencyComponent->AddOnyx(OnyxAwarded);
	}
}

bool AOryxPlayerState::RequestBuyItem(UOryxItem* Item, AOryxVendor* Vendor)
{
	if (!Item || !Vendor || !CurrencyComponent || !InventoryComponent) return false;

	// Stock check first — if vendor has none, abort before touching gold.
	bool bHasStock = false;
	for (const FOryxVendorStockSlot& Slot : Vendor->GetStock())
	{
		if (Slot.Item == Item && Slot.Stock > 0)
		{
			bHasStock = true;
			break;
		}
	}
	if (!bHasStock) return false;

	const int32 Cost = Item->GetCost(Vendor->Stage);
	if (!CurrencyComponent->CanAfford(Cost)) return false;

	// Atomic: spend → consume stock → grant. If any link fails after spend, revert.
	if (!CurrencyComponent->SpendGold(Cost)) return false;

	if (!Vendor->ConsumeStock(Item))
	{
		// Stock vanished between check and consume (shouldn't happen single-player). Refund.
		CurrencyComponent->AddGold(Cost);
		return false;
	}

	if (!InventoryComponent->AddItem(Item))
	{
		// Inventory rejected the add (no slot cap right now, so unexpected). Refund + restore stock.
		CurrencyComponent->AddGold(Cost);
		// No restore-stock helper; one unit lost. Acceptable until AddItem can actually fail.
		return false;
	}
	return true;
}

bool AOryxPlayerState::RequestSellItem(UOryxItem* Item)
{
	if (!Item || !CurrencyComponent || !InventoryComponent) return false;

	const int32 Refund = Item->GetSellValue(CurrentStage);
	if (!InventoryComponent->RemoveItem(Item)) return false;

	CurrencyComponent->AddGold(Refund);
	return true;
}
