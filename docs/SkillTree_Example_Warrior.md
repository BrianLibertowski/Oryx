# Skill Tree Authoring Example — Warrior (D21 — 5 trees, demo)

> **Refactored 2026-05-26 to match D21.** Previous version was authored against D14 (1 linear passive + 5 ability cluster trees = 6 total). D21 collapses to **5 trees per class**, no separate passive — each tree is built around one of the class's 5 anchor abilities, with passive modifier nodes that buff/modify that ability or the class's general stats.
>
> The C++ machinery is unchanged:
> - `UOryxSkillTree.TreeType` still has Passive + Ability enum options — just author all 5 as `Ability` for D21 (`Passive` is a legacy option from D14 days; safe to leave declared).
> - `MaxInvestedInTree` stays 15 (D14 spec retained — keeps you from maxing every node and forces choices).
> - `bIsBaseAbility` flag on the center node = ability granted free.
> - `MaxRanks` per node retained (3 ranks per modifier node, stepped values).
>
> Use this Warrior doc as a template. Pattern carries to Archer / Assassin / Mage with their own ability lists.

---

## The 5 Warrior Trees

| # | Tree | Anchor Ability (free center node) | Theme |
|---|---|---|---|
| 1 | **Cleave Mastery** | `Ability_Cleave` (sword swing primary) | Crit, attack speed, basic-attack scaling |
| 2 | **Heavy Strike** | `Ability_HeavyStrike` (telegraphed slow swing) | Burst damage, knockback, single-target |
| 3 | **Battle Rush** | `Ability_BattleRush` (charge/dash with impact) | Mobility, charge damage, gap-closer utility |
| 4 | **War Cry** | `Ability_WarCry` (AoE buff/debuff shout) | Group buffs, debuffs on enemies, sustain |
| 5 | **Bulwark** | `Ability_ShieldStance` (defensive stance/block) | Armor, damage reduction, MaxHealth |

Each tree:
- 10 nodes total (1 center + 9 modifiers)
- Center = `bIsBaseAbility = true`, MaxRanks = 1, no cost (granted free)
- 9 modifier nodes × 3 ranks each = 27 ranks per tree
- `MaxInvestedInTree` = 15 → can't fully max every modifier; forces ~5 nodes maxed or wider shallow spread

Total Warrior demo content: 5 trees × ~9 modifier nodes = 45 unique modifier nodes. Level 20 demo cap means a Warrior has 19 ability points (1 ability point per 2 levels per D14) — barely enough to fill 1.25 trees. Players pick which 1-2 trees to specialize in.

---

## DataAsset Setup (per tree)

Create each at `/Game/SkillTrees/Warrior/<TreeName>`:

```
/Game/SkillTrees/Warrior/
├── Tree_Cleave.uasset
├── Tree_HeavyStrike.uasset
├── Tree_BattleRush.uasset
├── Tree_WarCry.uasset
└── Tree_Bulwark.uasset
```

For each `UOryxSkillTree` asset:
- **OwningClass** = `Warrior`
- **TreeType** = `Ability`
- **TreeName** = display name (e.g. "Cleave Mastery")
- **MaxInvestedInTree** = `15`

---

## Tree 1 — Cleave Mastery (FULL SPEC — template for the other 4)

### Center node — free, `bIsBaseAbility = true`

| NodeId | DisplayName | Description | MaxRanks | Modifier |
|---|---|---|---|---|
| `War_Cleave_00_Ability` | Cleave | Wide sword swing in front. Base melee. | 1 | (none — ability grant only) |

### Tier 1 — Foundation (0 points in tree required)

| NodeId | DisplayName | MaxRanks | Modifier per rank | RequiredNodes |
|---|---|---|---|---|
| `War_Cleave_01_AtkSpd` | Swift Cuts | 3 | `+0.10 AttackSpeed (Additive)` | `War_Cleave_00_Ability` |
| `War_Cleave_02_PhysPower` | Brute Force | 3 | `+0.05 PhysicalPower (Additive)` | `War_Cleave_00_Ability` |

### Tier 2 — Specialize (2 points in tree required)

| NodeId | DisplayName | MaxRanks | Modifier per rank | RequiredNodes |
|---|---|---|---|---|
| `War_Cleave_03_CritChance` | Killing Edge | 3 | `+0.04 CritChance (Additive)` | `War_Cleave_01_AtkSpd` |
| `War_Cleave_04_Damage` | Onslaught | 3 | `+0.05 Damage (Additive)` | `War_Cleave_02_PhysPower` |
| `War_Cleave_05_Armor` | Stalwart | 3 | `+5 Armor (Additive)` | `War_Cleave_00_Ability` |

### Tier 3 — Mid-tier (5 points in tree required)

| NodeId | DisplayName | MaxRanks | Modifier per rank | RequiredNodes |
|---|---|---|---|---|
| `War_Cleave_06_CritDmg` | Devastate | 3 | `+0.10 CritDamage (Additive)` | `War_Cleave_03_CritChance` |
| `War_Cleave_07_Health` | Hardy | 3 | `+25 MaxHealth (Additive)` | `War_Cleave_05_Armor` |

### Tier 4 — Capstone-adjacent (10 points in tree required)

| NodeId | DisplayName | MaxRanks | Modifier per rank | RequiredNodes |
|---|---|---|---|---|
| `War_Cleave_08_Frenzy` | Frenzy | 3 | `+0.08 AttackSpeed (Multiplicative)` | `War_Cleave_06_CritDmg` AND `War_Cleave_04_Damage` |
| `War_Cleave_09_Executioner` | Executioner | 3 | `+0.15 CritDamage (Additive)` | `War_Cleave_06_CritDmg` |

### Investment shapes (level 20 — 19 ability points)

- **Pure DPS:** 01×3 + 02×3 + 03×3 + 06×3 + 08×3 = 15 points → high attack speed + crit chance + crit damage stacking
- **Hybrid:** 01×3 + 02×3 + 04×3 + 05×3 + 07×3 = 15 points → balanced damage + survival
- **Off-pick:** spend ~5 points here, rest in another tree (e.g. Bulwark for defense)

---

## Tree 2 — Heavy Strike (skeleton)

- Center: `War_HS_00_Ability` (telegraphed slow heavy swing, 1.5× cleave damage)
- Modifier nodes scale damage / knockback / mana cost / windup duration / area / crit on heavy
- Suggested nodes: HeavyStrikeDamage / HeavyStrikeKnockback / FasterWindup / HeavyCritChance / HeavyCritDamage / HeavyAOE / DamageStacking / PhysPower / MaxHealth

## Tree 3 — Battle Rush (skeleton)

- Center: `War_BR_00_Ability` (charge that knocks enemies + i-frames)
- Modifier nodes: ChargeDamage / LongerCharge / FasterCooldown / MovementSpeed / Stamina / DamageWhileMoving / GapCloserAOE / DashImpactArmor / ChargeKnockbackStrength

## Tree 4 — War Cry (skeleton)

- Center: `War_WC_00_Ability` (AoE buff: +damage to nearby + debuff: -armor on enemies)
- Modifier nodes: WarCryRadius / DamageBuff / ArmorDebuff / DurationExtend / CooldownReduction / WarCryHealing / WarCryCritBuff / HealthRegen / Vitality

## Tree 5 — Bulwark (skeleton)

- Center: `War_BW_00_Ability` (Shield Stance — 50% damage reduction while active, slow movement)
- Modifier nodes: ArmorPassive / DR / MaxHealthBig / HealthRegen / DamageReflect / ShieldDurability / ShieldRange / TauntPassive / SturdyStance

> Each skeleton tree follows the same 10-node shape as Cleave Mastery: 1 free center + 2 foundation nodes + 3 specialize + 2 mid + 2 capstone. Copy the Cleave structure, swap NodeIds + display names + modifier choices.

---

## Authoring Workflow in Editor (per node)

1. Open `Tree_<Name>.uasset` in Content Browser.
2. Scroll to **Nodes** array → +Add.
3. Fill in `NodeId` exactly as listed in this doc (other nodes reference these via RequiredNodes — typos break the graph silently).
4. Set `DisplayName` + `Description` (player-facing).
5. Set `MaxRanks` (1 for center ability node, 3 for modifier nodes).
6. **RequiredNodes** array → add prereq NodeIds (FName matches — copy-paste to avoid typos).
7. **RequiredPointsInTree** → the tier soft-gate (0, 2, 5, or 10 for the 4 tiers).
8. **ModifiersPerRank** → one `FOryxStatModifier` entry per rank. For stepped values, all 3 rank entries hold the SAME value (rank 1 = +0.10 AS, rank 2 cumulative = +0.20, etc — the C++ stat aggregator sums modifiers).
   - Stat (enum)
   - Op (Additive / Multiplicative)
   - Value (e.g. 25 for +25 Health, 0.10 for +10%)
9. For the center node: leave `ModifiersPerRank` empty, set `bIsBaseAbility = true`.
10. Compile + Save asset.

## Tree Registry (required for save-load skill rehydration)

`UOryxGameInstance` will need a `TArray<UOryxSkillTree*> RegisteredSkillTrees` UPROPERTY (configured per-asset on BP_OryxGameInstance) so the load path can walk allocated NodeRanks and re-push stat modifiers. **Not yet wired** — flagged in bugs.md as `[Medium]` follow-up. Until then, the demo persists XP/Level/UnallocatedPoints/NodeRanks data correctly, but the stat-modifier side of allocated nodes only goes live when the player re-invests in the editor session.

---

## Estimated Authoring Time (D21-revised)

- 1 ability tree fully spec'd in editor = ~40 min (modifier choices + RequiredNodes linkage takes time)
- Per class: 5 trees = ~3.5 hours
- All 4 classes: ~14 hours (unchanged from D14 estimate)
- **If demo ships Warrior-only:** ~3.5 hours of authoring work, not 14.

W6 has Mon-Sun to author — fits comfortably either way. Warrior-first is the recommended approach since it's the floor for Jul 15.

---

## Balance Targets (Demo-Tuned)

Warrior at level 20 with 19 ability points spent across trees should feel meaningfully stronger than level 1. Rough targets:

- **Pure Cleave spec (DPS) at level 20:** +0.30 AttackSpeed + 15% PhysicalPower + 12% CritChance + 30% CritDamage stacking → roughly 2.5× base DPS
- **Hybrid (Cleave + Bulwark):** ~1.8× DPS + +75 Armor + +50 MaxHealth → noticeably tankier
- **Multi-ability spec (5 points per tree):** broader utility, ~1.5× DPS but more abilities feel viable

Validate during W8 combat tuning playtest. If 19 points produces a power spike too large, reduce per-rank values uniformly (e.g. 0.10 AttackSpeed → 0.07).

---

## Quick Reference — All Stats Available

Available `EOryxStat` values (23 total per Phase 4 expansion):
- **HP/MP/Stamina:** MaxHealth, MaxMana, MaxStamina, HealthRegen, ManaRegen, StaminaRegen
- **Damage:** Damage (global), PhysicalPower, FireDamage, ColdDamage, MagicDamage, CurseDamage, PoisonDamage
- **Combat tempo:** AttackSpeed, CastSpeed, CooldownReduction
- **Crits:** CritChance, CritDamage
- **Defense:** Armor, DamageReduction
- **Misc:** MovementSpeed, Luck, GoldGain

Stat caps (D18): AttackSpeed=5.0, CooldownReduction=0.75, Armor=100, MovementSpeed=2000, DamageReduction=0.90 (soft). CritChance/CritDamage uncapped (overflow per design).
