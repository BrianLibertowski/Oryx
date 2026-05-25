#include "OryxPlayerState.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "Component/Inventory/OryxInventoryComponent.h"
#include "Component/SkillTree/OryxSkillTreeComponent.h"
#include "Items/OryxItem.h"
#include "Actors/Vendors/OryxVendor.h"

AOryxPlayerState::AOryxPlayerState()
{
	CurrencyComponent = CreateDefaultSubobject<UOryxCurrencyComponent>(TEXT("CurrencyComponent"));
	LevelComponent = CreateDefaultSubobject<UOryxLevelComponent>(TEXT("LevelComponent"));
	InventoryComponent = CreateDefaultSubobject<UOryxInventoryComponent>(TEXT("InventoryComponent"));
	SkillTreeComponent = CreateDefaultSubobject<UOryxSkillTreeComponent>(TEXT("SkillTreeComponent"));
}

void AOryxPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (LevelComponent)
	{
		LevelComponent->OnLevelUp.AddDynamic(this, &AOryxPlayerState::HandleLevelUp);
	}
}

void AOryxPlayerState::HandleLevelUp(EOryxClass /*ClassEnum*/, int32 /*NewLevel*/)
{
	// D13 REVOKED: no Onyx grant. Skill points are awarded inside LevelComponent on the per-class progression struct.
	// Phase 4 TODO: heal-to-full on level-up + toast UI (BP_OryxCharacter listens to this delegate too,
	// or we add a HUD-side bind in WBP_PlayerHUD).
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
