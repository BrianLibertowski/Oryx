# Compile Checklist — 2026-05-26 First Pass

> **2026-06-13 BATCH — FULL REBUILD REQUIRED (new modules + new translation units, NOT Live-Coding-safe):**
> - `Oryx.Build.cs` gained **HTTP + Json** modules → regenerate project files + rebuild.
> - New .cpp files (new translation units): `Systems/Interaction/OryxInteractableRegistry.cpp`,
>   `Systems/BugReport/OryxBugReportSubsystem.cpp`, `Component/Stats/OryxStatFormatLibrary.cpp`.
> - Header changes: OryxAbility.h (bIsSpell), OryxSkillTreeComponent.h (ReapplyAllocations +
>   SpentPassive/AbilityPoints), OryxGameInstance.h (RegisteredSkillTrees), OryxCharacter.h (PossessedBy).
> - Path: close editor → Generate VS Project Files → Build Solution → relaunch.
> - **After rebuild:** add webhook URL to DefaultGame.ini for the bug reporter; add all skill tree
>   DataAssets to BP_OryxGameInstance → Registered Skill Trees.
>
> **2026-05-26 STATUS:** P3+P4 batch ✅ COMPILED via Live Coding. Same-session follow-up wiring (XP grant on kill, OnPlayerLevelUp delegate, Boss/Player death → RunManager, OryxGameInstance save/load) **awaits a second compile pass**. See "Second compile (post-wiring batch)" section at bottom.

## Pre-compile sanity

1. Close PIE if running.
2. Save all assets (Ctrl+Shift+S in editor) to flush any unsaved Modeling-Mode primitives.
3. Verify `Saved/` and `Intermediate/` aren't locked by another process.

## Compile path — try in order

### A. Live Coding (Ctrl+Alt+F11) — fastest
- Use first.
- **Expected outcome:** if successful, log shows "Patch was successfully compiled" with module name.
- **Likely failure modes:**
  - Header changes adding new UFUNCTION/UPROPERTY (we have several — `OryxEnemy.h SetAttackDamage`, `OryxLevelComponent.h` full refactor, `OryxPlayerState.h` new component) → Live Coding may reject. Move to path B.
  - Delegate signature changes (`FOnLevelUp` is now `(EOryxClass, int32)` not `(int32, int32)`) → may reject for same reason.

### B. Full recompile from IDE
1. Close UE editor entirely.
2. Right-click `Oryx.uproject` → Generate Visual Studio Project Files (or Rider equivalent).
3. Open VS solution → Build → Build Solution (or hit F7).
4. Wait for "Build succeeded" in output.
5. Re-launch UE editor by double-clicking `Oryx.uproject`.

### C. Manual UBT (last resort)
If both above fail:
```
"C:/Program Files/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" OryxEditor Win64 Development "C:/Users/Arzuh/Documents/Unreal Projects/Oryx/Oryx.uproject" -waitmutex
```

## Expected compile errors to watch for

| Error | File | Likely cause | Fix |
|---|---|---|---|
| Cannot find OryxClassTypes.h | various | Include path | Verify `Source/Oryx/Characters/OryxClassTypes.h` exists |
| Unknown class UOryxSkillTreeComponent | OryxPlayerState | Header include order | Check `OryxPlayerState.cpp` includes `Component/SkillTree/OryxSkillTreeComponent.h` |
| Delegate signature mismatch | OryxPlayerState binding | Old binding for OnLevelUp 2-int signature still in BP somewhere | Search BP for AddDynamic to OnLevelUp |
| EOryxStat::DamageReduction not declared | OryxStatsComponent.cpp | Enum value missing | Verify all 23 stats in EOryxStat enum |
| TimerManager.h not found | OryxBoss.cpp | Missing include | Should be auto-resolved by Engine.h |

## Post-compile verification

1. Open UE editor → no startup errors in Output Log.
2. Right-click in Content Browser → C++ Class → verify these classes appear:
   - `OryxClass` (the enum is hidden but the file is compiled)
   - `OryxSkillTree` (DataAsset)
   - `OryxSkillTreeComponent` (component)
   - `OryxProfileSave` (SaveGame)
   - `OryxRunSave` (SaveGame)
   - `OryxRunManager` (GameState)
   - `OryxBoss` (under Characters/Enemies/Boss/)
   - `OryxChest` (under Actors/Chests/)
   - `OryxEncounter_Outpost`, `OryxEncounter_EliteArena`
   - `OryxCardLibrary` (BlueprintFunctionLibrary)

3. **Smoke test BP_OryxCharacter compile:**
   - Open BP_OryxCharacter → Compile.
   - If errors about OnLevelUp delegate signature, fix the BP graph (the binding param order changed).

4. **Smoke test BP_Vendor still works:**
   - PIE Blockout → approach vendor → press Interact → vendor screen opens.

## If Things Are Broken

- Phase 3 + 4 are a big batch. Roll back individually if needed:
  - LevelComponent refactor was the biggest breaking change — if it fails, the encounter/playerstate fall back to single-class behavior temporarily. Revert via Git stash and proceed with Phase 3 BP work first.
- Generic advice: re-enable Live Coding ONLY after you've done one full clean recompile so reflection state is clean.

---

# Second compile (post-wiring batch, 2026-05-26)

After the initial batch compiled, the same session landed a follow-up wiring batch. **It contains a brand-new translation unit (`OryxGameInstance.cpp` — previously header-only), which Live Coding generally cannot pick up.**

## Files in this batch

**Modified:**
- `Characters/Enemies/Base/OryxEnemy.h` — added `XPReward` UPROPERTY (default 5).
- `Characters/Enemies/Base/OryxEnemy.cpp` — HandleDeath now routes XP via `AOryxPlayerState.LevelComponent.AddXPToActiveClass`. New includes: OryxLevelComponent.h, OryxCharacter.h, OryxPlayerState.h.
- `Characters/Enemies/Boss/OryxBoss.h/.cpp` — new `HandleBossDied` handler. BeginPlay binds HealthComp.OnDeath → calls `RunManager.HandleBossDefeated()`. New include: OryxRunManager.h.
- `Characters/Player/OryxCharacter.cpp` — HandleDeath now calls `RunManager.HandlePlayerDeath()` after game-over widget. New include: OryxRunManager.h.
- `States/Player/OryxPlayerState.h` — declared `FOnPlayerLevelUp(EOryxClass, int32)` delegate + `OnPlayerLevelUp` UPROPERTY.
- `States/Player/OryxPlayerState.cpp` — BeginPlay applies profile via `GameInstance->ApplyProfileToPlayerState(this)`. HandleLevelUp heals to full via HealthComponent->ApplyHealing(GetMaxHealth()) + broadcasts OnPlayerLevelUp + calls GameInstance->SaveProfile(this). New includes: OryxHealthComponent.h, OryxGameInstance.h, GameFramework/Pawn.h.
- `GameModes/Base/OryxGameInstance.h` — converted from header-only stub to full UCLASS with Save/Load API (LoadProfile / SaveProfile / ApplyProfileToPlayerState / HasRunSave / DeleteRunSave / GetCachedProfile + CachedProfile UPROPERTY).
- `GameModes/RunManager/OryxRunManager.h/.cpp` — new `PersistProfileAndCleanupRun()` private helper. HandlePlayerDeath / HandleExtract both call it before transitioning state.

**New file (the gotcha):**
- `GameModes/Base/OryxGameInstance.cpp` — implementation of Init / LoadProfile / SaveProfile / ApplyProfileToPlayerState / HasRunSave / DeleteRunSave.

## Audit pass additions (2026-05-26, same session)

Two correctness bugs found + fixed:
- **`AOryxCharacter::HandleDeath`** now gates the `RunManager.HandlePlayerDeath()` call on `IsPlayerControlled()`. Without the gate, a future AI-controlled `AOryxCharacter` (companion subclass) dying would end the run.
- **`UOryxGameInstance::SelectedClass`** is a new `EOryxClass` UPROPERTY. `WBP_CharacterSelection` MUST set it alongside `SelectedPawnClass` when the player picks a class. `ApplyProfileToPlayerState` now calls `LC->SetActiveClass(SelectedClass)` instead of `LastPlayedClass`. Without this, XP would route to whatever class the profile last saved, regardless of which class the player just picked. `LoadProfile` syncs `SelectedClass = CachedProfile->LastPlayedClass` on game start so a relaunch defaults to last-played.

## Co-op proofing pass (2026-05-26, same session)

Six fixes that remove single-player ASSUMPTIONS from this session's wiring without adding actual replication. Single-player behavior unchanged.

- **`UOryxHealthComponent`** captures `LastInstigator` TWeakObjectPtr in ApplyDamageEvent. New `GetLastInstigator()` BlueprintPure.
- **`AOryxEnemy::HandleDeath`** XP grant routes through `HealthComponent->GetLastInstigator()` cast to AOryxCharacter; falls back to iterator when instigator is unknown.
- **`AOryxEnemy::HandleDamaged`** aggro target prefers actual damage dealer over "nearest player" guess; same fallback shape.
- **`AOryxRunManager::HandlePlayerDeath`** counts alive AOryxCharacter actors and only ends the run when count == 0. Single-player: identical (1 player → 0 alive after death → ends).
- **`AOryxRunManager::PersistProfileAndCleanupRun`** removed early break, now gates SaveProfile on `PC->IsLocalController()` per-PlayerState.
- **`AOryxPlayerState::BeginPlay`** + **`AOryxPlayerState::HandleLevelUp`** both gated on `IsLocalPlayerState(this)` helper (`GetPlayerController()->IsLocalController()`). Prevents host from clobbering remote clients' profiles or applying host's profile to remote PlayerStates.

What's still deferred to the actual replication pass (post-demo):
- Replicates flags on components, Server RPCs on Request*/ApplyDamage paths, HasAuthority gates on damage/spawning/reward sites, D19 downed-state + revive shrine.

## Graphics settings scaffold

`Source/Oryx/Settings/OryxGameUserSettings.h/.cpp` expanded substantially. New API:
- 2 enums: `EOryxQualityPreset` (Low/Medium/High/Epic/Cinematic/Custom) + `EOryxColorBlindMode`
- `FOnOryxSettingsChanged` BlueprintAssignable multicast delegate (zero params; listeners re-read what they care about)
- ~18 Config UPROPERTYs across Combat / VFX / UI / Camera / Accessibility / Content categories
- Getter+Setter pair per UPROPERTY (setters clamp + early-out + SaveSettings + broadcast)
- `ApplyQualityPreset(Preset)` — batch-writes engine scalability buckets in one call
- `ResetOryxDefaults()` — restores all Oryx-side fields, broadcasts once

See `docs/Settings_BPConsumption.md` for the BP wire-up reference.

## Compile path for this batch

**DO NOT try Live Coding first.** Live Coding does not pick up new .cpp files (no new translation unit registration). Symptoms when you try it anyway: "patch succeeded" message but linker complains at runtime about missing UOryxGameInstance symbols, OR the new functions silently call into stale code paths.

Required path:
1. Close UE editor entirely.
2. Right-click `Oryx.uproject` → Generate Visual Studio Project Files.
3. Open VS solution → Build → Build Solution (F7).
4. Re-launch UE editor.

If VS still doesn't see `OryxGameInstance.cpp`, force-refresh by deleting `Binaries/` and `Intermediate/` then re-running step 2.

## Expected errors to watch for (this batch)

| Error | File | Likely cause | Fix |
|---|---|---|---|
| Unknown class UOryxGameInstance | OryxGameInstance.cpp / OryxPlayerState.cpp / OryxRunManager.cpp | Forgot to regen project files | Step 2 above |
| Unresolved external symbol UOryxGameInstance::SaveProfile | linker | Live Coding tried to patch new .cpp | Full rebuild |
| OnDeath.AddDynamic ambiguous | OryxBoss.cpp | Parent AOryxEnemy already binds OnDeath in its BeginPlay | Not an error — multicast supports multiple listeners |
| FOnPlayerLevelUp not declared | OryxPlayerState.cpp | Header didn't pick up the new delegate | Recompile (full, not Live Coding) |
| Missing GameFramework/Pawn.h | OryxPlayerState.cpp | Pawn forward-declare wasn't enough for GetPawn() | Already included — verify |

## Post-compile verification (this batch)

1. PIE Blockout → kill an enemy → check Output Log for "XP grant" or open Stats Screen and verify XP bar advances.
2. Force a level-up (cheat: lower XP curve in BP defaults, or use console `Stat fps` only kidding — just kill enough enemies) → expect health to refill instantly + look for `OryxGameInstance::SaveProfile → OK` in log.
3. After level-up, quit PIE → reopen → verify per-class XP/Level persisted (check Stats Screen, or breakpoint LevelComponent::GetCurrentLevel).
4. Verify SaveGame file appeared on disk: `%LOCALAPPDATA%/UnrealEngine/Oryx/Saved/SaveGames/ProfileSave.sav`.
5. Run-end smoke: trigger boss kill → verify Output Log shows `RunManager: state ... → BossDefeated`. Trigger player death → verify state goes to Dead AND `DeleteRunSave` removed `RunSave.sav` if it existed.
