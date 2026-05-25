# Compile Checklist — 2026-05-26 First Pass

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
