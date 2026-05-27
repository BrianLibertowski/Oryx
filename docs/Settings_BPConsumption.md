# Settings — BP Consumption Reference

Reference for wiring `WBP_Settings` (and any in-game widget that respects settings) to `UOryxGameUserSettings`. Written 2026-05-26 after the C++ scaffold expansion.

## Two layers, one consumer

Every settings UI page mixes two backends:

| Backend | What's there | How to access |
|---|---|---|
| `UGameUserSettings` (engine) | Resolution, fullscreen mode, VSync, frame rate cap, all scalability buckets, resolution scale | `Get Game User Settings` BP node |
| `UOryxGameUserSettings` (Oryx subclass) | Combat feedback toggles, screen shake, FOV, FX density, post-process toggles, colour-blind mode, HUD opacity, UI scale, accessibility | `Get Oryx Settings` BP node (static, returns cast subclass) |

WBP_Settings tabs map to:
- **Display tab** = engine-side (resolution + window mode + VSync + frame cap) + Oryx quality preset (5 tier buttons) + Oryx post-process toggles (bloom / motion blur / film grain / CA / FX density) + brightness slider
- **Audio tab** = Oryx-side (Master / Music / SFX / UI / Ambient / Dialogue volume sliders + MuteOnFocusLoss). Each slider drives a USoundClass volume when audio lands in P5/P6 — for now BP just stores the values.
- **Gameplay tab** = Oryx-side (floating damage numbers / hit markers / enemy HP bars / FOV / camera shake / screen shake / DoT icons / damage direction indicator / blood toggle / toggle sprint / tutorial hints)
- **UI tab** = Oryx-side (HUD opacity / UI scale / ability cooldown text / enemy outlines / interactable highlight / colour-blind mode / subtitles / auto-pause)
- **Controls tab** = Oryx-side (mouse sensitivity / invert mouse Y / controller sensitivity / invert controller Y / rumble intensity). Enhanced Input key-remapping is out of scope for demo.
- **Debug tab** (playtester builds only — hide in shipping) = bShowFPSCounter toggle. Conditionally render this tab only when build configuration is Development.

## BP wire-up pattern (per setting)

### Toggle (bool)

**Construct:**
```
Get Oryx Settings → Get<X> → Set Is Checked (on CheckBox)
```

**OnCheckStateChanged:**
```
Get Oryx Settings → Set<X>(Is Checked)
```

The setter handles SaveSettings + broadcasts internally. No manual save call needed.

### Slider (float)

**Construct:**
```
Get Oryx Settings → Get<X> → Set Value (on Slider)
```

**OnValueChanged:** (no commit)
```
Get Oryx Settings → Set<X>(Value)
```

(Setter clamps to the field's documented range — e.g. HUDOpacity to [0.3, 1.0]. UI doesn't need its own clamp.)

### Enum dropdown (combo box / cycle)

**Construct:**
```
Get Oryx Settings → GetColorBlindMode → SetSelectedOption (on ComboBox)
```

**OnSelectionChanged:**
```
Map string back to enum → Get Oryx Settings → SetColorBlindMode(enum)
```

### Quality preset (button group)

5 buttons (Low / Medium / High / Epic / Cinematic). Each OnClicked:
```
Get Oryx Settings → ApplyQualityPreset(<enum>)
```

Custom is read-only — it's the auto-set state when the player tweaks an individual bucket on the Display tab. UI shows "Custom" when `GetActiveQualityPreset == Custom`.

### Reset to defaults button

```
Get Oryx Settings → ResetOryxDefaults
Get Game User Settings → Set To Defaults → Apply Settings   (engine side, optional)
```

## Live consumers — bind to OnSettingsChanged

Anything that should react at runtime (no PIE restart) binds the multicast delegate. Pattern:

**On Construct** (e.g. WBP_PlayerHUD):
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (HandleSettingsChanged)
HandleSettingsChanged: re-read whichever settings this widget cares about + refresh visuals
```

Examples of live consumers:
- **WBP_PlayerHUD** — bind to update HUD opacity (`HUDOpacity`), UI scale (`UIScale`), ability cooldown text visibility
- **BP_OryxCharacter** — bind to apply FOV change to `FollowCamera` (call `SetFieldOfView(Get Oryx Settings → GetFieldOfView)`)
- **BP_FloatingDamageActor / BP_OryxCharacter HandleDamageReceived** — already checks `GetShowFloatingDamageNumbers` on each spawn; no delegate needed (per-event read)
- **Post-process volume** in Blockout — bind to flip Bloom/Motion Blur/Film Grain/Chromatic Aberration material params
- **WBP_EnemyHealthBar** — bind to set Visibility based on `GetShowEnemyHealthBars`

## Specific consumer wire-ups (for when these widgets get built)

### FOV consumer (BP_OryxCharacter)

In Event BeginPlay (after Super BeginPlay):
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyFOV)
ApplyFOV: FollowCamera → SetFieldOfView(Get Oryx Settings → GetFieldOfView)
Also call ApplyFOV once on BeginPlay so initial FOV matches the saved setting.
```

### HUD opacity / UI scale (WBP_PlayerHUD)

In Event Construct:
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (RefreshFromSettings)
RefreshFromSettings:
  Self → Set Render Opacity (Get Oryx Settings → GetHUDOpacity)
  Self → Set Render Scale (Vector2D(Get Oryx Settings → GetUIScale))
Also call RefreshFromSettings once on Construct.
```

### Screen shake intensity consumer

Camera shake invocation sites multiply their Scale parameter by `Get Oryx Settings → GetScreenShakeIntensity`:
```
Play Camera Shake (Class=CS_HitImpact, Scale = 1.0 × GetScreenShakeIntensity)
```
For boss telegraph / cinematic shakes, use `GetCameraShakeIntensity` instead.

### Show Blood (D24 future-proofing)

BP_OryxEnemy.HandleDamageReceived branches:
```
Branch on Get Oryx Settings → GetShowBlood
  True  → Spawn NS_Blood at HitLocation
  False → Spawn NS_HitSpark at HitLocation (current default)
```

### Audio sliders (when audio lands)

In WBP_PlayerHUD Event Construct (or a global UOryxAudioSubsystem):
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyAudioVolumes)
ApplyAudioVolumes:
  Set Sound Mix Class Override (SC_Master, MasterVolume, 1.0, 0.0, true)
  Set Sound Mix Class Override (SC_Music, MusicVolume, 1.0, 0.0, true)
  Set Sound Mix Class Override (SC_SFX, SFXVolume, 1.0, 0.0, true)
  ...etc per class
  Push Sound Mix Modifier (Mix_Master)
```
Also call ApplyAudioVolumes once on Construct so initial volumes match saved settings.

### Mouse / controller sensitivity

In BP_OryxCharacter Event BeginPlay (after Super):
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyInputScalars)
ApplyInputScalars:
  Get Enhanced Input Local Player Subsystem → Add Input Modifier (or set scalar on existing Modifier)
  Apply MouseSensitivity to IA_MouseLook's Scalar modifier
  Apply ControllerSensitivity to IA_Look's Scalar modifier
  Apply Invert Y by negating the Y axis modifier
```
The "right" implementation uses `UInputModifierScalar` on each look mapping — set the scalar value via the modifier's `Scalar.X = sensitivity, Scalar.Y = (invertY ? -sensitivity : sensitivity)` pattern. Cleaner than rebuilding mapping contexts.

### Toggle Sprint

In BP_OryxCharacter sprint input handler:
```
On IA_Sprint Started:
  Branch on Get Oryx Settings → GetToggleSprint
    True  → If bIsSprinting then DoSprintEnd, else DoSprintStart   (toggle pattern)
    False → DoSprintStart  (hold pattern; Completed still calls DoSprintEnd)
```

### Brightness (post-process exposure)

In Blockout level BP (or a global PP volume) Event BeginPlay:
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyBrightness)
ApplyBrightness:
  Get PP volume → Settings → Set Auto Exposure Bias to log2(Get Oryx Settings → GetBrightness)
  (or set a scalar parameter on a global PP material instance)
```

### Tutorial hints gate

In WBP_HintOverlay (or wherever hints fire):
```
On hint trigger:
  Branch on Get Oryx Settings → GetTutorialHintsEnabled
    True → Show hint
    False → Skip
```

### FPS counter (playtester / dev only)

In WBP_PlayerHUD:
```
Tick (or 0.5s timer):
  Branch on Get Oryx Settings → GetShowFPSCounter
    True  → Update FPSText with 1.0 / Delta Seconds → as int
    False → Hide FPSText
```
Hide the toggle UI entirely in shipping builds — check `Is Shipping Build` BP node or use a `#if !UE_BUILD_SHIPPING` guard in C++ if you make the toggle a UFUNCTION gate.

### Display colour grading (Brightness / Contrast / Saturation)

In Blockout level BP Event BeginPlay, set up a global PP volume reference:
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyColorGrading)
ApplyColorGrading:
  PP volume → Settings → Set Auto Exposure Bias to log2(GetBrightness)
  PP volume → Settings → Color Grading → Global → Contrast = (1.0, 1.0, 1.0, GetContrastBoost)
  PP volume → Settings → Color Grading → Global → Saturation = (1.0, 1.0, 1.0, GetSaturation)
```
Set the override flags for each (bOverride_ColorContrast / bOverride_ColorSaturation) when writing values; otherwise the PP volume ignores them.

### Camera distance

In BP_OryxCharacter Event BeginPlay (after Super):
```
Get Oryx Settings → OnSettingsChanged → Add Custom Event (ApplyCameraDistance)
ApplyCameraDistance:
  CameraBoom → Set Target Arm Length (Get Oryx Settings → GetCameraDistance)
```
Call once on BeginPlay too so initial arm length matches saved setting.

### Reduce Motion gate

Consumers AND-gate motion-heavy effects against `GetReduceMotion`:
```
// Screen shake invocation:
Effective = ScreenShakeIntensity × (bReduceMotion ? 0 : 1)

// Motion blur post-process:
Final = bMotionBlur AND NOT bReduceMotion

// Boss telegraph dolly:
Branch GetReduceMotion → True: skip dolly, just play VFX
                       → False: dolly + VFX
```

### Subtitle size + opacity

In WBP_Subtitle (when audio + subtitles ship):
```
On Construct:
  Self → Set Render Scale (Vector2D(GetSubtitleSize))
  Background Image → Set Opacity (GetSubtitleOpacity)
  Bind OnSettingsChanged → RefreshSubtitleScale (repeat both above)
```

### Damage number scale

In BP_FloatingDamageActor Event BeginPlay:
```
Widget Component → Set World Scale (Vector3D(GetDamageNumberScale))
```
Already a per-instance spawn so no live consumer needed — each new floating number reads the current setting at spawn time.

### Incoming damage numbers (split master toggle)

In BP_OryxCharacter HandleDamageReceived (existing handler):
```
Branch on (GetShowFloatingDamageNumbers AND GetShowIncomingDamageNumbers)
  True → Spawn BP_FloatingDamageActor
  False → No-op
```

In BP_OryxEnemy HandleDamageReceived (existing handler):
```
Branch on GetShowFloatingDamageNumbers  // master only — enemy taking damage = player dealing damage
  True → Spawn BP_FloatingDamageActor
  False → No-op
```

### Minimal HUD

In WBP_PlayerHUD Event Construct (and OnSettingsChanged refresh):
```
Branch on GetMinimalHUD:
  True:
    XP_Bar → Set Visibility (Collapsed)
    Gold_Counter → Set Visibility (Collapsed)
    Stamina_Bar → Set Visibility (Collapsed)
    AbilityCooldownText → Set Visibility (Collapsed)
    DoTIcons → Set Visibility (Collapsed)
    // Keep: HealthBar + ManaBar + AbilityIcons (essential)
  False:
    All → Set Visibility (Visible)  // then respect individual toggles
```

### Auto-save interval (when RunSave snapshot lands)

AOryxRunManager (post-snapshot-work) reads `GetAutoSaveIntervalSeconds` at `StartRun` to set the recurring timer:
```cpp
// C++ stub for the wiring once mid-run save lands:
const int32 Interval = UOryxGameUserSettings::GetOryxSettings()->GetAutoSaveIntervalSeconds();
if (Interval > 0) {
    GetWorld()->GetTimerManager().SetTimer(AutoSaveTimer, this,
        &AOryxRunManager::DoAutoSave, (float)Interval, true);
}
```

## Settings file location

`%LOCALAPPDATA%/UnrealEngine/Oryx/Saved/Config/Windows/GameUserSettings.ini`

The `[/Script/Oryx.OryxGameUserSettings]` section will hold all Oryx-side fields. Engine-side fields are in `[/Script/Engine.GameUserSettings]` in the same file.

## Adding a new setting later (recipe)

1. Add `UPROPERTY(Config)` field to `OryxGameUserSettings.h` private section.
2. Add `BlueprintCallable` getter + setter pair (setter clamps + early-outs + calls `NotifyAndSave()`).
3. Add reset line to `ResetOryxDefaults()`.
4. Add to `WBP_Settings` UI in the relevant tab.
5. Add live consumer if it should react without restart (bind to `OnSettingsChanged`).

## Defaults rationale

| Setting | Default | Why |
|---|---|---|
| `bShowFloatingDamageNumbers` | true | Demo expectation; player wants feedback |
| `bShowHitMarkers` | true | Combat readability |
| `bShowEnemyHealthBars` | true | Combat readability — turn off for "hardcore" mode |
| `ScreenShakeIntensity` | 1.0 | Full effect; accessibility users dial down |
| `CameraShakeIntensity` | 1.0 | Same as above |
| `FXDensity` | 1.0 | Full effects on Epic preset |
| `bMotionBlur` | true | UE default; pitch wants cinematic look |
| `bFilmGrain` | false | Dark fantasy aesthetic, but grain divides players — opt-in |
| `bChromaticAberration` | false | Same as grain — divisive |
| `bBloom` | true | Magic VFX legibility |
| `HUDOpacity` | 1.0 | Full visibility default |
| `UIScale` | 1.0 | Standard |
| `bShowAbilityCooldownText` | true | Combat readability |
| `FieldOfView` | 90.0 | Standard third-person |
| `ColorBlindMode` | None | Accessibility opt-in |
| `bShowSubtitles` | false | Audio not in scope yet — opt-in when audio lands |
| `bAutoPause` | true | Standard UX |
| `bShowBlood` | false | D24: 6+/E10+ rating compliance |
| `ActiveQualityPreset` | High | Default for modern hardware |
| `MasterVolume` | 1.0 | Full audio out the box |
| `MusicVolume` | 1.0 | Full music |
| `SFXVolume` | 1.0 | Full SFX |
| `UIVolume` | 1.0 | Full UI feedback |
| `AmbientVolume` | 1.0 | Full ambient |
| `DialogueVolume` | 1.0 | Future VO support |
| `bMuteOnFocusLoss` | true | Standard UX — alt-tab silences |
| `MouseSensitivity` | 1.0 | Neutral |
| `bInvertMouseY` | false | Standard |
| `ControllerSensitivity` | 1.0 | Neutral |
| `bInvertControllerY` | false | Standard |
| `RumbleIntensity` | 1.0 | Full rumble — accessibility users zero out |
| `bToggleSprint` | false | Hold is the demo expectation; toggle is opt-in |
| `bShowEnemyOutlines` | true | Combat readability priority |
| `bShowInteractableHighlight` | true | Discoverability |
| `bShowDamageDirectionIndicator` | true | Combat awareness |
| `bShowDoTIcons` | true | Status visibility |
| `Brightness` | 1.0 | Neutral exposure; biome 2 may need users to bump |
| `bTutorialHintsEnabled` | true | Demo expectation; experienced players turn off |
| `bShowFPSCounter` | false | Off by default; playtesters enable for perf reports |
| `bShowIncomingDamageNumbers` | true | Sub-toggle to master; default-on for combat awareness |
| `DamageNumberScale` | 1.0 | Neutral; low-vision users bump up |
| `bMinimalHUD` | false | Default = full HUD; streamers / immersion mode opt-in |
| `CameraDistance` | 400 | Standard third-person framing |
| `SubtitleSize` | 1.0 | Default size |
| `SubtitleOpacity` | 0.9 | Mostly opaque plate for legibility |
| `bReduceMotion` | false | Off by default; accessibility users opt-in |
| `ContrastBoost` | 1.0 | Neutral colour grading |
| `Saturation` | 1.0 | Neutral |
| `AutoSaveIntervalSeconds` | 300 | D16 spec (5min); 0 disables; range [60, 600] |
