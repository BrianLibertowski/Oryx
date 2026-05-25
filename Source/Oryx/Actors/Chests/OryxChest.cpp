#include "OryxChest.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Characters/Player/OryxCharacter.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Inventory/OryxInventoryComponent.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "States/Player/OryxPlayerState.h"
#include "Items/OryxItem.h"
#include "Actors/Pickups/OryxRewardPickup.h"

AOryxChest::AOryxChest()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->InitSphereRadius(160.f);
	RootComponent = InteractSphere;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetupAttachment(RootComponent);
}

int32 AOryxChest::GetCost() const
{
	const int32 Idx = FMath::Clamp(Stage - 1, 0, CostPerStage.Num() - 1);
	if (CostPerStage.IsValidIndex(Idx))
	{
		return CostPerStage[Idx];
	}
	return 0;
}

void AOryxChest::Interact_Implementation(AOryxCharacter* Interactor)
{
	if (bOpened || !Interactor) return;

	AOryxPlayerState* PS = Interactor->GetPlayerState<AOryxPlayerState>();
	if (!PS) return;

	UOryxCurrencyComponent* Currency = PS->GetCurrencyComponent();
	UOryxInventoryComponent* Inventory = PS->GetInventoryComponent();
	if (!Currency || !Inventory) return;

	const int32 Cost = GetCost();
	if (!Currency->CanAfford(Cost))
	{
		UE_LOG(LogTemp, Log, TEXT("Chest: player can't afford %d gold (has %d)"), Cost, Currency->GetGold());
		return;
	}

	if (!Currency->SpendGold(Cost)) return;

	// Roll rarity, pick item, grant. If no item matches the rolled rarity, downgrade to Common.
	const EOryxItemRarity Rolled = RollRarity(Interactor);
	UOryxItem* Item = PickItemOfRarity(Rolled);
	if (!Item)
	{
		Item = PickItemOfRarity(EOryxItemRarity::Common);
	}
	if (!Item)
	{
		// ItemPool is empty or malformed — refund and bail.
		Currency->AddGold(Cost);
		UE_LOG(LogTemp, Warning, TEXT("Chest '%s': ItemPool empty — refunded."), *GetName());
		return;
	}

	Inventory->AddItem(Item);
	bOpened = true;

	UE_LOG(LogTemp, Log, TEXT("Chest opened: granted %s (rarity %d)"),
		*Item->DisplayName.ToString(), static_cast<int32>(Rolled));

	// TODO: visual feedback — disable interact mesh, swap to "opened" mesh, particle effect.
}

EOryxItemRarity AOryxChest::RollRarity(AOryxCharacter* Interactor) const
{
	// Base weights from UOryxItemEconomy (currently Stage 1 only — Stage 2/3 deferred).
	int32 Weights[5] = {
		UOryxItemEconomy::GetStage1Weight(EOryxItemRarity::Common),
		UOryxItemEconomy::GetStage1Weight(EOryxItemRarity::Uncommon),
		UOryxItemEconomy::GetStage1Weight(EOryxItemRarity::Rare),
		UOryxItemEconomy::GetStage1Weight(EOryxItemRarity::Epic),
		UOryxItemEconomy::GetStage1Weight(EOryxItemRarity::Legendary)
	};

	// Luck shift: each point of Luck moves probability mass from Common toward Uncommon+.
	// Placeholder formula — tune when chest balance pass happens.
	if (Interactor && LuckWeightShift > 0.f)
	{
		if (UOryxStatsComponent* Stats = Interactor->GetStatsComponent())
		{
			const float Luck = Stats->GetStat(EOryxStat::Luck);
			const int32 Shift = FMath::FloorToInt(Luck * LuckWeightShift);
			if (Shift > 0)
			{
				const int32 FromCommon = FMath::Min(Weights[0] - 1, Shift);
				Weights[0] = FMath::Max(1, Weights[0] - FromCommon);
				// Distribute the shift across Uncommon..Legendary, weighted by current values.
				Weights[1] += FromCommon * 2 / 5;
				Weights[2] += FromCommon * 1 / 5;
				Weights[3] += FromCommon * 1 / 5;
				Weights[4] += FromCommon * 1 / 5;
			}
		}
	}

	int32 Total = 0;
	for (int32 W : Weights) Total += W;
	if (Total <= 0) return EOryxItemRarity::Common;

	int32 Roll = FMath::RandRange(0, Total - 1);
	for (int32 i = 0; i < 5; ++i)
	{
		Roll -= Weights[i];
		if (Roll < 0) return static_cast<EOryxItemRarity>(i);
	}
	return EOryxItemRarity::Common;
}

UOryxItem* AOryxChest::PickItemOfRarity(EOryxItemRarity Rarity) const
{
	TArray<UOryxItem*> Matching;
	Matching.Reserve(ItemPool.Num());
	for (const TObjectPtr<UOryxItem>& Entry : ItemPool)
	{
		if (Entry && Entry->Rarity == Rarity)
		{
			Matching.Add(Entry);
		}
	}
	if (Matching.Num() == 0) return nullptr;
	return Matching[FMath::RandRange(0, Matching.Num() - 1)];
}
