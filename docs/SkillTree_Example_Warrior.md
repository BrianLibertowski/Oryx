# Skill Tree Authoring Example — Warrior Passive (Demo, 20 nodes)

Concrete reference for authoring a `UOryxSkillTree` DataAsset. Use this as a copy-and-modify template for the other 23 trees (4 classes × (1 passive + 5 ability) − 1 = 23).

## DataAsset Setup

Create at `/Game/SkillTrees/Warrior/Passive_WarriorMain`:
- **OwningClass** = `Warrior`
- **TreeType** = `Passive`
- **TreeName** = `"Warrior Mastery"`
- **MaxInvestedInTree** = `20` (full demo passive tree)

## Linear Passive Tree Structure (20 nodes)

Nodes flow left-to-right with branching at tiers. Each node has 1 rank unless noted.

### Tier 1 — Foundation (no prereqs)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_01_Health` | Hardy | `+25 MaxHealth (Additive)` | 0 | (none) |
| `Warrior_Pass_02_Armor` | Stalwart | `+10 Armor (Additive)` | 0 | (none) |

### Tier 2 — Specialise (need 2 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_03_PhysPower` | Brute Strength | `+10% PhysicalPower (Additive)` | 2 | (none) |
| `Warrior_Pass_04_AttackSpeed` | Swift Cuts | `+5% AttackSpeed (Multiplicative)` | 2 | (none) |
| `Warrior_Pass_05_CritChance` | Killing Edge | `+5% CritChance (Additive)` | 2 | (none) |

### Tier 3 — Branch left (combat) (need 4 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_06_HealthBig` | Resilience | `+50 MaxHealth (Additive)` | 4 | `Warrior_Pass_01_Health` |
| `Warrior_Pass_07_PhysPowerBig` | Heavy Hands | `+15% PhysicalPower (Additive)` | 4 | `Warrior_Pass_03_PhysPower` |
| `Warrior_Pass_08_CritDmg` | Devastate | `+10% CritDamage (Additive)` | 4 | `Warrior_Pass_05_CritChance` |

### Tier 3 — Branch right (defense) (need 4 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_09_ArmorBig` | Iron Skin | `+15 Armor (Additive)` | 4 | `Warrior_Pass_02_Armor` |
| `Warrior_Pass_10_DR` | Damage Reduction | `+5% DamageReduction (Additive)` | 4 | `Warrior_Pass_02_Armor` |

### Tier 4 — Hybrid (need 7 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_11_HealthRegen` | Vitality | `+3 HealthRegen (Additive)` | 7 | `Warrior_Pass_06_HealthBig` |
| `Warrior_Pass_12_CDR` | Battle Rhythm | `+5% CooldownReduction (Additive)` | 7 | `Warrior_Pass_07_PhysPowerBig` |
| `Warrior_Pass_13_Damage` | Onslaught | `+10% Damage (Additive)` | 7 | `Warrior_Pass_08_CritDmg` |

### Tier 5 — Capstone-adjacent (need 11 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_14_HealthHuge` | Bulwark | `+100 MaxHealth (Additive)` | 11 | `Warrior_Pass_11_HealthRegen` |
| `Warrior_Pass_15_CritChanceBig` | Headhunter | `+10% CritChance (Additive)` | 11 | `Warrior_Pass_12_CDR` |
| `Warrior_Pass_16_AttackSpeedBig` | Frenzy | `+10% AttackSpeed (Multiplicative)` | 11 | `Warrior_Pass_13_Damage` |

### Tier 6 — Pre-capstone (need 15 points in tree)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_17_ArmorHuge` | Mountain | `+25 Armor (Additive)` | 15 | `Warrior_Pass_09_ArmorBig` AND `Warrior_Pass_10_DR` |
| `Warrior_Pass_18_PhysPowerHuge` | Hammer Strike | `+25% PhysicalPower (Additive)` | 15 | `Warrior_Pass_14_HealthHuge` OR `Warrior_Pass_16_AttackSpeedBig` |
| `Warrior_Pass_19_CritDmgHuge` | Executioner | `+25% CritDamage (Additive)` | 15 | `Warrior_Pass_15_CritChanceBig` |

### Tier 7 — Capstone (need 18 points in tree, AND 2 specific prereqs)

| NodeId | DisplayName | Modifier | Required Points | Required Nodes |
|---|---|---|---|---|
| `Warrior_Pass_20_Capstone` | Apex Warrior | `+50 MaxHealth + +15% PhysicalPower + +10% AttackSpeed` (multi-modifier node) | 18 | `Warrior_Pass_17_ArmorHuge` AND `Warrior_Pass_19_CritDmgHuge` |

> **Note:** Multi-modifier nodes — set `ModifiersPerRank[0]` to be an array of 3 entries (a single rank with 3 stat changes). Each rank applies all its modifiers.

## Authoring Workflow in Editor

For each node:

1. Open `Passive_WarriorMain.uasset` in Content Browser.
2. Scroll to **Nodes** array → +Add.
3. Fill in `NodeId` exactly as listed (these get referenced by other nodes via RequiredNodes).
4. Set `DisplayName` and `Description` (player-facing).
5. Set `MaxRanks` (default 1 for passive tree; multi-rank only for ability trees).
6. **RequiredNodes** array → add the prereq NodeIds (use FName matches).
7. **RequiredPointsInTree** → soft-gate threshold.
8. **ModifiersPerRank** → add one FOryxStatModifier per rank:
   - Stat (enum)
   - Op (Additive / Multiplicative)
   - Value (e.g. 25 for +25 Health, 0.10 for +10%)
9. Compile + Save asset.

## Ability Tree Template Notes

Ability trees differ:
- **10 nodes × 3 ranks = 30 ranks total per tree**
- **Cluster shape** (visualized in WBP_SkillTreeScreen) — center node is `bIsBaseAbility = true`, granted free
- **MaxInvestedInTree = 15** (so you can't max out every node)
- Use stepped values: each rank of a node contributes the same amount (e.g. node says "+5% damage" — rank 1 = +5%, rank 2 = +10% cumulative, rank 3 = +15% cumulative). Implement this by putting the same value in each `ModifiersPerRank` entry.

## Estimated Authoring Time

- 1 passive tree = ~1 hour to fully spec + enter in editor
- 1 ability tree = ~30 min (smaller, more rank repetition)
- Per class: 1 passive + 5 ability = ~3.5 hours
- All 4 classes: ~14 hours

W6 has Mon-Sun to author. Should fit comfortably.

## Balance Targets (Demo-Tuned)

- Passive tree fully invested (20 points = level 20) should grant roughly:
  - +200 MaxHealth (so a Warrior at level 20 has ~350 HP base + passive = ~550 from this tree alone)
  - +50% PhysicalPower
  - +15% AttackSpeed
  - +50 Armor (capped at 100)
  - +15% CritChance
  - +35% CritDamage
- This is intentional — Warrior at max passive should feel meaningfully stronger than at level 1. Items + cards layer on top.
- Validate during W8 combat tuning playtest.

## Quick Reference — All Stats Available

Available `EOryxStat` values (23 total per Phase 4 expansion):
- **HP/MP/Stamina**: MaxHealth, MaxMana, MaxStamina, HealthRegen, ManaRegen, StaminaRegen
- **Damage**: Damage (global), PhysicalPower, FireDamage, ColdDamage, MagicDamage, CurseDamage, PoisonDamage
- **Combat tempo**: AttackSpeed, CastSpeed, CooldownReduction
- **Crits**: CritChance, CritDamage
- **Defense**: Armor, DamageReduction
- **Misc**: MovementSpeed, Luck, GoldGain
