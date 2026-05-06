#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "OryxGameUserSettings.generated.h"

/**
 *  Project-wide user settings, persisted to GameUserSettings.ini.
 *  Registered in DefaultEngine.ini under [/Script/Engine.Engine] GameUserSettingsClassName.
 *
 *  Access from BP/C++ via GetOryxSettings(). Always call SaveSettings() after a setter
 *  if you want the change to survive a session.
 */
UCLASS(BlueprintType, Config = GameUserSettings)
class ORYX_API UOryxGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UOryxGameUserSettings();

	/** Convenience accessor — returns the active GameUserSettings cast to our subclass. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings", meta = (DisplayName = "Get Oryx Settings"))
	static UOryxGameUserSettings* GetOryxSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	bool GetShowFloatingDamageNumbers() const { return bShowFloatingDamageNumbers; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetShowFloatingDamageNumbers(bool bEnabled);

private:
	/** Master toggle for the floating damage number widget. Persists across sessions. */
	UPROPERTY(Config)
	bool bShowFloatingDamageNumbers = true;
};
