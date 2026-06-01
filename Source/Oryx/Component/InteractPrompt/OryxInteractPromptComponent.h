#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxInteractPromptComponent.generated.h"

class UWidgetComponent;
class UUserWidget;
class AOryxCharacter;

/**
 *  Drop-in interact prompt for anything implementing IOryxInteractable.
 *
 *  How it works:
 *    - Spawns a UWidgetComponent attached to its owner's root at PromptOffset.
 *    - WidgetComponent is in Screen space — always faces the camera, no rotation pop.
 *    - Binds to the local AOryxCharacter's OnInteractableChanged delegate.
 *    - Shows the widget when its owner becomes the player's currently-closest interactable;
 *      hides otherwise. Single source of truth = the player, so only ONE prompt shows at a time
 *      across all interactables in the level.
 *
 *  Usage:
 *    - Add as a CreateDefaultSubobject in the interactable actor's constructor:
 *        InteractPromptComponent = CreateDefaultSubobject<UOryxInteractPromptComponent>(TEXT("InteractPromptComponent"));
 *    - In BP, set PromptWidgetClass = WBP_InteractPrompt (your UMG widget) per Class Defaults.
 *    - Done — no per-BP graph wiring.
 *
 *  Co-op note: single-player demo binds to the first AOryxCharacter found. Co-op will need a
 *  per-player prompt component instance OR a per-player delegate signature. Defer to replication pass.
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxInteractPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxInteractPromptComponent();

	/** The UMG widget class shown above the interactable (e.g. WBP_InteractPrompt). Set per-BP. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Prompt")
	TSubclassOf<UUserWidget> PromptWidgetClass;

	/** Local offset from the owner's root where the prompt floats. Z up by default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Prompt")
	FVector PromptOffset = FVector(0.f, 0.f, 150.f);

	/** Render size of the screen-space widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Prompt")
	FVector2D PromptDrawSize = FVector2D(64.f, 64.f);

	UFUNCTION(BlueprintPure, Category = "Oryx|Prompt")
	UWidgetComponent* GetWidgetComponent() const { return WidgetComp; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Transient)
	UWidgetComponent* WidgetComp = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<AOryxCharacter> BoundPlayer;

	FTimerHandle BindRetryTimer;

	UFUNCTION()
	void HandleInteractableChanged(AActor* NewInteractable);

	/** Tries to bind to the first AOryxCharacter found. Retries on a 0.5s timer if no player yet. */
	void TryBindPlayer();
};
