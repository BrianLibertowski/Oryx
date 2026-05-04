#include "OryxEncounter_Camp.h"
#include "Components/SceneComponent.h"

AOryxEncounter_Camp::AOryxEncounter_Camp()
{
	// Simple scene root — camp doesn't need collision, it's just a "this is where the camp is" marker
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void AOryxEncounter_Camp::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStart)
	{
		// No specific instigating player — anyone can grab the reward pickup when the camp clears
		StartEncounter(nullptr);
	}
}
