#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OryxInteractable.h"
#include "OryxVendor.generated.h"

class UOryxItem;
class UStaticMeshComponent;
class USphereComponent;
class UUserWidget;

/** A single rolled stock entry — what's on the rack. */
USTRUCT(BlueprintType)
struct FOryxVendorStockSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UOryxItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Stock = 1;
};

/**
 *  Stationary vendor (D9). Player walks up, presses Interact, vendor opens its UI.
 *  Stock is rolled at run start from ItemPool, weighted by rarity (Stage 1 weights).
 *
 *  Phase 2 scaffold — UI widget + run-start integration come with the full implementation.
 *  TODO: WBP_VendorScreen, run-start hook (probably from OryxRunManager when it lands), buy/sell handlers.
 */
UCLASS()
class ORYX_API AOryxVendor : public AActor, public IOryxInteractable
{
	GENERATED_BODY()

public:
	AOryxVendor();

	/** Items this vendor can roll at run start. Configure per-instance or per-class. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Vendor")
	TArray<TObjectPtr<UOryxItem>> ItemPool;

	/** How many distinct slots to roll. Plan: 6-8 per shop. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Vendor")
	int32 SlotCount = 6;

	/** Stage this vendor sits in (1/2/3). Drives cost via UOryxItemEconomy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Vendor")
	int32 Stage = 1;

	/** Widget shown on Interact. Set in BP. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oryx|Vendor")
	TSubclassOf<UUserWidget> VendorWidgetClass;

	/** Rolls fresh stock from ItemPool (rarity-weighted). Called at run start. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Vendor")
	void RollStock();

	/** Decrements stock for the item (called by the vendor UI on a successful purchase). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Vendor")
	bool ConsumeStock(UOryxItem* Item);

	UFUNCTION(BlueprintPure, Category = "Oryx|Vendor")
	const TArray<FOryxVendorStockSlot>& GetStock() const { return Stock; }

	// IOryxInteractable
	virtual void Interact_Implementation(AOryxCharacter* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Vendor")
	TArray<FOryxVendorStockSlot> Stock;
};
