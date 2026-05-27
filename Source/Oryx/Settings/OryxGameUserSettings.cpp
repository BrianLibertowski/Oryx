#include "OryxGameUserSettings.h"
#include "Engine/Engine.h"

UOryxGameUserSettings::UOryxGameUserSettings()
{
}

UOryxGameUserSettings* UOryxGameUserSettings::GetOryxSettings()
{
	return Cast<UOryxGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UOryxGameUserSettings::NotifyAndSave()
{
	SaveSettings();
	OnSettingsChanged.Broadcast();
}

// ====================================================================================
//  Combat feedback
// ====================================================================================

void UOryxGameUserSettings::SetShowFloatingDamageNumbers(bool bEnabled)
{
	if (bShowFloatingDamageNumbers == bEnabled) return;
	bShowFloatingDamageNumbers = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowIncomingDamageNumbers(bool bEnabled)
{
	if (bShowIncomingDamageNumbers == bEnabled) return;
	bShowIncomingDamageNumbers = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetDamageNumberScale(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.5f, 2.0f);
	if (FMath::IsNearlyEqual(DamageNumberScale, Clamped)) return;
	DamageNumberScale = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowHitMarkers(bool bEnabled)
{
	if (bShowHitMarkers == bEnabled) return;
	bShowHitMarkers = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowEnemyHealthBars(bool bEnabled)
{
	if (bShowEnemyHealthBars == bEnabled) return;
	bShowEnemyHealthBars = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetScreenShakeIntensity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(ScreenShakeIntensity, Clamped)) return;
	ScreenShakeIntensity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetCameraShakeIntensity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(CameraShakeIntensity, Clamped)) return;
	CameraShakeIntensity = Clamped;
	NotifyAndSave();
}

// ====================================================================================
//  Visual FX
// ====================================================================================

void UOryxGameUserSettings::SetFXDensity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(FXDensity, Clamped)) return;
	FXDensity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetMotionBlur(bool bEnabled)
{
	if (bMotionBlur == bEnabled) return;
	bMotionBlur = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetFilmGrain(bool bEnabled)
{
	if (bFilmGrain == bEnabled) return;
	bFilmGrain = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetChromaticAberration(bool bEnabled)
{
	if (bChromaticAberration == bEnabled) return;
	bChromaticAberration = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetBloom(bool bEnabled)
{
	if (bBloom == bEnabled) return;
	bBloom = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  UI / HUD
// ====================================================================================

void UOryxGameUserSettings::SetHUDOpacity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.3f, 1.0f);
	if (FMath::IsNearlyEqual(HUDOpacity, Clamped)) return;
	HUDOpacity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetUIScale(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.8f, 1.5f);
	if (FMath::IsNearlyEqual(UIScale, Clamped)) return;
	UIScale = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowAbilityCooldownText(bool bEnabled)
{
	if (bShowAbilityCooldownText == bEnabled) return;
	bShowAbilityCooldownText = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetMinimalHUD(bool bEnabled)
{
	if (bMinimalHUD == bEnabled) return;
	bMinimalHUD = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Camera
// ====================================================================================

void UOryxGameUserSettings::SetFieldOfView(float Value)
{
	const float Clamped = FMath::Clamp(Value, 60.0f, 110.0f);
	if (FMath::IsNearlyEqual(FieldOfView, Clamped)) return;
	FieldOfView = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetCameraDistance(float Value)
{
	const float Clamped = FMath::Clamp(Value, 300.0f, 600.0f);
	if (FMath::IsNearlyEqual(CameraDistance, Clamped)) return;
	CameraDistance = Clamped;
	NotifyAndSave();
}

// ====================================================================================
//  Accessibility
// ====================================================================================

void UOryxGameUserSettings::SetColorBlindMode(EOryxColorBlindMode Mode)
{
	if (ColorBlindMode == Mode) return;
	ColorBlindMode = Mode;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowSubtitles(bool bEnabled)
{
	if (bShowSubtitles == bEnabled) return;
	bShowSubtitles = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetAutoPause(bool bEnabled)
{
	if (bAutoPause == bEnabled) return;
	bAutoPause = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetSubtitleSize(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.7f, 1.5f);
	if (FMath::IsNearlyEqual(SubtitleSize, Clamped)) return;
	SubtitleSize = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetSubtitleOpacity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.5f, 1.0f);
	if (FMath::IsNearlyEqual(SubtitleOpacity, Clamped)) return;
	SubtitleOpacity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetReduceMotion(bool bEnabled)
{
	if (bReduceMotion == bEnabled) return;
	bReduceMotion = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Audio
// ====================================================================================

void UOryxGameUserSettings::SetMasterVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(MasterVolume, Clamped)) return;
	MasterVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetMusicVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(MusicVolume, Clamped)) return;
	MusicVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetSFXVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(SFXVolume, Clamped)) return;
	SFXVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetUIVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(UIVolume, Clamped)) return;
	UIVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetAmbientVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(AmbientVolume, Clamped)) return;
	AmbientVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetDialogueVolume(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(DialogueVolume, Clamped)) return;
	DialogueVolume = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetMuteOnFocusLoss(bool bEnabled)
{
	if (bMuteOnFocusLoss == bEnabled) return;
	bMuteOnFocusLoss = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Input
// ====================================================================================

void UOryxGameUserSettings::SetMouseSensitivity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.1f, 3.0f);
	if (FMath::IsNearlyEqual(MouseSensitivity, Clamped)) return;
	MouseSensitivity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetInvertMouseY(bool bEnabled)
{
	if (bInvertMouseY == bEnabled) return;
	bInvertMouseY = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetControllerSensitivity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.1f, 3.0f);
	if (FMath::IsNearlyEqual(ControllerSensitivity, Clamped)) return;
	ControllerSensitivity = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetInvertControllerY(bool bEnabled)
{
	if (bInvertControllerY == bEnabled) return;
	bInvertControllerY = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetRumbleIntensity(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(RumbleIntensity, Clamped)) return;
	RumbleIntensity = Clamped;
	NotifyAndSave();
}

// ====================================================================================
//  Control feel
// ====================================================================================

void UOryxGameUserSettings::SetToggleSprint(bool bEnabled)
{
	if (bToggleSprint == bEnabled) return;
	bToggleSprint = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Visual readability
// ====================================================================================

void UOryxGameUserSettings::SetShowEnemyOutlines(bool bEnabled)
{
	if (bShowEnemyOutlines == bEnabled) return;
	bShowEnemyOutlines = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowInteractableHighlight(bool bEnabled)
{
	if (bShowInteractableHighlight == bEnabled) return;
	bShowInteractableHighlight = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowDamageDirectionIndicator(bool bEnabled)
{
	if (bShowDamageDirectionIndicator == bEnabled) return;
	bShowDamageDirectionIndicator = bEnabled;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetShowDoTIcons(bool bEnabled)
{
	if (bShowDoTIcons == bEnabled) return;
	bShowDoTIcons = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Brightness
// ====================================================================================

void UOryxGameUserSettings::SetBrightness(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.5f, 1.5f);
	if (FMath::IsNearlyEqual(Brightness, Clamped)) return;
	Brightness = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetContrastBoost(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.5f, 1.5f);
	if (FMath::IsNearlyEqual(ContrastBoost, Clamped)) return;
	ContrastBoost = Clamped;
	NotifyAndSave();
}

void UOryxGameUserSettings::SetSaturation(float Value)
{
	const float Clamped = FMath::Clamp(Value, 0.5f, 1.5f);
	if (FMath::IsNearlyEqual(Saturation, Clamped)) return;
	Saturation = Clamped;
	NotifyAndSave();
}

// ====================================================================================
//  Tutorial
// ====================================================================================

void UOryxGameUserSettings::SetTutorialHintsEnabled(bool bEnabled)
{
	if (bTutorialHintsEnabled == bEnabled) return;
	bTutorialHintsEnabled = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Save
// ====================================================================================

void UOryxGameUserSettings::SetAutoSaveIntervalSeconds(int32 Value)
{
	// 0 = disabled (manual save only). Otherwise clamp to [60, 600].
	int32 Clamped;
	if (Value <= 0)
	{
		Clamped = 0;
	}
	else
	{
		Clamped = FMath::Clamp(Value, 60, 600);
	}
	if (AutoSaveIntervalSeconds == Clamped) return;
	AutoSaveIntervalSeconds = Clamped;
	NotifyAndSave();
}

// ====================================================================================
//  Debug
// ====================================================================================

void UOryxGameUserSettings::SetShowFPSCounter(bool bEnabled)
{
	if (bShowFPSCounter == bEnabled) return;
	bShowFPSCounter = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Content
// ====================================================================================

void UOryxGameUserSettings::SetShowBlood(bool bEnabled)
{
	if (bShowBlood == bEnabled) return;
	bShowBlood = bEnabled;
	NotifyAndSave();
}

// ====================================================================================
//  Quality presets
// ====================================================================================

void UOryxGameUserSettings::ApplyQualityPreset(EOryxQualityPreset Preset)
{
	ActiveQualityPreset = Preset;

	if (Preset == EOryxQualityPreset::Custom)
	{
		// Custom = user has tweaked individual buckets. Don't touch engine settings,
		// just remember the tier so the UI can show it.
		NotifyAndSave();
		return;
	}

	// Engine scalability levels: 0=Low, 1=Medium, 2=High, 3=Epic, 4=Cinematic.
	int32 EngineLevel = 2;
	float ResolutionScale = 100.0f;

	switch (Preset)
	{
	case EOryxQualityPreset::Low:
		EngineLevel = 0;
		ResolutionScale = 75.0f;
		break;
	case EOryxQualityPreset::Medium:
		EngineLevel = 1;
		ResolutionScale = 85.0f;
		break;
	case EOryxQualityPreset::High:
		EngineLevel = 2;
		ResolutionScale = 100.0f;
		break;
	case EOryxQualityPreset::Epic:
		EngineLevel = 3;
		ResolutionScale = 100.0f;
		break;
	case EOryxQualityPreset::Cinematic:
		EngineLevel = 4;
		ResolutionScale = 100.0f;
		break;
	default:
		break;
	}

	// Pump the same level into every scalability bucket. Players can override individual
	// buckets later; doing so flips the preset to Custom (caller's responsibility).
	SetViewDistanceQuality(EngineLevel);
	SetShadowQuality(EngineLevel);
	SetAntiAliasingQuality(EngineLevel);
	SetPostProcessingQuality(EngineLevel);
	SetTextureQuality(EngineLevel);
	SetVisualEffectQuality(EngineLevel);
	SetFoliageQuality(EngineLevel);
	SetShadingQuality(EngineLevel);
	SetGlobalIlluminationQuality(EngineLevel);
	SetReflectionQuality(EngineLevel);

	SetResolutionScaleValueEx(ResolutionScale);

	// Engine side
	ApplyNonResolutionSettings();
	SaveSettings();

	// Oryx side
	OnSettingsChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("OryxGameUserSettings::ApplyQualityPreset → tier=%d, scale=%.0f%%"),
		EngineLevel, ResolutionScale);
}

void UOryxGameUserSettings::ResetOryxDefaults()
{
	// Match the in-header defaults. Any future field added must be reset here too.

	// Combat feedback
	bShowFloatingDamageNumbers = true;
	bShowIncomingDamageNumbers = true;
	DamageNumberScale = 1.0f;
	bShowHitMarkers = true;
	bShowEnemyHealthBars = true;
	ScreenShakeIntensity = 1.0f;
	CameraShakeIntensity = 1.0f;

	// Visual FX
	FXDensity = 1.0f;
	bMotionBlur = true;
	bFilmGrain = false;
	bChromaticAberration = false;
	bBloom = true;

	// UI / HUD
	HUDOpacity = 1.0f;
	UIScale = 1.0f;
	bShowAbilityCooldownText = true;
	bMinimalHUD = false;

	// Camera
	FieldOfView = 90.0f;
	CameraDistance = 400.0f;

	// Accessibility
	ColorBlindMode = EOryxColorBlindMode::None;
	bShowSubtitles = false;
	SubtitleSize = 1.0f;
	SubtitleOpacity = 0.9f;
	bReduceMotion = false;
	bAutoPause = true;

	// Content
	bShowBlood = false;

	// Audio
	MasterVolume = 1.0f;
	MusicVolume = 1.0f;
	SFXVolume = 1.0f;
	UIVolume = 1.0f;
	AmbientVolume = 1.0f;
	DialogueVolume = 1.0f;
	bMuteOnFocusLoss = true;

	// Input
	MouseSensitivity = 1.0f;
	bInvertMouseY = false;
	ControllerSensitivity = 1.0f;
	bInvertControllerY = false;
	RumbleIntensity = 1.0f;

	// Control feel
	bToggleSprint = false;

	// Visual readability
	bShowEnemyOutlines = true;
	bShowInteractableHighlight = true;
	bShowDamageDirectionIndicator = true;
	bShowDoTIcons = true;

	// Display (Brightness / Contrast / Saturation)
	Brightness = 1.0f;
	ContrastBoost = 1.0f;
	Saturation = 1.0f;

	// Tutorial
	bTutorialHintsEnabled = true;

	// Save
	AutoSaveIntervalSeconds = 300;

	// Debug
	bShowFPSCounter = false;

	// Quality preset
	ActiveQualityPreset = EOryxQualityPreset::High;

	NotifyAndSave();
}
