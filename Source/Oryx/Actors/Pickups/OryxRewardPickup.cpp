#include "OryxRewardPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#include "Characters/Player/OryxCharacter.h"

AOryxRewardPickup::AOryxRewardPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(80.f);
	CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic")); // visible to interact trace
	RootComponent = CollisionSphere;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AOryxRewardPickup::Interact_Implementation(AOryxCharacter* Interactor)
{
	if (!IsValid(Interactor) || !RewardWidgetClass)
	{
		Destroy();
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Interactor->GetController());
	if (!PC)
	{
		Destroy();
		return;
	}

	if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, RewardWidgetClass))
	{
		Widget->AddToViewport();

		// Co-op friendly: Game and UI lets this player click buttons while still moving.
		// Other players are completely unaffected.
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(Widget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	// Single-use — once interacted, the reward is "claimed" via the widget.
	Destroy();
}
