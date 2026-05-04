#include "OryxPlayerState.h"
#include "Component/Currency/OryxCurrencyComponent.h"

AOryxPlayerState::AOryxPlayerState()
{
	CurrencyComponent = CreateDefaultSubobject<UOryxCurrencyComponent>(TEXT("CurrencyComponent"));
}
