# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Oryx** is an Unreal Engine 5.7 third-person action game built as a multi-variant test bed. It contains three independent gameplay modes sharing a common character base:
- **Variant_Combat** — Melee action with AI enemies and a combo/charged-attack system
- **Variant_Platforming** — Precision platformer with advanced jump mechanics
- **Variant_SideScrolling** — 2D side-scrolling platformer with interaction and soft platforms

Default startup map: `Content/Map/Blockout.Blockout`. Default game mode: `BP_OryxGameMode`.

## Build & Compile

This is a standard UE5 C++ project. There is no custom build script — use the standard UBT/IDE workflow:

- **Editor build (Visual Studio)**: Open `Oryx.sln`, build target `OryxEditor Development Win64`
- **Game build**: Build target `Oryx Development Win64`
- **Regenerate project files**: Right-click `Oryx.uproject` → "Generate Visual Studio project files"
- **Live coding** (in-editor): `Ctrl+Alt+F11`

Module: `Oryx` (single runtime module, `Source/Oryx/Oryx.Build.cs`)

## Architecture

### Module Layout

All C++ lives in `Source/Oryx/`. The module is split into a shared base layer and three variant namespaces:

```
Source/Oryx/
├── Characters/Player/OryxCharacter       ← Abstract base (health/stamina/mana, dash, sprint)
├── Characters/Enemies/Base/OryxEnemy     ← Abstract base enemy
├── Controllers/Player/OryxPlayerController
├── GameModes/Base/OryxGameMode
├── Projectiles/Base/OryxProjectile
├── Systems/                              ← Stubs (Abilities, Combat, Loot, Progression, Waves)
├── Variant_Combat/
├── Variant_Platforming/
└── Variant_SideScrolling/
```

### Variant Structure (each variant mirrors this pattern)

```
Variant_<X>/
├── Characters/          ← Variant-specific player + enemy characters
├── Controllers/         ← Variant PlayerController + GameMode
├── AI/                  ← AIController + StateTree utilities + EQS contexts
├── Animation/           ← AnimNotifies
├── Gameplay/            ← Level actors (volumes, hazards, platforms, pickups)
├── Interfaces/          ← ICombatAttacker / ICombatDamageable etc.
└── UI/                  ← HUD widgets (LifeBar, etc.)
```

Content mirrors this: `Content/Variant_Combat/`, `Content/Variant_Platforming/`, `Content/Variant_SideScrolling/`.

### Combat System (Variant_Combat)

The combat loop is **montage-driven + interface-based**:

1. Player/Enemy implements `ICombatAttacker` and `ICombatDamageable`.
2. Attack input triggers an AnimMontage.
3. `AnimNotify_DoAttackTrace` fires a sphere trace during the active frames → calls `DoAttackTrace(DamageBone)`.
4. `AnimNotify_CheckCombo` / `AnimNotify_CheckChargedAttack` read cached input to chain attacks.
5. `ApplyDamage(Damage, DamageCauser, Location, Impulse)` applies knockback + launch to hit actors.
6. `NotifyDanger(Location, Source)` is called on enemies *before* the hit lands so AI can react.

Both `CombatCharacter` and `CombatEnemy` implement both interfaces — the logic works identically regardless of whether the actor is player or AI controlled.

### AI System

The project uses **StateTree exclusively** (no legacy BehaviorTree). AI controllers hold a `StateTreeAIComponent`. EQS contexts (`EnvQueryContext_Player`, `EnvQueryContext_Danger`) feed perception data into the tree. `CombatStateTreeUtility` / `SideScrollingStateTreeUtility` expose helper functions callable from StateTree tasks.

### Input

All variants use the **Enhanced Input System**. Character base exposes `Do*` methods (DoMove, DoLook, DoJump, DoFire, DoDash, DoSprint) called from input actions. Input mapping contexts and actions live in `Content/<Variant>/Input/Actions/`. Mobile touch controls are supported via a UMG overlay widget.

### Key Movement Values (for reference when modifying)

| Mechanic | Value |
|---|---|
| Walk speed | 500 cm/s |
| Sprint speed | 800 cm/s |
| Dash impulse | 1200 cm/s |
| Coyote time (Platforming/SideScrolling) | 0.16 s |
| Wall jump bounce (Platforming) | 800 horizontal / 900 vertical cm/s |
| Wall jump bounce (SideScrolling) | 500 horizontal, 1.4× jump vertical |
| Melee sphere trace radius | 75 cm |
| Danger notification trace | 300 cm |

## Dependencies (Oryx.Build.cs)

Public module dependencies: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `UMG`, `Slate`.

Enabled plugins: `StateTree`, `GameplayStateTree`, `ModelingToolsEditorMode`, `VisualStudioTools` (Win64 editor only).

## Rendering Configuration

From `Config/DefaultEngine.ini`: DirectX 12 / SM6, ray tracing enabled, Lumen GI enabled, Substrate material system enabled.
