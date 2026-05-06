#include "OryxPlayerState.h"
#include "Component/Currency/OryxCurrencyComponent.h"
#include "Component/Level/OryxLevelComponent.h"

AOryxPlayerState::AOryxPlayerState()
{
	CurrencyComponent = CreateDefaultSubobject<UOryxCurrencyComponent>(TEXT("CurrencyComponent"));
	LevelComponent = CreateDefaultSubobject<UOryxLevelComponent>(TEXT("LevelComponent"));
}

void AOryxPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// D13: each level-up grants Onyx. LevelComponent broadcasts the amount; we route to currency.
	if (LevelComponent)
	{
		LevelComponent->OnLevelUp.AddDynamic(this, &AOryxPlayerState::HandleLevelUp);
	}
}

void AOryxPlayerState::HandleLevelUp(int32 /*NewLevel*/, int32 OnyxAwarded)
{
	if (CurrencyComponent && OnyxAwarded > 0)
	{
		CurrencyComponent->AddOnyx(OnyxAwarded);
	}
}
