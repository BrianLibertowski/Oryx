#include "OryxEncounter_Totem.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/Player/OryxCharacter.h"

AOryxEncounter_Totem::AOryxEncounter_Totem()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = MeshComponent;
}

void AOryxEncounter_Totem::Interact_Implementation(AOryxCharacter* Interactor)
{
	if (bActive || bCleared) return;
	StartEncounter(Interactor);
}
