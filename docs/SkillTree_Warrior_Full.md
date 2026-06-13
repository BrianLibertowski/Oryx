# Warrior Skill Trees — FULL AUTHORING SPEC (Demo)

> Rev 2 — 2026-06-13 overnight. Ability roster updated to the approved progression brief:
> **Cleave Slash / Shield Rush / Earthshatter / War Cry / Execution Strike.**
> Lead doc for the 4-class set — framework + economy sections here apply to
> `SkillTree_Archer_Full.md`, `SkillTree_Assassin_Full.md`, `SkillTree_Mage_Full.md`.
>
> All names/values are PLACEHOLDER per the progression brief — the deliverable is a clean,
> data-driven framework + complete first-pass content. Balance happens in the W8 tuning pass.

---

## Framework compliance (brief requirement → existing system)

| Brief requirement | Where it lives |
|---|---|
| Data-driven, no hardcoded class behavior | `UOryxSkillTree` DataAssets + `FOryxSkillNode`; classes differ by data only |
| 5 abilities + 1 passive tree + 1 tree per ability | 6 DataAssets per class (1 Passive-type + 5 Ability-type) |
| Passive vs Ability point pools | `FOryxClassProgression` Unallocated counters; tree's `TreeType` selects the pool |
| Node dependencies | `RequiredNodes` (hard, AND-all) + `RequiredPointsInTree` (soft tree-wide gate) |
| Multi-rank nodes | `MaxRanks` + `ModifiersPerRank` array |
| Respec | `RefundAll` — exact per-pool refunds (fixed 2026-06-13) |
| Save progression | `UOryxProfileSave` + rehydration via `PossessedBy → ReapplyAllocations` |
| Expansion without rewrites | Add nodes to an asset / add assets + register in `BP_OryxGameInstance.RegisteredSkillTrees` |

**Point economy:** 1 passive pt/level, 1 ability pt per 2 levels. Demo cap 20 → **20 passive + 10
ability**. Full release cap 60 → 60 + 30 (trees below leave expansion headroom for that).

**Demo constraint:** pure `EOryxStat` modifiers only (D14). Behavior themes from the brief (bleed,
stun, pierce count, stealth duration…) are recorded per tree as the **post-funding upgrade path** —
they need a per-ability parameter system that doesn't exist yet.

**Subclasses (Mastery Choice @ 20, post-demo):** Warrior = **Vanguard / Berserker** per mockup.

---

## Asset layout

```
/Game/SkillTrees/Warrior/
├── DA_SkillTree_Warrior_Class          (TreeType = Passive)
├── DA_SkillTree_Warrior_Cleave         (TreeType = Ability)  ← extends the existing prototype asset
├── DA_SkillTree_Warrior_ShieldRush     (TreeType = Ability)
├── DA_SkillTree_Warrior_Earthshatter   (TreeType = Ability)
├── DA_SkillTree_Warrior_WarCry         (TreeType = Ability)
└── DA_SkillTree_Warrior_Execution      (TreeType = Ability)
```

⚠️ **Every asset must be added to `BP_OryxGameInstance → Registered Skill Trees`** or saved ranks
restore data-only (no stat rehydration).

Common: OwningClass=Warrior. Class tree `MaxInvestedInTree=0`; ability trees `=15`.
Center ability nodes: `bIsBaseAbility ✓`, MaxRanks 1, empty `Ranks`, **never referenced in any
RequiredNodes** (C++ doesn't auto-rank them).

**Node modifier data lives in `Ranks` (multi-stat capable) — see §4 for entry patterns.**

---

# 1. CLASS TREE — `DA_SkillTree_Warrior_Class`

TreeType **Passive** · 24 nodes · 4 branches · gates tree-wide: T1=0, T2=4, T3=8, Keystone=12.

### CORE — "Strength and resilience"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_Class_Core_01` | Brute Force | 3 | PhysicalPower +0.04 | 0 | — |
| `War_Class_Core_02` | Vitality | 3 | MaxHealth +15 | 0 | — |
| `War_Class_Core_03` | Warrior's Might | 3 | Damage +0.03 | 4 | Core_01 |
| `War_Class_Core_04` | Iron Grip | 2 | PhysicalPower +0.06 | 4 | Core_01 |
| `War_Class_Core_05` | Colossus | 2 | MaxHealth +30 | 8 | Core_02 |
| `War_Class_Core_06` | **KEY: Avatar of War** | 1 | PhysPower +0.10 AND Damage +0.05 AND MaxHealth +25 | 12 | Core_04 |

### OFFENSE — "Improve your damage"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_Class_Off_01` | Killing Edge | 3 | CritChance +0.03 | 0 | — |
| `War_Class_Off_02` | Swift Cuts | 3 | AttackSpeed +0.08 | 0 | — |
| `War_Class_Off_03` | Devastate | 3 | CritDamage +0.08 | 4 | Off_01 |
| `War_Class_Off_04` | Frenzy | 2 | AttackSpeed +0.10 | 8 | Off_02 |
| `War_Class_Off_05` | Executioner | 2 | CritDamage +0.12 | 8 | Off_03 |
| `War_Class_Off_06` | **KEY: Bloodlust** | 1 | CritChance +0.05 AND AttackSpeed +0.10 | 12 | Off_04 |

### DEFENSE — "Bolster your survivability"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_Class_Def_01` | Stalwart | 3 | Armor +4 | 0 | — |
| `War_Class_Def_02` | Thick Skin | 3 | DamageReduction +0.02 | 0 | — |
| `War_Class_Def_03` | Second Wind | 3 | HealthRegen +1.5 | 4 | Def_01 |
| `War_Class_Def_04` | Iron Skin | 2 | Armor +6 | 8 | Def_01 |
| `War_Class_Def_05` | Juggernaut | 2 | DamageReduction +0.03 | 8 | Def_02 |
| `War_Class_Def_06` | **KEY: Mountain** | 1 | Armor +10 AND MaxHealth +40 | 12 | Def_04 |

### UTILITY — "Improve your versatility"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_Class_Util_01` | Fleetfoot | 3 | MovementSpeed +15 | 0 | — |
| `War_Class_Util_02` | Battle Rhythm | 3 | CooldownReduction +0.04 | 0 | — |
| `War_Class_Util_03` | Endurance | 3 | StaminaRegen +3 | 4 | Util_01 |
| `War_Class_Util_04` | Tactician | 2 | CooldownReduction +0.05 | 8 | Util_02 |
| `War_Class_Util_05` | Plunderer | 2 | GoldGain +0.10 | 8 | Util_01 |
| `War_Class_Util_06` | **KEY: Warlord's Pace** | 1 | MovementSpeed +25 AND CDR +0.05 | 12 | Util_04 |

---

# 2. ABILITY TREES

Shape per tree: 1 free center + 9 nodes × 3 ranks (27 ranks vs MaxInvested 15 vs 10 demo points).
Gates tree-wide: T1=0, T2=3, T3=6. Apex node = 2 modifiers per rank.

### 2a. `DA_SkillTree_Warrior_Cleave` — "Cleave Slash" (Basic Attack)
*Post-funding themes: width, bleed, armor break, multi-swing, rage generation.*
**Keeps the prototype's existing NodeIds — extend, don't recreate.**

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_Cleave_00_Ability` | Cleave Slash | 1 | (bIsBaseAbility ✓) | — | — |
| `War_Cleave_01_AtkSpd` | Swift Edge | 3 | AttackSpeed +0.10 | 0 | — |
| `War_Cleave_02_PhysPower` | Heavy Hands | 3 | PhysicalPower +0.05 | 0 | — |
| `War_Cleave_03_CritChance` | Opportunist | 3 | CritChance +0.04 | 3 | Cleave_01 |
| `War_Cleave_04_MoveSpeed` | Skirmisher | 3 | MovementSpeed +10 | 0 | — |
| `War_Cleave_05_Stamina` | Conditioning | 3 | MaxStamina +10 | 3 | Cleave_04 |
| `War_Cleave_06_Armor` | Guarded Swings | 3 | Armor +3 | 3 | — |
| `War_Cleave_07_Lifeline` | Lifeline | 3 | HealthRegen +1 | 6 | Cleave_06 |
| `War_Cleave_08_CritDmg` | Butcher | 3 | CritDamage +0.10 | 6 | Cleave_03 |
| `War_Cleave_09_Damage` | Relentless | 3 | Damage +0.04 | 6 | Cleave_02 |

### 2b. `DA_SkillTree_Warrior_ShieldRush` — "Shield Rush" (Mobility)
*Post-funding themes: charge distance, stun, knockback strength, defensive shield.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_SR_00_Ability` | Shield Rush | 1 | (bIsBaseAbility ✓) | — | — |
| `War_SR_01_MoveSpeed` | Charge Momentum | 3 | MovementSpeed +15 | 0 | — |
| `War_SR_02_Stamina` | Deep Lungs | 3 | MaxStamina +12 | 0 | — |
| `War_SR_03_StamRegen` | Recovery | 3 | StaminaRegen +3 | 3 | SR_02 |
| `War_SR_04_Damage` | Impact | 3 | Damage +0.04 | 0 | — |
| `War_SR_05_PhysPower` | Battering Ram | 3 | PhysicalPower +0.05 | 3 | SR_04 |
| `War_SR_06_Armor` | Shield Plating | 3 | Armor +3 | 3 | — |
| `War_SR_07_DR` | Unstoppable | 3 | DamageReduction +0.02 | 6 | SR_06 |
| `War_SR_08_CDR` | Relentless Pursuit | 3 | CooldownReduction +0.04 | 6 | SR_01 |
| `War_SR_09_Apex` | Vanguard's Charge | 3 | MovementSpeed +10 AND Damage +0.03 | 6 | SR_05 |

### 2c. `DA_SkillTree_Warrior_Earthshatter` — "Earthshatter" (AoE)
*Post-funding themes: shockwave size, multiple shockwaves, stun duration, earth cracks.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_ES_00_Ability` | Earthshatter | 1 | (bIsBaseAbility ✓) | — | — |
| `War_ES_01_Damage` | Aftershock | 3 | Damage +0.05 | 0 | — |
| `War_ES_02_PhysPower` | Tremor Force | 3 | PhysicalPower +0.05 | 0 | — |
| `War_ES_03_CritDmg` | Fault Line | 3 | CritDamage +0.10 | 3 | ES_02 |
| `War_ES_04_Armor` | Braced Impact | 3 | Armor +3 | 0 | — |
| `War_ES_05_MaxHealth` | Unshakable | 3 | MaxHealth +15 | 3 | ES_04 |
| `War_ES_06_MoveSpeed` | Quake Step | 3 | MovementSpeed +8 | 3 | — |
| `War_ES_07_CDR` | Echoing Slam | 3 | CooldownReduction +0.04 | 6 | ES_01 |
| `War_ES_08_StamRegen` | Stonebound | 3 | StaminaRegen +2 | 6 | ES_04 |
| `War_ES_09_Apex` | Cataclysm | 3 | Damage +0.04 AND CritDamage +0.08 | 6 | ES_03 |

### 2d. `DA_SkillTree_Warrior_WarCry` — "War Cry" (Buff)
*Post-funding themes: duration, ally support, healing pulse, fury generation.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_WC_00_Ability` | War Cry | 1 | (bIsBaseAbility ✓) | — | — |
| `War_WC_01_Damage` | Rallying Roar | 3 | Damage +0.04 | 0 | — |
| `War_WC_02_AtkSpd` | Bloodcry | 3 | AttackSpeed +0.07 | 3 | WC_01 |
| `War_WC_03_MaxHealth` | Heartening Shout | 3 | MaxHealth +18 | 0 | — |
| `War_WC_04_HealthRegen` | Battle Hymn | 3 | HealthRegen +1.5 | 3 | WC_03 |
| `War_WC_05_DR` | Iron Will | 3 | DamageReduction +0.02 | 6 | WC_04 |
| `War_WC_06_CDR` | Echoing Voice | 3 | CooldownReduction +0.04 | 0 | — |
| `War_WC_07_ManaRegen` | Focused Fury | 3 | ManaRegen +2 | 3 | WC_06 |
| `War_WC_08_MoveSpeed` | Marching Orders | 3 | MovementSpeed +10 | 3 | — |
| `War_WC_09_Apex` | Voice of Command | 3 | Damage +0.03 AND CDR +0.03 | 6 | WC_02 |

### 2e. `DA_SkillTree_Warrior_Execution` — "Execution Strike" (Heavy Attack)
*Post-funding themes: execute threshold, cooldown reset on kill, splash, heal on kill.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `War_EX_00_Ability` | Execution Strike | 1 | (bIsBaseAbility ✓) | — | — |
| `War_EX_01_PhysPower` | Headsman | 3 | PhysicalPower +0.06 | 0 | — |
| `War_EX_02_CritDmg` | Grim Resolve | 3 | CritDamage +0.12 | 0 | — |
| `War_EX_03_Damage` | Killing Blow | 3 | Damage +0.05 | 3 | EX_01 |
| `War_EX_04_AtkSpd` | Butcher's Rhythm | 3 | AttackSpeed +0.05 | 3 | — |
| `War_EX_05_CritChance` | Bloodied Edge | 3 | CritChance +0.03 | 3 | EX_02 |
| `War_EX_06_GoldGain` | Trophy Taker | 3 | GoldGain +0.08 | 6 | — |
| `War_EX_07_CDR` | Relentless | 3 | CooldownReduction +0.03 | 6 | EX_04 |
| `War_EX_08_MaxHealth` | Iron Resolve | 3 | MaxHealth +15 | 0 | — |
| `War_EX_09_Apex` | Reaper | 3 | PhysPower +0.05 AND CritDamage +0.08 | 6 | EX_03 |

---

# 3. Balance audit (worst-case stacking)

| Stat | Class max | Worst ability stack | Cap | Verdict |
|---|---|---|---|---|
| AttackSpeed | +0.54 | +0.30 (Cleave) | 5.0 | ✓ huge headroom |
| CDR | +0.27 | +0.12 | 0.75 | ✓ |
| Armor | +34 | +9-18 | 100 | ✓ watch with items |
| DR | +0.12 | +0.06 | 0.90 soft | ✓ |
| CritChance | +0.14 | +0.12 | uncapped | by design (D1 overflow) |
| MoveSpeed | +95 | +45 | 2000 | ✓ |

Demo points (20+10) reach roughly half these worst cases. Target ≈1.5-2.2× level-1 power at 20
before items.

# 4. Authoring workflow

1. Full rebuild first (rehydration + data-model C++ batch, 2026-06-13).
2. Author class tree → Cleave extension → remaining 4 trees (tables above, top to bottom).
3. Add all 6 to `BP_OryxGameInstance → Registered Skill Trees`.

**Node data entry (the `Ranks` structure — changed 2026-06-13):**
Each node has a **`Ranks`** array (NOT the old `ModifiersPerRank`). Each element = one rank, and
holds a **`Modifiers`** list of all stats that rank grants:
- **Single-stat node, table says "3 ranks, +0.10 AttackSpeed":** set MaxRanks=3, add 3 Ranks
  entries, each entry's Modifiers = `[AttackSpeed Additive 0.10]` (identical — stepped).
- **Keystone, table says "Armor +10 AND MaxHealth +40", 1 rank:** MaxRanks=1, 1 Ranks entry whose
  Modifiers list has BOTH `[Armor Additive 10, MaxHealth Additive 40]`.
- **Apex, "PhysPower +0.04 AND CritDmg +0.06", 3 ranks:** MaxRanks=3, 3 Ranks entries, each entry's
  Modifiers = `[PhysicalPower Additive 0.04, CritDamage Additive 0.06]`.
- **Center ability node:** bIsBaseAbility ✓, MaxRanks 1, **Ranks empty** (free grant).
- Op is `Additive` for all demo nodes. Percentage stats store the fraction (10% → 0.10);
  UOryxStatFormatLibrary renders it as "10%".

4. Test loop: L-key XP → invest → Stats Screen moves → respec returns correct pool → relaunch →
   log shows `reapplied N rank modifier(s)`.
5. **Authoring priority: Warrior first (demo floor), other classes after** — specs in their docs.

> ⚠️ The existing `DA_SkillTree_Warrior_Cleave` prototype node (War_Cleave_01_AtkSpd) was authored
> against the OLD `ModifiersPerRank` field which no longer exists — re-enter its 3 ranks under the
> new `Ranks` structure (1 node, ~1 min).

**UI tip (the payoff for tonight's helper work):** build ONE generic node widget. On construct/refresh
call `SkillTreeComponent → Get Node Display Info (Class, Tree, NodeId)` — the returned struct gives
DisplayName, "2 / 3" rank text, "+10% → +20% Attack Speed" preview, the node State enum (for icon
color: Locked/Available/Partial/Maxed/Granted), bCanInvest (Invest button enabled), and LockReason
(hover tooltip). The widget is pure presentation; one widget renders all ~270 nodes.

# 5. Deferred (post-funding)

Behavior-mod nodes (per-ability parameter system), radial UI (Handoff_UI), respec gold cost,
node search, active-modifiers panel, subclass trees (Vanguard/Berserker), level-60 economy.
