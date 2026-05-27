#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "OryxGameUserSettings.generated.h"

/**
 *  Graphics quality preset. Wraps the engine's 5 scalability tiers + a Custom slot
 *  for when the player tweaks individual buckets through the Display tab.
 *
 *  ApplyQualityPreset() pipes a single enum into UGameUserSettings' SetViewDistanceQuality /
 *  SetShadowQuality / SetAntiAliasingQuality / SetPostProcessingQuality / SetTextureQuality /
 *  SetVisualEffectQuality / SetFoliageQuality / SetShadingQuality / SetGlobalIlluminationQuality /
 *  SetReflectionQuality so the user gets one-click preset switching.
 */
UENUM(BlueprintType)
enum class EOryxQualityPreset : uint8
{
	Low        UMETA(DisplayName = "Low"),
	Medium     UMETA(DisplayName = "Medium"),
	High       UMETA(DisplayName = "High"),
	Epic       UMETA(DisplayName = "Epic"),
	Cinematic  UMETA(DisplayName = "Cinematic"),
	Custom     UMETA(DisplayName = "Custom")
};

/**
 *  Colour-blind mode. Drives a post-process material parameter set on the player camera
 *  (wiring lives in BP_OryxCharacter / the post-process volume) — not the engine-level
 *  accessibility shaders since those are heavyweight and we want author control.
 */
UENUM(BlueprintType)
enum class EOryxColorBlindMode : uint8
{
	None            UMETA(DisplayName = "Off"),
	Protanopia      UMETA(DisplayName = "Protanopia (red-blind)"),
	Deuteranopia    UMETA(DisplayName = "Deuteranopia (green-blind)"),
	Tritanopia      UMETA(DisplayName = "Tritanopia (blue-blind)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOryxSettingsChanged);

/**
 *  Project-wide user settings, persisted to GameUserSettings.ini.
 *  Registered in DefaultEngine.ini under [/Script/Engine.Engine] GameUserSettingsClassName.
 *
 *  Three layers of settings live here:
 *
 *  1. ENGINE-PROVIDED — inherited from UGameUserSettings: resolution, fullscreen mode,
 *     VSync, frame rate cap, scalability buckets (View Distance / Shadows / Textures / etc),
 *     resolution scale. WBP_Settings's Display tab calls these directly via the
 *     "Get Game User Settings" BP node — no Oryx-side accessors needed.
 *
 *  2. ORYX GAME-SPECIFIC — declared below as Config UPROPERTYs. Anything that affects
 *     gameplay feel, HUD layout, or post-process tone that the engine doesn't expose:
 *     hit markers, screen shake, FOV, FX density, bloom/grain/CA, colour-blind mode,
 *     blood toggle (for D24 6+/E10+ rating compliance), subtitles, auto-pause.
 *
 *  3. CONVENIENCE — ApplyQualityPreset() + ResetToDefaults() helpers that batch-write
 *     engine + Oryx settings at once.
 *
 *  Listener pattern: anything that needs to react instantly to a settings flip
 *  (HUD widget, post-process volume, floating-damage spawner) binds to
 *  OnSettingsChanged. The delegate is parameterless — listeners re-read the specific
 *  setting they care about via GetOryxSettings()->Get*().
 *
 *  Setters all SaveSettings() and broadcast OnSettingsChanged. No need to manually save.
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

	/**
	 *  Broadcast after any Oryx-side setter writes + saves. Engine-provided settings (resolution
	 *  etc) DON'T trigger this — the engine fires its own ApplyResolutionSettings flow.
	 *  Multicast, parameterless — listeners re-read the value they care about.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Oryx|Settings")
	FOnOryxSettingsChanged OnSettingsChanged;

	// ====================================================================================
	//  Combat feedback
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	bool GetShowFloatingDamageNumbers() const { return bShowFloatingDamageNumbers; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetShowFloatingDamageNumbers(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	bool GetShowIncomingDamageNumbers() const { return bShowIncomingDamageNumbers; }

	/**
	 *  Sub-toggle to bShowFloatingDamageNumbers. False = floating numbers only show for damage
	 *  the player DEALS; True = also show numbers for damage the player TAKES (incoming).
	 *  Master toggle off → no numbers anywhere regardless of this setting.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetShowIncomingDamageNumbers(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	float GetDamageNumberScale() const { return DamageNumberScale; }

	/** 0.5..2.0. Multiplier on floating damage number widget Render Scale. Low-vision accessibility. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetDamageNumberScale(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	bool GetShowHitMarkers() const { return bShowHitMarkers; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetShowHitMarkers(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	bool GetShowEnemyHealthBars() const { return bShowEnemyHealthBars; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetShowEnemyHealthBars(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	float GetScreenShakeIntensity() const { return ScreenShakeIntensity; }

	/** 0..1. Multiplier on screen-shake amplitude (camera shakes for hits, explosions). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetScreenShakeIntensity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Combat")
	float GetCameraShakeIntensity() const { return CameraShakeIntensity; }

	/** 0..1. Multiplier on cinematic camera-shake assets (boss telegraph, big VFX). Separate from screen shake. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Combat")
	void SetCameraShakeIntensity(float Value);

	// ====================================================================================
	//  Visual FX (post-process + particles)
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|VFX")
	float GetFXDensity() const { return FXDensity; }

	/** 0..1. Multiplier on non-essential VFX (impact sparks, ambient particles). 0 = essential only. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|VFX")
	void SetFXDensity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|VFX")
	bool GetMotionBlur() const { return bMotionBlur; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|VFX")
	void SetMotionBlur(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|VFX")
	bool GetFilmGrain() const { return bFilmGrain; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|VFX")
	void SetFilmGrain(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|VFX")
	bool GetChromaticAberration() const { return bChromaticAberration; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|VFX")
	void SetChromaticAberration(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|VFX")
	bool GetBloom() const { return bBloom; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|VFX")
	void SetBloom(bool bEnabled);

	// ====================================================================================
	//  UI / HUD
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|UI")
	float GetHUDOpacity() const { return HUDOpacity; }

	/** 0.3..1.0. Multiplier on HUD root widget RenderOpacity. Don't go below 0.3 (unreadable). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|UI")
	void SetHUDOpacity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|UI")
	float GetUIScale() const { return UIScale; }

	/** 0.8..1.5. Multiplier on root UMG render scale (accessibility for ultrawide / 4K). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|UI")
	void SetUIScale(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|UI")
	bool GetShowAbilityCooldownText() const { return bShowAbilityCooldownText; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|UI")
	void SetShowAbilityCooldownText(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|UI")
	bool GetMinimalHUD() const { return bMinimalHUD; }

	/**
	 *  When true, BP hides non-essential HUD elements (XP bar, gold counter, stamina bar,
	 *  ability cooldown text, status icons). Health bar + active ability icons stay.
	 *  Streamers / immersion-mode players.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|UI")
	void SetMinimalHUD(bool bEnabled);

	// ====================================================================================
	//  Camera
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Camera")
	float GetFieldOfView() const { return FieldOfView; }

	/** 60..110. Pumped into FollowCamera->SetFieldOfView() by BP_OryxCharacter on settings change. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Camera")
	void SetFieldOfView(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Camera")
	float GetCameraDistance() const { return CameraDistance; }

	/** 300..600 cm. Third-person camera arm length. BP applies to SpringArm->TargetArmLength on settings change. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Camera")
	void SetCameraDistance(float Value);

	// ====================================================================================
	//  Accessibility
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	EOryxColorBlindMode GetColorBlindMode() const { return ColorBlindMode; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetColorBlindMode(EOryxColorBlindMode Mode);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	bool GetShowSubtitles() const { return bShowSubtitles; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetShowSubtitles(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	bool GetAutoPause() const { return bAutoPause; }

	/** Pause the game when the window loses focus (alt-tab / desktop click). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetAutoPause(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	float GetSubtitleSize() const { return SubtitleSize; }

	/** 0.7..1.5. Multiplier on subtitle text widget Render Scale. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetSubtitleSize(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	float GetSubtitleOpacity() const { return SubtitleOpacity; }

	/** 0.5..1.0. Background plate opacity behind subtitle text. Accessibility for cluttered scenes. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetSubtitleOpacity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Accessibility")
	bool GetReduceMotion() const { return bReduceMotion; }

	/**
	 *  Accessibility umbrella toggle. Consumers AND-gate motion-heavy effects against this:
	 *    - Screen shake: clamp ScreenShakeIntensity to 0 when bReduceMotion
	 *    - Camera shake: clamp CameraShakeIntensity to 0 when bReduceMotion
	 *    - Motion blur: force off when bReduceMotion (regardless of bMotionBlur)
	 *    - Damage direction indicator: pulse animation static when bReduceMotion
	 *    - Boss telegraph: skip the camera dolly-in animation when bReduceMotion
	 *  BP-side discipline — this setting doesn't auto-modify other settings, consumers check it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Accessibility")
	void SetReduceMotion(bool bEnabled);

	// ====================================================================================
	//  Audio — slider volumes (0..1). Each consumer routes to a USoundClass via BP wiring
	//  (typically WBP_Settings binds them to SC_Master / SC_Music / SC_SFX / etc Sound Classes).
	//  Scaffold-only — Sound Class assets ship in P5/P6 when audio handoff lands.
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetMasterVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetMusicVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetSFXVolume() const { return SFXVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetSFXVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetUIVolume() const { return UIVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetUIVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetAmbientVolume() const { return AmbientVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetAmbientVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	float GetDialogueVolume() const { return DialogueVolume; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetDialogueVolume(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Audio")
	bool GetMuteOnFocusLoss() const { return bMuteOnFocusLoss; }

	/** Mute all audio when the window loses focus (alt-tab / desktop). Standard UX default. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Audio")
	void SetMuteOnFocusLoss(bool bEnabled);

	// ====================================================================================
	//  Input — mouse / controller sensitivity + invert + rumble. Routes via Enhanced Input
	//  per-mapping context scalars set on Input Modifiers at runtime by BP_OryxCharacter.
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Input")
	float GetMouseSensitivity() const { return MouseSensitivity; }

	/** 0.1..3.0. Multiplier applied to the mouse-look Input Modifier in IMC_MouseLook. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Input")
	void SetMouseSensitivity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Input")
	bool GetInvertMouseY() const { return bInvertMouseY; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Input")
	void SetInvertMouseY(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Input")
	float GetControllerSensitivity() const { return ControllerSensitivity; }

	/** 0.1..3.0. Multiplier applied to the gamepad-look Input Modifier in IMC_Default. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Input")
	void SetControllerSensitivity(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Input")
	bool GetInvertControllerY() const { return bInvertControllerY; }

	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Input")
	void SetInvertControllerY(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Input")
	float GetRumbleIntensity() const { return RumbleIntensity; }

	/** 0..1. Multiplier on force feedback effects for gamepad. 0 = no rumble. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Input")
	void SetRumbleIntensity(float Value);

	// ====================================================================================
	//  Control feel
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Control")
	bool GetToggleSprint() const { return bToggleSprint; }

	/** True = press once to start sprint, press again to stop. False (default) = hold to sprint. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Control")
	void SetToggleSprint(bool bEnabled);

	// ====================================================================================
	//  Visual readability — outlines and damage source indicator
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Readability")
	bool GetShowEnemyOutlines() const { return bShowEnemyOutlines; }

	/** Outline pass on enemies (post-process material wired to mesh stencil bit). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Readability")
	void SetShowEnemyOutlines(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Readability")
	bool GetShowInteractableHighlight() const { return bShowInteractableHighlight; }

	/** Glow/outline on vendors / chests / totems / outpost flags when player is in range. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Readability")
	void SetShowInteractableHighlight(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Readability")
	bool GetShowDamageDirectionIndicator() const { return bShowDamageDirectionIndicator; }

	/** HUD arrow pointing at the source of the most recent damage. Combat awareness aid. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Readability")
	void SetShowDamageDirectionIndicator(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Readability")
	bool GetShowDoTIcons() const { return bShowDoTIcons; }

	/** Show Burn / Poison / Chill icons next to the health bar when the player has an active DoT. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Readability")
	void SetShowDoTIcons(bool bEnabled);

	// ====================================================================================
	//  Display — brightness + contrast + saturation. Three-knob post-process exposure trio.
	//  Critical for biome 2 (dark cave) + colour-blind / low-vision accessibility.
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Display")
	float GetBrightness() const { return Brightness; }

	/** 0.5..1.5. Routes to PP_GlobalExposure material parameter set in the world post-process volume. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Display")
	void SetBrightness(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Display")
	float GetContrastBoost() const { return ContrastBoost; }

	/** 0.5..1.5. Routes to PP volume Color Grading → Global → Contrast (1.0 = neutral). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Display")
	void SetContrastBoost(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Display")
	float GetSaturation() const { return Saturation; }

	/** 0.5..1.5. Routes to PP volume Color Grading → Global → Saturation (1.0 = neutral). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Display")
	void SetSaturation(float Value);

	// ====================================================================================
	//  Tutorial / onboarding gate
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Tutorial")
	bool GetTutorialHintsEnabled() const { return bTutorialHintsEnabled; }

	/** Gate for the OnboardingHints.md hint system. Players can disable hints after they've learned. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Tutorial")
	void SetTutorialHintsEnabled(bool bEnabled);

	// ====================================================================================
	//  Save — D16 auto-save interval (RunSave). 0 = manual-only.
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Save")
	int32 GetAutoSaveIntervalSeconds() const { return AutoSaveIntervalSeconds; }

	/**
	 *  60..600. Auto-save period for the mid-run RunSave (D16 default = 300 / 5min).
	 *  Set 0 to disable auto-save (manual save in pause menu only).
	 *  Currently unused — AOryxRunManager will read this when the mid-run snapshot work lands.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Save")
	void SetAutoSaveIntervalSeconds(int32 Value);

	// ====================================================================================
	//  Debug — playtester-facing overlays. Hidden in shipping config later.
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Debug")
	bool GetShowFPSCounter() const { return bShowFPSCounter; }

	/** FPS overlay in HUD corner. Useful for playtesters reporting perf issues. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Debug")
	void SetShowFPSCounter(bool bEnabled);

	// ====================================================================================
	//  Content (rating compliance)
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Content")
	bool GetShowBlood() const { return bShowBlood; }

	/**
	 *  D24: 6+/E10+ rating target. Defaults FALSE. If/when a higher rating tier is offered,
	 *  this can be flipped on and BP_OryxEnemy will route to blood VFX instead of hit-marker sparks.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Content")
	void SetShowBlood(bool bEnabled);

	// ====================================================================================
	//  Quality presets — convenience batch-set
	// ====================================================================================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oryx|Settings|Quality")
	EOryxQualityPreset GetActiveQualityPreset() const { return ActiveQualityPreset; }

	/**
	 *  Apply a quality tier. Maps the preset enum to engine scalability buckets (View Distance,
	 *  Shadows, AA, Post Process, Textures, Effects, Foliage, Shading, GI, Reflections) and
	 *  writes ResolutionScale in one call. Triggers ApplyNonResolutionSettings + SaveSettings.
	 *  Custom = no-op (player has manually tweaked individual buckets).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Quality")
	void ApplyQualityPreset(EOryxQualityPreset Preset);

	/**
	 *  Reset ALL Oryx-side settings to their declared defaults. Engine-side settings are
	 *  untouched — call UGameUserSettings::SetToDefaults() separately for those.
	 *  Broadcasts OnSettingsChanged once at the end (not per-field).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Settings|Quality")
	void ResetOryxDefaults();

private:
	// --- Combat feedback ---

	UPROPERTY(Config)
	bool bShowFloatingDamageNumbers = true;

	UPROPERTY(Config)
	bool bShowIncomingDamageNumbers = true;

	UPROPERTY(Config)
	float DamageNumberScale = 1.0f;

	UPROPERTY(Config)
	bool bShowHitMarkers = true;

	UPROPERTY(Config)
	bool bShowEnemyHealthBars = true;

	UPROPERTY(Config)
	float ScreenShakeIntensity = 1.0f;

	UPROPERTY(Config)
	float CameraShakeIntensity = 1.0f;

	// --- Visual FX ---

	UPROPERTY(Config)
	float FXDensity = 1.0f;

	UPROPERTY(Config)
	bool bMotionBlur = true;

	UPROPERTY(Config)
	bool bFilmGrain = false;

	UPROPERTY(Config)
	bool bChromaticAberration = false;

	UPROPERTY(Config)
	bool bBloom = true;

	// --- UI / HUD ---

	UPROPERTY(Config)
	float HUDOpacity = 1.0f;

	UPROPERTY(Config)
	float UIScale = 1.0f;

	UPROPERTY(Config)
	bool bShowAbilityCooldownText = true;

	UPROPERTY(Config)
	bool bMinimalHUD = false;

	// --- Camera ---

	UPROPERTY(Config)
	float FieldOfView = 90.0f;

	UPROPERTY(Config)
	float CameraDistance = 400.0f;

	// --- Accessibility ---

	UPROPERTY(Config)
	EOryxColorBlindMode ColorBlindMode = EOryxColorBlindMode::None;

	UPROPERTY(Config)
	bool bShowSubtitles = false;

	UPROPERTY(Config)
	float SubtitleSize = 1.0f;

	UPROPERTY(Config)
	float SubtitleOpacity = 0.9f;

	UPROPERTY(Config)
	bool bReduceMotion = false;

	UPROPERTY(Config)
	bool bAutoPause = true;

	// --- Audio ---

	UPROPERTY(Config)
	float MasterVolume = 1.0f;

	UPROPERTY(Config)
	float MusicVolume = 1.0f;

	UPROPERTY(Config)
	float SFXVolume = 1.0f;

	UPROPERTY(Config)
	float UIVolume = 1.0f;

	UPROPERTY(Config)
	float AmbientVolume = 1.0f;

	UPROPERTY(Config)
	float DialogueVolume = 1.0f;

	UPROPERTY(Config)
	bool bMuteOnFocusLoss = true;

	// --- Input ---

	UPROPERTY(Config)
	float MouseSensitivity = 1.0f;

	UPROPERTY(Config)
	bool bInvertMouseY = false;

	UPROPERTY(Config)
	float ControllerSensitivity = 1.0f;

	UPROPERTY(Config)
	bool bInvertControllerY = false;

	UPROPERTY(Config)
	float RumbleIntensity = 1.0f;

	// --- Control feel ---

	UPROPERTY(Config)
	bool bToggleSprint = false;

	// --- Visual readability ---

	UPROPERTY(Config)
	bool bShowEnemyOutlines = true;

	UPROPERTY(Config)
	bool bShowInteractableHighlight = true;

	UPROPERTY(Config)
	bool bShowDamageDirectionIndicator = true;

	UPROPERTY(Config)
	bool bShowDoTIcons = true;

	// --- Display (Brightness / Contrast / Saturation) ---

	UPROPERTY(Config)
	float Brightness = 1.0f;

	UPROPERTY(Config)
	float ContrastBoost = 1.0f;

	UPROPERTY(Config)
	float Saturation = 1.0f;

	// --- Tutorial ---

	UPROPERTY(Config)
	bool bTutorialHintsEnabled = true;

	// --- Save (D16 auto-save) ---

	UPROPERTY(Config)
	int32 AutoSaveIntervalSeconds = 300;

	// --- Debug ---

	UPROPERTY(Config)
	bool bShowFPSCounter = false;

	// --- Content ---

	UPROPERTY(Config)
	bool bShowBlood = false;

	// --- Quality preset (cached so UI can show the active tier) ---

	UPROPERTY(Config)
	EOryxQualityPreset ActiveQualityPreset = EOryxQualityPreset::High;

	// --- Internal helpers ---

	/** Single-line broadcast + save shortcut for the typical setter shape. */
	void NotifyAndSave();
};
