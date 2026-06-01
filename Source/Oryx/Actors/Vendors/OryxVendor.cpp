#include "OryxVendor.h"
#include "Items/OryxItem.h"
#include "Items/OryxItemTypes.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Component/InteractPrompt/OryxInteractPromptComponent.h"

AOryxVendor::AOryxVendor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->InitSphereRadius(180.f);
	RootComponent = InteractSphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	InteractPromptComponent = CreateDefaultSubobject<UOryxInteractPromptComponent>(TEXT("InteractPromptComponent"));
}

void AOryxVendor::BeginPlay()
{
	Super::BeginPlay();

	// Roll stock at spawn for now. Once OryxRunManager exists, the manager will
	// call RollStock() at run start so all vendors roll deterministically against
	// the same run seed.
	if (Stock.Num() == 0)
	{
		RollStock();
	}
}

void AOryxVendor::RollStock()
{
	Stock.Reset();
	if (ItemPool.Num() == 0 || SlotCount <= 0) return;

	// Build weight buckets per pool entry from rarity weights. Higher-rarity items
	// roll less often. Pool can repeat items — that's fine, slots will too.
	TArray<int32> Weights;
	Weights.Reserve(ItemPool.Num());
	int32 TotalWeight = 0;
	for (const TObjectPtr<UOryxItem>& Entry : ItemPool)
	{
		const int32 W = Entry ? UOryxItemEconomy::GetStage1Weight(Entry->Rarity) : 0;
		Weights.Add(W);
		TotalWeight += W;
	}
	if (TotalWeight <= 0) return;

	for (int32 Slot = 0; Slot < SlotCount; ++Slot)
	{
		int32 Roll = FMath::RandRange(0, TotalWeight - 1);
		UOryxItem* Picked = nullptr;
		for (int32 i = 0; i < ItemPool.Num(); ++i)
		{
			Roll -= Weights[i];
			if (Roll < 0)
			{
				Picked = ItemPool[i];
				break;
			}
		}
		if (!Picked) continue;

		FOryxVendorStockSlot New;
		New.Item = Picked;
		// Plan: stock per slot = randint(1, 5) + (NumPlayers - 1).
		// NumPlayers detection deferred to co-op work; for now single-player → randint(1, 5).
		New.Stock = FMath::RandRange(1, 5);
		Stock.Add(New);
	}
}

bool AOryxVendor::ConsumeStock(UOryxItem* Item)
{
	if (!Item) return false;
	for (FOryxVendorStockSlot& Slot : Stock)
	{
		if (Slot.Item == Item && Slot.Stock > 0)
		{
			--Slot.Stock;
			return true;
		}
	}
	return false;
}

void AOryxVendor::Interact_Implementation(AOryxCharacter* /*Interactor*/)
{
	// Phase 2 placeholder: log only. Real impl spawns WBP_VendorScreen with this->Stock
	// and routes buy/sell through Interactor's PlayerState (currency + inventory).
	UE_LOG(LogTemp, Log, TEXT("Vendor interacted — %d slots in stock. (UI pending)"), Stock.Num());
}
