#include "OryxInteractPromptComponent.h"

#include "Components/WidgetComponent.h"
#include "Characters/Player/OryxCharacter.h"
#include "Systems/Interaction/OryxInteractableRegistry.h"
#include "EngineUtils.h"
#include "TimerManager.h"

UOryxInteractPromptComponent::UOryxInteractPromptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxInteractPromptComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Register the owning interactable so AOryxCharacter's proximity poll finds it without
	// iterating every actor in the world.
	if (UOryxInteractableRegistry* Registry = UOryxInteractableRegistry::Get(this))
	{
		Registry->Register(Owner);
	}

	USceneComponent* OwnerRoot = Owner->GetRootComponent();
	if (!OwnerRoot)
	{
		UE_LOG(LogTemp, Warning, TEXT("OryxInteractPromptComponent on '%s' — owner has no root component, prompt skipped."), *Owner->GetName());
		return;
	}

	// Spawn the widget component dynamically attached to the owner's root.
	WidgetComp = NewObject<UWidgetComponent>(Owner, TEXT("InteractPromptWidget"));
	if (WidgetComp)
	{
		WidgetComp->SetupAttachment(OwnerRoot);
		WidgetComp->RegisterComponent();
		WidgetComp->SetRelativeLocation(PromptOffset);
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawSize(PromptDrawSize);
		WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
		WidgetComp->SetTwoSided(true);
		WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (PromptWidgetClass)
		{
			WidgetComp->SetWidgetClass(PromptWidgetClass);
		}
		WidgetComp->SetVisibility(false);
	}

	TryBindPlayer();
}

void UOryxInteractPromptComponent::TryBindPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<AOryxCharacter> It(World); It; ++It)
	{
		AOryxCharacter* Player = *It;
		if (!IsValid(Player)) continue;
		BoundPlayer = Player;
		Player->OnInteractableChanged.AddDynamic(this, &UOryxInteractPromptComponent::HandleInteractableChanged);
		return;
	}

	// No player yet — retry shortly. Common case is interactables BeginPlay before character spawn
	// when the level loads from a fresh class-select.
	World->GetTimerManager().SetTimer(BindRetryTimer, this,
		&UOryxInteractPromptComponent::TryBindPlayer, 0.5f, false);
}

void UOryxInteractPromptComponent::HandleInteractableChanged(AActor* NewInteractable)
{
	if (!WidgetComp) return;
	const bool bIAmIt = (NewInteractable == GetOwner());
	WidgetComp->SetVisibility(bIAmIt);
}

void UOryxInteractPromptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* Owner = GetOwner())
	{
		if (UOryxInteractableRegistry* Registry = UOryxInteractableRegistry::Get(this))
		{
			Registry->Unregister(Owner);
		}
	}

	if (AOryxCharacter* Player = BoundPlayer.Get())
	{
		Player->OnInteractableChanged.RemoveDynamic(this, &UOryxInteractPromptComponent::HandleInteractableChanged);
	}
	BoundPlayer.Reset();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BindRetryTimer);
	}

	Super::EndPlay(EndPlayReason);
}
