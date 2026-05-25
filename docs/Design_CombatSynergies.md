# Combat Synergies — Chain Reactions + Curse Shrines

Concrete gameplay design for D22 (damage type chain reactions) + D23 (curse/risk system). Reference for P5/P6 implementation.

---

## Part 1 — Damage Type Chain Reactions (D22)

Status effects can already exist on enemies. When two specific effects are simultaneously active, they combine into a powerful new effect that consumes both.

### Implementation pattern

When `UOryxStatusEffectsComponent::ApplyEffect` fires:
1. Check if target already has the "paired" effect from any source
2. If yes → trigger combo effect + clear both source effects
3. Combo effects are one-shot bursts (not persistent), broadcast a separate `OnComboTriggered` delegate for VFX/SFX/screen shake

### Combo table

| Pair | Combo Name | Effect | Visual cue |
|---|---|---|---|
| **Burn + Chill** | **Shatter** | Large physical burst damage (200% of target's MaxHealth × 0.15). Both effects removed. Brief target stun (0.3s). | White starburst + glass break SFX |
| **Poison + Curse** | **Plague** | Poison spreads to all enemies within 600cm radius + 50% larger DoT for 4s. Original effects removed from source. | Green expanding ring + dark mist |
| **Fire + Magic** | **Pyroclasm** | Knockback all enemies within 400cm (impulse 1500) + 25% damage burst. Both effects removed. | Orange-purple explosion radial |
| **Cold + Curse** | **Wither** | Target receives +50% damage from all sources for 6s. Cold effect persists; Curse consumed. | Black-blue aura with cracking sound |
| **Magic + Curse** | **Hex** | Target's outgoing damage reduced by 50% for 5s + lingering Magic DoT. Both effects removed. | Purple swirl over enemy head |
| **Poison + Fire** | **Pyrotoxin** | All enemies within 500cm gain Poison + 30% AttackSpeed buff to player for 4s. (Self-buff combo). | Green-orange swirl, brief player aura |

### Six damage types × pair logic

We have 6 damage types: Physical, Fire, Cold, Magic, Curse, Poison.
- Physical doesn't apply status effects — excluded from combos.
- 5 elemental types × pairings = `5 × 4 / 2 = 10` possible combos.
- **6 implemented above** = enough for build identity, room to add 4 more in post-EA content drops.

### Tuning notes

- Chain damage scales with **player's relevant damage stats** (Fire side scales with FireDamage, Cold side scales with ColdDamage)
- Player needs to ACHIEVE the pair (apply Burn AND Chill) — not just passive synergy
- Combos do NOT chain into more combos (a Shatter doesn't trigger another effect)
- Boss enemies get reduced combo damage (60% of normal) to prevent trivializing boss fights

### BP wiring

`UOryxStatusEffectsComponent::ApplyEffect`:
```cpp
// Pseudocode
void ApplyEffect(FOryxStatusEffectSpec NewEffect)
{
    EOryxStatusEffectType PairedType = LookupComboPair(NewEffect.Type);
    if (PairedType != Invalid && HasEffect(PairedType))
    {
        TriggerCombo(NewEffect.Type, PairedType);
        RemoveEffect(NewEffect.Type);
        RemoveEffect(PairedType);
        return; // Don't add the new effect — combo consumed it
    }
    // ... normal apply path
}
```

`LookupComboPair` is a static `TMap<EOryxStatusEffectType, EOryxStatusEffectType>` initialized once. `TriggerCombo` does damage / spawn VFX / play SFX / fire delegate.

---

## Part 2 — Curse Shrine System (D23)

Player encounters 1-2 shrines per run. Interact → choose a curse → gain power + accept a downside.

### Implementation pattern

`AOryxShrine` — new actor type, child of `AActor` + `IOryxInteractable`.
- Spawned by RunManager during stage gen (1-2 per stage)
- Placement: marked nodes in the level, RunManager picks N at random per run
- One-shot per shrine: interact → curse applied → shrine destroyed/disabled
- Curse applied via `FOryxStatModifier` with a unique `SourceId` for clean revoke on death/extract

### Curse pool (8 curses)

| Curse Name | Power | Cost | Implementation |
|---|---|---|---|
| **Sworn Vow** | +50% Damage (global) | -50% Max Health | `+0.50 Damage Additive` / `-0.50 MaxHealth Multiplicative` |
| **Glass Heart** | +100% CritChance, +50% CritDamage | Player dies in 1 hit (HP cap = 1) | `+1.0 CritChance / +0.5 CritDamage`. Special HP cap implemented via override on HealthComponent. |
| **Bound Soul** | +30% to ALL damage types (6 element stats) | Cards do nothing — picking a card grants no buff | `+0.30 Fire/Cold/Magic/Curse/Poison/Physical`. Card-application path checks a "BoundSoul" flag and no-ops. |
| **Ravenous** | +30% AttackSpeed | Lose 10HP/sec outside combat (no enemy in 1500cm) | `+0.30 AttackSpeed Multiplicative`. Tick: scan enemies within radius; if none alive, drain. |
| **Iron Will** | +50 Armor (caps at 100) | Cannot dash | `+50 Armor Additive`. Dash input gated by flag check. |
| **Featherweight** | +50% MovementSpeed | -50% Armor (down to 0 min) | `+0.50 MoveSpeed Multiplicative` / `-0.50 Armor Multiplicative` |
| **Berserker's Bargain** | +100% Damage WHEN HP <30% | -25% Damage when HP >70% | Conditional modifier: HealthComponent checks HP fraction, broadcasts to StatsComponent which applies the right modifier dynamically. |
| **Empty Coffers** | +1 item drop per kill | -50% gold drop | Pickup spawn logic checks flag; gold drop reduced, item drop chance increased. |

### Shrine UI

`WBP_CursesShrine`:
- 3 curse cards displayed (random 3 from pool)
- Hover shows power + cost preview
- "Accept" button per card + "Walk Away" button at bottom
- Walk away = shrine still consumed (no second pick this run) but no curse applied

### Tuning notes

- Curses are run-only — cleared on death/extract via tracked `SourceId` GUIDs
- Multiple curses can stack (player encounters 1-2 shrines per stage = up to 6 curses across 3 stages)
- Some curses have anti-stacking rules: "Glass Heart" cannot be combined with "Sworn Vow" (would be 1HP / -50% HP = unplayable)
- Players record which curses they took on the WBP_RunTally screen — fun metadata

### Authoring tasks (post-funding)

1. Create `Source/Oryx/Actors/Shrines/OryxShrine.h/.cpp`
2. Create `Source/Oryx/Systems/Shrines/OryxCurseLibrary.h/.cpp` (static curse definitions)
3. Author 8 `UOryxCurse` DataAssets (or hardcoded — DataAssets give more flexibility)
4. Create `BP_Shrine` with 4 placeholder mesh variants (one per biome theme)
5. Build `WBP_CursesShrine` UI
6. Wire RunManager spawning logic (1-2 per stage at marked placement points)

Estimated work: ~10 hours total.

### Future expansion

Post-1.0 we could add:
- **Boon shrines** — opposite of curses. Pure positive buff but consume gold cost (10× rarity tier).
- **Story-locked curses** — only available after certain achievement milestones.
- **Mutator curses** — change gameplay rules, not just stats (e.g., "Doomed" — must complete boss in 2 minutes or run ends).
