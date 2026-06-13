# Archer Skill Trees — FULL AUTHORING SPEC (Demo)

> 2026-06-13. Framework rules, point economy, authoring workflow: see `SkillTree_Warrior_Full.md`
> (lead doc). All names/values placeholder per the progression brief.
>
> **Theme:** mobile ranged damage, precision, positioning.
> **Abilities:** Piercing Shot / Volley Rain / Dashstep / Poison Arrow / Falcon Mark.
> **Subclasses (Mastery @ 20, post-demo, per mockup):** Ranger / Shadow Hunter.
> **Class branch names (per mockup):** Marksmanship / Skirmisher / Survival / Utility.
>
> Gameplay note: Archer's ability BPs (projectile-based) are gated on the projectile system —
> trees are authorable NOW (pure stats apply globally regardless).

## Asset layout

```
/Game/SkillTrees/Archer/
├── DA_SkillTree_Archer_Class         (Passive)
├── DA_SkillTree_Archer_PiercingShot  (Ability)
├── DA_SkillTree_Archer_VolleyRain    (Ability)
├── DA_SkillTree_Archer_Dashstep      (Ability)
├── DA_SkillTree_Archer_PoisonArrow   (Ability)
└── DA_SkillTree_Archer_FalconMark    (Ability)
```
⚠️ Register all 6 in `BP_OryxGameInstance → Registered Skill Trees`.

---

# 1. CLASS TREE — `DA_SkillTree_Archer_Class`

TreeType **Passive** · 24 nodes · gates T1=0, T2=4, T3=8, Key=12.

### MARKSMANSHIP — "Improves your damage with bows and arrows"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_Class_Mark_01` | Steady Hand | 3 | CritChance +0.03 | 0 | — |
| `Arc_Class_Mark_02` | Sharpened Tips | 3 | PhysicalPower +0.04 | 0 | — |
| `Arc_Class_Mark_03` | Deadeye | 3 | CritDamage +0.08 | 4 | Mark_01 |
| `Arc_Class_Mark_04` | Heavy Draw | 2 | PhysicalPower +0.06 | 4 | Mark_02 |
| `Arc_Class_Mark_05` | Marksman's Focus | 2 | Damage +0.04 | 8 | Mark_03 |
| `Arc_Class_Mark_06` | **KEY: Perfect Shot** | 1 | CritChance +0.05 AND CritDamage +0.15 | 12 | Mark_05 |

### SKIRMISHER — "Improves your mobility and attacking"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_Class_Skirm_01` | Light Feet | 3 | MovementSpeed +15 | 0 | — |
| `Arc_Class_Skirm_02` | Quick Hands | 3 | AttackSpeed +0.08 | 0 | — |
| `Arc_Class_Skirm_03` | Acrobat | 3 | StaminaRegen +3 | 4 | Skirm_01 |
| `Arc_Class_Skirm_04` | Rapid Volley | 2 | AttackSpeed +0.10 | 8 | Skirm_02 |
| `Arc_Class_Skirm_05` | Runner's High | 2 | MaxStamina +12 | 8 | Skirm_03 |
| `Arc_Class_Skirm_06` | **KEY: Windrunner** | 1 | MovementSpeed +25 AND AttackSpeed +0.08 | 12 | Skirm_04 |

### SURVIVAL — "Improves your defenses and sustain"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_Class_Surv_01` | Padded Leathers | 3 | Armor +3 | 0 | — |
| `Arc_Class_Surv_02` | Forager | 3 | MaxHealth +12 | 0 | — |
| `Arc_Class_Surv_03` | Mend | 3 | HealthRegen +1.5 | 4 | Surv_02 |
| `Arc_Class_Surv_04` | Evasive | 2 | DamageReduction +0.03 | 8 | Surv_01 |
| `Arc_Class_Surv_05` | Tough Hide | 2 | Armor +5 | 8 | Surv_01 |
| `Arc_Class_Surv_06` | **KEY: Survivalist** | 1 | MaxHealth +30 AND DR +0.04 | 12 | Surv_03 |

### UTILITY — "Improves your versatility and resourcefulness"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_Class_Util_01` | Scavenger | 3 | GoldGain +0.08 | 0 | — |
| `Arc_Class_Util_02` | Quiver Discipline | 3 | CooldownReduction +0.04 | 0 | — |
| `Arc_Class_Util_03` | Keen Eye | 3 | Luck +0.5 | 4 | Util_01 |
| `Arc_Class_Util_04` | Field Tactician | 2 | CooldownReduction +0.05 | 8 | Util_02 |
| `Arc_Class_Util_05` | Deep Quiver | 2 | MaxStamina +10 | 8 | Util_02 |
| `Arc_Class_Util_06` | **KEY: Hawk's Patience** | 1 | CDR +0.05 AND CritChance +0.04 | 12 | Util_04 |

---

# 2. ABILITY TREES (1 free center + 9 × 3 ranks · gates 0/3/6 · MaxInvested 15)

### 2a. `DA_SkillTree_Archer_PiercingShot` — "Piercing Shot" (Basic Attack)
*Post-funding themes: extra pierces, projectile speed, armor penetration.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_PS_00_Ability` | Piercing Shot | 1 | (bIsBaseAbility ✓) | — | — |
| `Arc_PS_01_PhysPower` | Honed Heads | 3 | PhysicalPower +0.05 | 0 | — |
| `Arc_PS_02_AtkSpd` | Snap Shot | 3 | AttackSpeed +0.09 | 0 | — |
| `Arc_PS_03_CritChance` | Threading the Line | 3 | CritChance +0.04 | 3 | PS_02 |
| `Arc_PS_04_MoveSpeed` | Longstride | 3 | MovementSpeed +8 | 0 | — |
| `Arc_PS_05_StamRegen` | Featherweight | 3 | StaminaRegen +2 | 3 | PS_04 |
| `Arc_PS_06_Armor` | Guarded Stance | 3 | Armor +2 | 3 | — |
| `Arc_PS_07_CritDmg` | Heartpiercer | 3 | CritDamage +0.10 | 6 | PS_03 |
| `Arc_PS_08_Damage` | Followthrough | 3 | Damage +0.04 | 6 | PS_01 |
| `Arc_PS_09_Apex` | Skewer | 3 | PhysPower +0.04 AND CritChance +0.03 | 6 | PS_08 |

### 2b. `DA_SkillTree_Archer_VolleyRain` — "Volley Rain" (AoE)
*Post-funding themes: area size, additional waves, bleed, slow.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_VR_00_Ability` | Volley Rain | 1 | (bIsBaseAbility ✓) | — | — |
| `Arc_VR_01_Damage` | Saturation | 3 | Damage +0.05 | 0 | — |
| `Arc_VR_02_PhysPower` | Barbed Tips | 3 | PhysicalPower +0.05 | 0 | — |
| `Arc_VR_03_CDR` | Storm Caller | 3 | CooldownReduction +0.04 | 3 | VR_01 |
| `Arc_VR_04_CritDmg` | High Arc | 3 | CritDamage +0.08 | 3 | VR_02 |
| `Arc_VR_05_Stamina` | Supply Lines | 3 | MaxStamina +8 | 0 | — |
| `Arc_VR_06_MoveSpeed` | Rain Dancer | 3 | MovementSpeed +8 | 3 | VR_05 |
| `Arc_VR_07_CritChance` | Splinter | 3 | CritChance +0.03 | 6 | VR_04 |
| `Arc_VR_08_DR` | Suppression | 3 | DamageReduction +0.02 | 6 | VR_05 |
| `Arc_VR_09_Apex` | Monsoon | 3 | Damage +0.04 AND CDR +0.03 | 6 | VR_03 |

### 2c. `DA_SkillTree_Archer_Dashstep` — "Dashstep" (Mobility)
*Post-funding themes: multiple charges, projectile dodge i-frames, distance.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_DS_00_Ability` | Dashstep | 1 | (bIsBaseAbility ✓) | — | — |
| `Arc_DS_01_MoveSpeed` | Spring-Loaded | 3 | MovementSpeed +12 | 0 | — |
| `Arc_DS_02_Stamina` | Cardio | 3 | MaxStamina +10 | 0 | — |
| `Arc_DS_03_StamRegen` | Recovery Roll | 3 | StaminaRegen +3 | 3 | DS_02 |
| `Arc_DS_04_AtkSpd` | Slipstream | 3 | AttackSpeed +0.06 | 3 | DS_01 |
| `Arc_DS_05_DR` | Phantom Step | 3 | DamageReduction +0.02 | 3 | — |
| `Arc_DS_06_Damage` | Momentum | 3 | Damage +0.03 | 6 | DS_04 |
| `Arc_DS_07_HealthRegen` | Breather | 3 | HealthRegen +1 | 6 | DS_03 |
| `Arc_DS_08_CDR` | Free Runner | 3 | CooldownReduction +0.04 | 6 | DS_01 |
| `Arc_DS_09_Apex` | Untouchable | 3 | MovementSpeed +10 AND DR +0.02 | 6 | DS_05 |

### 2d. `DA_SkillTree_Archer_PoisonArrow` — "Poison Arrow" (DoT)
*Post-funding themes: poison duration, stacks, spread on kill. Synergy: D22 Plague (Poison+Curse).*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_PA_00_Ability` | Poison Arrow | 1 | (bIsBaseAbility ✓) | — | — |
| `Arc_PA_01_Poison` | Virulence | 3 | PoisonDamage +0.08 | 0 | — |
| `Arc_PA_02_PhysPower` | Coated Broadheads | 3 | PhysicalPower +0.04 | 0 | — |
| `Arc_PA_03_Poison2` | Lingering Toxin | 3 | PoisonDamage +0.10 | 3 | PA_01 |
| `Arc_PA_04_CritChance` | Septic Strike | 3 | CritChance +0.03 | 3 | PA_02 |
| `Arc_PA_05_Stamina` | Alchemist's Stock | 3 | MaxStamina +8 | 0 | — |
| `Arc_PA_06_Damage` | Plague Bearer | 3 | Damage +0.04 | 6 | PA_03 |
| `Arc_PA_07_CritDmg` | Corrosion | 3 | CritDamage +0.08 | 6 | PA_04 |
| `Arc_PA_08_HealthRegen` | Antitoxin | 3 | HealthRegen +1.5 | 3 | — |
| `Arc_PA_09_Apex` | Pandemic | 3 | PoisonDamage +0.08 AND Damage +0.03 | 6 | PA_06 |

### 2e. `DA_SkillTree_Archer_FalconMark` — "Falcon Mark" (Utility)
*Post-funding themes: mark duration, damage amp, spread on kill, boss damage.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Arc_FM_00_Ability` | Falcon Mark | 1 | (bIsBaseAbility ✓) | — | — |
| `Arc_FM_01_Damage` | Marked Prey | 3 | Damage +0.05 | 0 | — |
| `Arc_FM_02_CritChance` | Predator's Eye | 3 | CritChance +0.04 | 0 | — |
| `Arc_FM_03_CritDmg` | Exposed Weakness | 3 | CritDamage +0.10 | 3 | FM_02 |
| `Arc_FM_04_CDR` | Hunting Party | 3 | CooldownReduction +0.04 | 3 | FM_01 |
| `Arc_FM_05_Luck` | Falconer's Bond | 3 | Luck +0.5 | 0 | — |
| `Arc_FM_06_GoldGain` | Bounty | 3 | GoldGain +0.10 | 3 | FM_05 |
| `Arc_FM_07_PhysPower` | Apex Predator | 3 | PhysicalPower +0.04 | 6 | FM_01 |
| `Arc_FM_08_Stamina` | Patient Hunter | 3 | MaxStamina +8 | 6 | — |
| `Arc_FM_09_Apex` | Death Sentence | 3 | Damage +0.04 AND CritDamage +0.08 | 6 | FM_03 |

---

# 3. Class balance notes

Archer base: 80 HP / 550 walk. Glass — Survival branch is its lifeline; Marksmanship+Skirmisher
full-stack: AttackSpeed +0.54 class +0.27 ability ≈ 1.8 total (cap 5 ✓), CritChance +0.14/+0.12.
PoisonDamage stacks (PA tree +0.54 deep) feed the D22 Plague combo with Curse sources later.
