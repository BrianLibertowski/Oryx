# Assassin Skill Trees — FULL AUTHORING SPEC (Demo)

> 2026-06-13. Framework rules, point economy, authoring workflow: see `SkillTree_Warrior_Full.md`.
> All names/values placeholder per the progression brief.
>
> **Theme:** fast melee burst, stealth, positioning, precision.
> **Abilities:** Shadow Strike / Smoke Veil / Backstab / Shadow Dash / Fan of Blades.
> **Subclasses (Mastery @ 20, post-demo, per mockup):** Blade Dancer / Shadowblade / Trickster
> — NOTE: mockup shows THREE masteries (D27 said two); roster TBD at subclass design time.
> **Class branch names (per mockup):** Agility / Deception / Lethality / Utility.

## Asset layout

```
/Game/SkillTrees/Assassin/
├── DA_SkillTree_Assassin_Class        (Passive)
├── DA_SkillTree_Assassin_ShadowStrike (Ability)
├── DA_SkillTree_Assassin_SmokeVeil    (Ability)
├── DA_SkillTree_Assassin_Backstab     (Ability)
├── DA_SkillTree_Assassin_ShadowDash   (Ability)
└── DA_SkillTree_Assassin_FanOfBlades  (Ability)
```
⚠️ Register all 6 in `BP_OryxGameInstance → Registered Skill Trees`.

---

# 1. CLASS TREE — `DA_SkillTree_Assassin_Class`

TreeType **Passive** · 24 nodes · gates T1=0, T2=4, T3=8, Key=12.

### AGILITY — "Improve your attacks, critical strikes and combat prowess"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_Class_Agi_01` | Fleet | 3 | MovementSpeed +18 | 0 | — |
| `Asn_Class_Agi_02` | Flurry | 3 | AttackSpeed +0.09 | 0 | — |
| `Asn_Class_Agi_03` | Nimble | 3 | StaminaRegen +3 | 4 | Agi_01 |
| `Asn_Class_Agi_04` | Blade Tempo | 2 | AttackSpeed +0.11 | 8 | Agi_02 |
| `Asn_Class_Agi_05` | Ghostsprint | 2 | MovementSpeed +15 | 8 | Agi_01 |
| `Asn_Class_Agi_06` | **KEY: Dancer's Edge** | 1 | AttackSpeed +0.10 AND MovementSpeed +20 | 12 | Agi_04 |

### DECEPTION — "Mislead your enemies and amplify your overwhelming strikes"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_Class_Dec_01` | Misdirect | 3 | DamageReduction +0.02 | 0 | — |
| `Asn_Class_Dec_02` | Slippery | 3 | MovementSpeed +10 | 0 | — |
| `Asn_Class_Dec_03` | Vanish-Ready | 3 | CooldownReduction +0.04 | 4 | Dec_01 |
| `Asn_Class_Dec_04` | Opportunist | 2 | Damage +0.05 | 8 | Dec_03 |
| `Asn_Class_Dec_05` | Shadow Veil | 2 | DamageReduction +0.03 | 8 | Dec_01 |
| `Asn_Class_Dec_06` | **KEY: Phantom** | 1 | CDR +0.06 AND DR +0.04 | 12 | Dec_04 |

### LETHALITY — "Focus your training on deadly precision and execution"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_Class_Leth_01` | Honed Daggers | 3 | CritChance +0.04 | 0 | — |
| `Asn_Class_Leth_02` | Vital Knowledge | 3 | CritDamage +0.10 | 0 | — |
| `Asn_Class_Leth_03` | Serrated | 3 | PhysicalPower +0.04 | 4 | Leth_01 |
| `Asn_Class_Leth_04` | Artery Seeker | 2 | CritChance +0.05 | 8 | Leth_01 |
| `Asn_Class_Leth_05` | Killer Instinct | 2 | CritDamage +0.14 | 8 | Leth_02 |
| `Asn_Class_Leth_06` | **KEY: Deathmark** | 1 | CritChance +0.06 AND CritDamage +0.20 | 12 | Leth_05 |

### UTILITY — "Enhance your mobility, survivability and resourcefulness"

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_Class_Util_01` | Pickpocket | 3 | GoldGain +0.10 | 0 | — |
| `Asn_Class_Util_02` | Conditioning | 3 | MaxStamina +10 | 0 | — |
| `Asn_Class_Util_03` | Lucky Strike | 3 | Luck +0.5 | 4 | Util_01 |
| `Asn_Class_Util_04` | Efficient Killer | 2 | CooldownReduction +0.05 | 8 | Util_02 |
| `Asn_Class_Util_05` | Blood Price | 2 | MaxHealth +15 | 8 | Util_02 |
| `Asn_Class_Util_06` | **KEY: One Step Ahead** | 1 | CDR +0.05 AND MovementSpeed +15 | 12 | Util_04 |

---

# 2. ABILITY TREES (1 free center + 9 × 3 ranks · gates 0/3/6 · MaxInvested 15)

### 2a. `DA_SkillTree_Assassin_ShadowStrike` — "Shadow Strike" (Basic Attack)
*Post-funding themes: combo length, bleed, shadow clones.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_SS_00_Ability` | Shadow Strike | 1 | (bIsBaseAbility ✓) | — | — |
| `Asn_SS_01_AtkSpd` | Twin Fangs | 3 | AttackSpeed +0.10 | 0 | — |
| `Asn_SS_02_CritChance` | Razor's Edge | 3 | CritChance +0.04 | 0 | — |
| `Asn_SS_03_PhysPower` | Deep Cuts | 3 | PhysicalPower +0.05 | 3 | SS_01 |
| `Asn_SS_04_MoveSpeed` | Juggler | 3 | MovementSpeed +8 | 0 | — |
| `Asn_SS_05_StamRegen` | Flow State | 3 | StaminaRegen +2 | 3 | SS_04 |
| `Asn_SS_06_CritDmg` | Cruelty | 3 | CritDamage +0.10 | 6 | SS_02 |
| `Asn_SS_07_Damage` | Relent | 3 | Damage +0.04 | 6 | SS_03 |
| `Asn_SS_08_Armor` | Light Armor | 3 | Armor +2 | 3 | — |
| `Asn_SS_09_Apex` | Thousand Cuts | 3 | AttackSpeed +0.08 AND CritChance +0.03 | 6 | SS_06 |

### 2b. `DA_SkillTree_Assassin_SmokeVeil` — "Smoke Veil" (Utility/Stealth)
*Post-funding themes: invis duration, aggro drop, bonus damage after stealth.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_SV_00_Ability` | Smoke Veil | 1 | (bIsBaseAbility ✓) | — | — |
| `Asn_SV_01_DR` | Thick Smoke | 3 | DamageReduction +0.03 | 0 | — |
| `Asn_SV_02_MoveSpeed` | Quick Escape | 3 | MovementSpeed +12 | 0 | — |
| `Asn_SV_03_HealthRegen` | Catch Breath | 3 | HealthRegen +1.5 | 3 | SV_01 |
| `Asn_SV_04_CDR` | Re-position | 3 | CooldownReduction +0.05 | 3 | SV_02 |
| `Asn_SV_05_Damage` | Ambusher | 3 | Damage +0.04 | 6 | SV_04 |
| `Asn_SV_06_CritDmg` | Cold Blood | 3 | CritDamage +0.08 | 6 | SV_05 |
| `Asn_SV_07_Stamina` | Endurance | 3 | MaxStamina +10 | 0 | — |
| `Asn_SV_08_Luck` | Shadow's Favor | 3 | Luck +0.5 | 3 | — |
| `Asn_SV_09_Apex` | Ghost | 3 | DR +0.03 AND MovementSpeed +10 | 6 | SV_03 |

### 2c. `DA_SkillTree_Assassin_Backstab` — "Backstab" (Burst)
*Post-funding themes: positional bonus, execute, weakness application.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_BS_00_Ability` | Backstab | 1 | (bIsBaseAbility ✓) | — | — |
| `Asn_BS_01_CritDmg` | Anatomy | 3 | CritDamage +0.14 | 0 | — |
| `Asn_BS_02_MoveSpeed` | Silent Approach | 3 | MovementSpeed +8 | 0 | — |
| `Asn_BS_03_CritChance` | Vital Point | 3 | CritChance +0.04 | 3 | BS_01 |
| `Asn_BS_04_PhysPower` | Twist the Knife | 3 | PhysicalPower +0.05 | 3 | BS_01 |
| `Asn_BS_05_Damage` | Executioner | 3 | Damage +0.05 | 6 | BS_04 |
| `Asn_BS_06_CDR` | Calculated | 3 | CooldownReduction +0.04 | 3 | BS_02 |
| `Asn_BS_07_StamRegen` | Adrenaline Rush | 3 | StaminaRegen +2 | 6 | BS_06 |
| `Asn_BS_08_Armor` | Exploit | 3 | Armor +2 | 0 | — |
| `Asn_BS_09_Apex` | Assassinate | 3 | CritDamage +0.12 AND CritChance +0.03 | 6 | BS_03 |

### 2d. `DA_SkillTree_Assassin_ShadowDash` — "Shadow Dash" (Mobility)
*Post-funding themes: multiple charges, pass-through damage, shadow explosion, energy refund.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_SD_00_Ability` | Shadow Dash | 1 | (bIsBaseAbility ✓) | — | — |
| `Asn_SD_01_MoveSpeed` | Blink Step | 3 | MovementSpeed +12 | 0 | — |
| `Asn_SD_02_Stamina` | Reserves | 3 | MaxStamina +10 | 0 | — |
| `Asn_SD_03_StamRegen` | Swift Recovery | 3 | StaminaRegen +3 | 3 | SD_02 |
| `Asn_SD_04_Damage` | Cutting Path | 3 | Damage +0.04 | 3 | SD_01 |
| `Asn_SD_05_DR` | Shadow Trail | 3 | DamageReduction +0.02 | 3 | — |
| `Asn_SD_06_CDR` | Chain Dash | 3 | CooldownReduction +0.05 | 6 | SD_03 |
| `Asn_SD_07_AtkSpd` | Dash Momentum | 3 | AttackSpeed +0.06 | 6 | SD_04 |
| `Asn_SD_08_Armor` | Phase Skin | 3 | Armor +2 | 6 | SD_05 |
| `Asn_SD_09_Apex` | Shadowmeld | 3 | MovementSpeed +10 AND CDR +0.03 | 6 | SD_06 |

### 2e. `DA_SkillTree_Assassin_FanOfBlades` — "Fan of Blades" (AoE)
*Post-funding themes: projectile count, radius, returning blades. Synergy: D22 Plague via Poison.*

| NodeId | Name | Ranks | Per-rank | Gate | Req |
|---|---|---|---|---|---|
| `Asn_FB_00_Ability` | Fan of Blades | 1 | (bIsBaseAbility ✓) | — | — |
| `Asn_FB_01_Damage` | Wide Fan | 3 | Damage +0.05 | 0 | — |
| `Asn_FB_02_Poison` | Envenomed | 3 | PoisonDamage +0.08 | 0 | — |
| `Asn_FB_03_CritChance` | Balanced Blades | 3 | CritChance +0.03 | 3 | FB_01 |
| `Asn_FB_04_PhysPower` | Hidden Stock | 3 | PhysicalPower +0.04 | 0 | — |
| `Asn_FB_05_Poison2` | Toxin Coating | 3 | PoisonDamage +0.10 | 3 | FB_02 |
| `Asn_FB_06_AtkSpd` | Bladestorm | 3 | AttackSpeed +0.06 | 6 | FB_03 |
| `Asn_FB_07_CritDmg` | Reaper's Harvest | 3 | CritDamage +0.08 | 6 | FB_03 |
| `Asn_FB_08_CDR` | Quick Reload | 3 | CooldownReduction +0.04 | 3 | — |
| `Asn_FB_09_Apex` | Razor Hurricane | 3 | Damage +0.04 AND PoisonDamage +0.06 | 6 | FB_05 |

---

# 3. Class balance notes

Assassin base: 90 HP / 600 walk — fastest, squishy. Identity = crit ceiling: Lethality branch full
(+0.15 CC / +0.48 CD) + Backstab deep (+0.12 CC apex-inclusive / +0.54 CD) is the highest crit
stack in the game by design — D1 crit overflow makes >100% CritChance productive. Survivability is
the deliberate weakness; Deception's DR line is the only mitigation.
