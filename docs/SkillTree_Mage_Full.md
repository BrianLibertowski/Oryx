# Mage Skill Trees — FULL AUTHORING SPEC (Demo)

> 2026-06-13. Framework rules, point economy, authoring workflow: see `SkillTree_Warrior_Full.md`.
> All names/values placeholder per the progression brief.
>
> **Theme:** elemental spellcaster, ranged magic, battlefield control.
> **Abilities:** Arcane Bolt / Frost Nova / Fireball / Lightning Storm / Blink.
> **Subclasses (Mastery @ 20, post-demo, per mockup):** Sorcerer / Spellblade / Runemaster
> — NOTE: three masteries shown (D27 said two); roster TBD at subclass design time.
> **Class branch names (per mockup):** Elemental / Arcane / Defense / Utility.
>
> Stat mapping notes: "Lightning" has no dedicated stat — Lightning Storm scales via **MagicDamage**.
> Frost Nova/Fireball feed the D22 combos (Burn+Chill=Shatter) once status application lands.

## Asset layout

```
/Game/SkillTrees/Mage/
├── DA_SkillTree_Mage_Class          (Passive)
├── DA_SkillTree_Mage_ArcaneBolt     (Ability)
├── DA_SkillTree_Mage_FrostNova      (Ability)
├── DA_SkillTree_Mage_Fireball       (Ability)
├── DA_SkillTree_Mage_LightningStorm (Ability)
└── DA_SkillTree_Mage_Blink          (Ability)
```
⚠️ Register all 6 in `BP_OryxGameInstance → Registered Skill Trees`.

---

# 1. CLASS TREE — `DA_SkillTree_Mage_Class`

TreeType **Passive** · 24 nodes · gates T1=0, T2=4, T3=8, Key=12.

### ELEMENTAL — "Harness the primal elements and amplify your spell damage"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_Class_Ele_01` | Kindling | 3 | FireDamage +0.05 | 0 | — |
| `Mag_Class_Ele_02` | Frostbite | 3 | ColdDamage +0.05 | 0 | — |
| `Mag_Class_Ele_03` | Attunement | 3 | MagicDamage +0.05 | 4 | — |
| `Mag_Class_Ele_04` | Pyromania | 2 | FireDamage +0.08 | 8 | Ele_01 |
| `Mag_Class_Ele_05` | Deep Freeze | 2 | ColdDamage +0.08 | 8 | Ele_02 |
| `Mag_Class_Ele_06` | **KEY: Elemental Overload** | 1 | FireDamage +0.08 AND ColdDamage +0.08 AND MagicDamage +0.08 | 12 | Ele_03 |

### ARCANE — "Refine your mana and manipulate magical energies"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_Class_Arc_01` | Mana Well | 3 | MaxMana +12 | 0 | — |
| `Mag_Class_Arc_02` | Swift Incantation | 3 | CastSpeed +0.06 | 0 | — |
| `Mag_Class_Arc_03` | Flow | 3 | ManaRegen +2 | 4 | Arc_01 |
| `Mag_Class_Arc_04` | Time Bend | 3 | CooldownReduction +0.04 | 4 | Arc_02 |
| `Mag_Class_Arc_05` | Overcharge | 2 | CastSpeed +0.08 | 8 | Arc_02 |
| `Mag_Class_Arc_06` | **KEY: Archmage** | 1 | MaxMana +20 AND CastSpeed +0.08 AND CDR +0.04 | 12 | Arc_05 |

### DEFENSE — "Improve your defenses and survivability on the battlefield"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_Class_Def_01` | Mage Armor | 3 | Armor +3 | 0 | — |
| `Mag_Class_Def_02` | Warded | 3 | MaxHealth +12 | 0 | — |
| `Mag_Class_Def_03` | Barrier Discipline | 3 | DamageReduction +0.02 | 4 | Def_01 |
| `Mag_Class_Def_04` | Stoneskin | 2 | Armor +5 | 8 | Def_01 |
| `Mag_Class_Def_05` | Lifeweave | 2 | HealthRegen +2 | 8 | Def_02 |
| `Mag_Class_Def_06` | **KEY: Aegis** | 1 | Armor +8 AND DR +0.04 | 12 | Def_03 |

### UTILITY — "Enhance your versatility and empower your spells and allies"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_Class_Util_01` | Looter's Cantrip | 3 | GoldGain +0.08 | 0 | — |
| `Mag_Class_Util_02` | Haste | 3 | MovementSpeed +12 | 0 | — |
| `Mag_Class_Util_03` | Fortune | 3 | Luck +0.5 | 4 | Util_01 |
| `Mag_Class_Util_04` | Celerity | 2 | MovementSpeed +15 | 8 | Util_02 |
| `Mag_Class_Util_05` | Efficiency | 2 | CooldownReduction +0.04 | 8 | Util_02 |
| `Mag_Class_Util_06` | **KEY: Chronomancer** | 1 | CDR +0.06 AND CastSpeed +0.06 | 12 | Util_05 |

---

# 2. ABILITY TREES (1 free center + 9 × 3 ranks · gates 0/3/6 · MaxInvested 15)

### 2a. `DA_SkillTree_Mage_ArcaneBolt` — "Arcane Bolt" (Basic Attack)
*Post-funding themes: projectile count, chain, arcane explosion, mana refund.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_AB_00_Ability` | Arcane Bolt | 1 | (bIsBaseAbility ✓) | — | — |
| `Mag_AB_01_Magic` | Focused Bolt | 3 | MagicDamage +0.07 | 0 | — |
| `Mag_AB_02_CastSpd` | Quick Cast | 3 | CastSpeed +0.07 | 0 | — |
| `Mag_AB_03_CritChance` | Arcane Precision | 3 | CritChance +0.04 | 3 | AB_01 |
| `Mag_AB_04_ManaRegen` | Mana Cycle | 3 | ManaRegen +2 | 0 | — |
| `Mag_AB_05_MaxMana` | Deep Reserves | 3 | MaxMana +10 | 3 | AB_04 |
| `Mag_AB_06_CritDmg` | Unstable Energy | 3 | CritDamage +0.10 | 6 | AB_03 |
| `Mag_AB_07_Damage` | Resonance | 3 | Damage +0.04 | 6 | AB_01 |
| `Mag_AB_08_MoveSpeed` | Light Footwork | 3 | MovementSpeed +8 | 3 | — |
| `Mag_AB_09_Apex` | Arcane Mastery | 3 | MagicDamage +0.06 AND CastSpeed +0.05 | 6 | AB_07 |

### 2b. `DA_SkillTree_Mage_FrostNova` — "Frost Nova" (AoE Control)
*Post-funding themes: freeze duration, area size, vulnerability. D22: Chill source for Shatter.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_FN_00_Ability` | Frost Nova | 1 | (bIsBaseAbility ✓) | — | — |
| `Mag_FN_01_Cold` | Biting Cold | 3 | ColdDamage +0.08 | 0 | — |
| `Mag_FN_02_MaxMana` | Frozen Core | 3 | MaxMana +10 | 0 | — |
| `Mag_FN_03_CDR` | Quick Freeze | 3 | CooldownReduction +0.05 | 3 | FN_01 |
| `Mag_FN_04_Armor` | Glacial Armor | 3 | Armor +3 | 3 | FN_02 |
| `Mag_FN_05_Cold2` | Winter's Embrace | 3 | ColdDamage +0.10 | 6 | FN_03 |
| `Mag_FN_06_CastSpd` | Cold Snap | 3 | CastSpeed +0.05 | 3 | — |
| `Mag_FN_07_DR` | Permafrost Skin | 3 | DamageReduction +0.02 | 6 | FN_04 |
| `Mag_FN_08_ManaRegen` | Icevein | 3 | ManaRegen +2 | 0 | — |
| `Mag_FN_09_Apex` | Absolute Zero | 3 | ColdDamage +0.08 AND CDR +0.04 | 6 | FN_05 |

### 2c. `DA_SkillTree_Mage_Fireball` — "Fireball" (Burst)
*Post-funding themes: explosion radius, projectile speed, chain explosions. D22: Burn source.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_FI_00_Ability` | Fireball | 1 | (bIsBaseAbility ✓) | — | — |
| `Mag_FI_01_Fire` | Combustion | 3 | FireDamage +0.08 | 0 | — |
| `Mag_FI_02_CastSpd` | Accelerant | 3 | CastSpeed +0.06 | 0 | — |
| `Mag_FI_03_CritChance` | Critical Burn | 3 | CritChance +0.04 | 3 | FI_01 |
| `Mag_FI_04_Fire2` | Immolation | 3 | FireDamage +0.10 | 6 | FI_03 |
| `Mag_FI_05_Armor` | Heat Shield | 3 | Armor +3 | 3 | — |
| `Mag_FI_06_MaxMana` | Fuel Reserve | 3 | MaxMana +10 | 0 | — |
| `Mag_FI_07_CritDmg` | Detonate | 3 | CritDamage +0.10 | 6 | FI_03 |
| `Mag_FI_08_Damage` | Wildfire | 3 | Damage +0.04 | 3 | FI_01 |
| `Mag_FI_09_Apex` | Inferno | 3 | FireDamage +0.08 AND CritDamage +0.08 | 6 | FI_04 |

### 2d. `DA_SkillTree_Mage_LightningStorm` — "Lightning Storm" (AoE)
*Post-funding themes: strike frequency, area, shock, chains. Scales via MagicDamage (no lightning stat).*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_LS_00_Ability` | Lightning Storm | 1 | (bIsBaseAbility ✓) | — | — |
| `Mag_LS_01_Magic` | Storm Charge | 3 | MagicDamage +0.07 | 0 | — |
| `Mag_LS_02_CritChance` | Conductivity | 3 | CritChance +0.04 | 0 | — |
| `Mag_LS_03_CDR` | Tempest | 3 | CooldownReduction +0.05 | 3 | LS_01 |
| `Mag_LS_04_CastSpd` | Static Field | 3 | CastSpeed +0.06 | 3 | LS_02 |
| `Mag_LS_05_CritDmg` | Overload | 3 | CritDamage +0.10 | 6 | LS_02 |
| `Mag_LS_06_ManaRegen` | Storm's Eye | 3 | ManaRegen +2 | 0 | — |
| `Mag_LS_07_MoveSpeed` | Galvanize | 3 | MovementSpeed +8 | 3 | LS_06 |
| `Mag_LS_08_Magic2` | Thunderhead | 3 | MagicDamage +0.08 | 6 | LS_03 |
| `Mag_LS_09_Apex` | Maelstrom | 3 | MagicDamage +0.06 AND CDR +0.04 | 6 | LS_08 |

### 2e. `DA_SkillTree_Mage_Blink` — "Blink" (Mobility)
*Post-funding themes: distance, charges, defensive barrier, arcane pulse.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Mag_BL_00_Ability` | Blink | 1 | (bIsBaseAbility ✓) | — | — |
| `Mag_BL_01_MoveSpeed` | Phase Distance | 3 | MovementSpeed +10 | 0 | — |
| `Mag_BL_02_MaxMana` | Mana Slip | 3 | MaxMana +10 | 0 | — |
| `Mag_BL_03_CDR` | Rapid Recall | 3 | CooldownReduction +0.06 | 3 | BL_01 |
| `Mag_BL_04_DR` | Displacement | 3 | DamageReduction +0.03 | 3 | BL_01 |
| `Mag_BL_05_Magic` | Arcane Wake | 3 | MagicDamage +0.05 | 3 | BL_02 |
| `Mag_BL_06_ManaRegen` | Blink Discipline | 3 | ManaRegen +2 | 6 | BL_02 |
| `Mag_BL_07_Armor` | Ethereal | 3 | Armor +2 | 6 | BL_04 |
| `Mag_BL_08_CastSpd` | Slipstream Cast | 3 | CastSpeed +0.05 | 6 | BL_05 |
| `Mag_BL_09_Apex` | Master of Space | 3 | MovementSpeed +10 AND CDR +0.04 | 6 | BL_03 |

---

# 3. Class balance notes

Mage base: 80 HP / 450 walk — slowest, squishiest, highest scaling ceiling. Elemental branch splits
Fire/Cold paths feeding the D22 Shatter combo (Burn+Chill) once both statuses can be applied.
CastSpeed currently gates nothing in C++ (AttackSpeed drives basic-attack cooldowns; CastSpeed is a
stat awaiting its consumer) — **wire CastSpeed into spell-type ability cooldowns during P5 ability
work**, or these nodes are decorative-pending. CDR stack worst case: 0.27 class + 0.18 Blink = 0.45
< 0.75 cap ✓. MaxMana/ManaRegen become meaningful as the new abilities get real mana costs.
