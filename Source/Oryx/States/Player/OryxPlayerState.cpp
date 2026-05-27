#include "OryxPlayerState.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Level/OryxLevelComponent.h"
#include "Component/Inventory/OryxInventoryComponent.h"
#include "Component/SkillTree/OryxSkillTreeComponent.h"
#include "Component/Health/OryxHealthComponent.h"
#include "Items/OryxItem.h"
#include "Actors/Vendors/OryxVendor.h"
#include "GameModes/Base/OryxGameInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

namespace
{
	/**
	 *  True when this PlayerState's owning PlayerController is local to this machine.
	 *  Single-player → always true. Listen server → true for host's PS, false for remote clients'.
	 *  Dedicated server → always false (no local PC).
	 */
	bool IsLocalPlayerState(const APlayerState* PS)
	{
		if (!PS) return false;
		const APlayerController* PC = PS->GetPlayerController();
		return PC && PC->IsLocalController();
	}
}

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

	// Restore persisted profile (per-class XP/level/unallocated points + skill allocations).
	// Apply must happen AFTER the OnLevelUp bind so that any future level-ups route correctly.
	// SetProgression is a direct write — no spurious OnLevelUp broadcast for restored levels.
	//
	// Co-op proofing: only apply the LOCAL machine's profile to a locally-owned PlayerState.
	// In single-player this is always true. In co-op, remote clients on the server (or other
	// clients) must NOT have the host's profile written into their LevelComponent — they get
	// their data via replication once that lands.
	if (IsLocalPlayerState(this))
	{
		if (UOryxGameInstance* GI = Cast<UOryxGameInstance>(GetGameInstance()))
		{
			GI->ApplyProfileToPlayerState(this);
		}
	}
}

void AOryxPlayerState::HandleLevelUp(EOryxClass ClassEnum, int32 NewLevel)
{
	// D13 REVOKED: no Onyx grant. Skill points are awarded inside LevelComponent on the per-class progression struct.

	// Heal-to-full on level-up (Phase 4 plan). Pulls the pawn's HealthComponent.
	if (APawn* OwnedPawn = GetPawn())
	{
		if (UOryxHealthComponent* HC = OwnedPawn->FindComponentByClass<UOryxHealthComponent>())
		{
			HC->ApplyHealing(HC->GetMaxHealth());
		}
	}

	// Broadcast for BP-side toast / SFX / VFX bindings.
	OnPlayerLevelUp.Broadcast(ClassEnum, NewLevel);

	// Persist immediately so progression survives a crash mid-run.
	// Co-op proofing: only the local PlayerState writes to this machine's profile slot.
	// Remote players' progression goes into their own machines' profiles on level-up there.
	if (IsLocalPlayerState(this))
	{
		if (UOryxGameInstance* GI = Cast<UOryxGameInstance>(GetGameInstance()))
		{
			GI->SaveProfile(this);
		}
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
