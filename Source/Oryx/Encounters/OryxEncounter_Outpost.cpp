#include "OryxEncounter_Outpost.h"
#include "Actors/Vendors/OryxVendor.h"

AOryxEncounter_Outpost::AOryxEncounter_Outpost()
{
	// Outpost defaults: ~30 enemies, big gold reward (100-150 per plan), big XP.
	// Override per-instance.
	EnemyCount = 30;
	SpawnRadius = 2500.f;
	GoldReward = 125;
	XPReward = 250;
}

void AOryxEncounter_Outpost::BeginPlay()
{
	Super::BeginPlay();

	OnEncounterComplete.AddDynamic(this, &AOryxEncounter_Outpost::HandleOutpostCleared);

	if (bAutoStart)
	{
		// nullptr InstigatingPlayer — base picks nearest alive player for rewards.
		StartEncounter(nullptr);
	}
}

void AOryxEncounter_Outpost::HandleOutpostCleared(AOryxEncounter* /*Encounter*/)
{
	if (!VendorClassToSpawn) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLoc = GetActorLocation() + VendorSpawnOffset;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AOryxVendor* SpawnedVendor = World->SpawnActor<AOryxVendor>(VendorClassToSpawn, SpawnLoc, FRotator::ZeroRotator, Params);
	if (!SpawnedVendor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Outpost '%s': failed to spawn vendor"), *GetName());
	}
}
