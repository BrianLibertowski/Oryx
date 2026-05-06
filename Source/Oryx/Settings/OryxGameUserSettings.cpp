#include "OryxGameUserSettings.h"
#include "Engine/Engine.h"

UOryxGameUserSettings::UOryxGameUserSettings()
{
}

UOryxGameUserSettings* UOryxGameUserSettings::GetOryxSettings()
{
	return Cast<UOryxGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UOryxGameUserSettings::SetShowFloatingDamageNumbers(bool bEnabled)
{
	if (bShowFloatingDamageNumbers == bEnabled) return;
	bShowFloatingDamageNumbers = bEnabled;
	SaveSettings();
}
