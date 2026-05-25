# ORYX — Asset Replacement Spec

Single source of truth for which in-engine placeholder gets swapped out by which outsource discipline. Fill the **Status** column as outsource deliveries land.

## Player Characters

| Path | Current (Placeholder) | Replaces With | Owner | Status |
|---|---|---|---|---|
| `SK_Mannequin` (Engine default) | UE5 Mannequin skeletal mesh | Custom Warrior/Archer/Assassin/Mage SK_*  | 3D character artist | Pending |
| `BP_Oryx_Warrior` Mesh | shared Mannequin + procedural color | SK_Player_Warrior (own skeleton or retargeted) | 3D character artist | Pending |
| `BP_Oryx_Archer` Mesh | shared Mannequin | SK_Player_Archer | 3D character artist | Pending |
| `BP_Oryx_Assassin` Mesh | shared Mannequin | SK_Player_Assassin | 3D character artist | Pending |
| `BP_Oryx_Mage` Mesh | shared Mannequin | SK_Player_Mage | 3D character artist | Pending |

## Weapons

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `Content/Meshes/Weapons/Placeholders/SM_Sword` | Modeling-Mode box+cube | SM_Weapon_Sword_Warrior | 3D character artist (or weapon-specialist) | Pending |
| `Content/Meshes/Weapons/Placeholders/SM_Bow` | curved cylinder | SM_Weapon_Bow_Archer | 3D character artist | Pending |
| `Content/Meshes/Weapons/Placeholders/SM_Dagger` | short box | SM_Weapon_Dagger_Assassin | 3D character artist | Pending |
| `Content/Meshes/Weapons/Placeholders/SM_Staff` | long thin cylinder + orb | SM_Weapon_Staff_Mage | 3D character artist | Pending |

## Enemies

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `BP_Enemy_MossyGoblin` Mesh | tinted Mannequin (green) | SK_Enemy_MossyGoblin (Grunt) | 3D character artist | Pending |
| `BP_Enemy_SwiftSprout` Mesh | tinted Mannequin (light green) | SK_Enemy_SwiftSprout (Runner) | 3D character artist | Pending |
| `BP_Enemy_SlimeBlob` Mesh | tinted Mannequin (blue-green) | SK_Enemy_SlimeBlob (Tank) | 3D character artist | Pending |
| `BP_Boss_MegaGrizzlebark` Mesh | scaled Mannequin | SK_Boss_MegaGrizzlebark | 3D character artist | Pending |

## Static Meshes (World Placement)

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `BP_Vendor` Mesh | placeholder cube | SM_Vendor_Stand | Environment artist | Pending |
| `BP_Totem` Mesh Easy/Med/Hard | 3 capsule sizes (Modeling Mode) | SM_Totem_Easy/Med/Hard | Environment artist | Pending |
| `BP_Chest` Mesh | crate placeholder | SM_Chest_Closed + SM_Chest_Open | Environment artist | Pending |
| Boss arena props | none | SM_BossArena_Pillar / SM_BossArena_Throne | Environment artist | Pending |

## Animation

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `AM_Attack_Light_*` (per class) | empty montage scaffold w/ OnHitFrame notify | Real swing/cast animation | Animator | Pending |
| `AM_HitReact_Light` | empty | Player + enemy hit react | Animator | Pending |
| `AM_Death` (per skeleton) | falls back to capsule disable | Real death anim | Animator | Pending |
| Boss telegraph windup anim | none — VFX-only currently | AM_Boss_HeavyTelegraph | Animator | Pending |
| Idle / Walk / Run / Sprint / Jump | Engine default | Custom per class | Animator | Pending |

## VFX (Niagara)

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `NS_HitImpact_Physical/Fire/Cold/Magic/Curse/Poison` | placeholder color-coded burst | Final stylized VFX per type | VFX artist | Pending |
| Ability VFX (16 abilities, 4 per class) | inherited from BP_Ability_AoEBurst variants | Final per-ability art | VFX artist | Pending |
| Status effect overlays (Burn/Chill/Poison loop) | placeholder color tint + sparkles | Final looped FX | VFX artist | Pending |
| Boss telegraph VFX | none | Charge-up FX matching anim | VFX artist | Pending |
| Pickup VFX (gold + item) | spinning mesh only | Glow + collect burst | VFX artist | Pending |
| Level-up VFX | none | Class-colored burst | VFX artist | Pending |

## Audio

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| Combat SFX (swings, impacts, deaths) | placeholder Engine sounds | Final SFX library | Sound designer | Pending |
| UI SFX (buttons, vendor, card pick) | placeholder | Final UI sound set | Sound designer | Pending |
| World SFX (pickups, chests, totems) | placeholder | Final world SFX | Sound designer | Pending |
| Music — biome ambient loop | royalty-free placeholder | Original score | Composer | Pending |
| Music — combat layer | royalty-free placeholder | Original score (instrumentation overlay) | Composer | Pending |
| Music — boss theme | royalty-free placeholder | Original score | Composer | Pending |

## UI

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `WBP_MainMenu` background | gradient | Stylized arcana-themed background | UI artist | Pending |
| `WBP_PlayerHUD` radial bars | M_RadialBar_Combined material | Stylized health/mana/stamina rings | UI artist | Pending |
| `WBP_VendorScreen` row layout | system font + plain buttons | Themed list with rarity-colored frames | UI artist | Pending |
| `WBP_InventoryIcon` slot frame | bare Image widget | Item-rarity bordered frame | UI artist | Pending |
| `WBP_CardPickerScreen` card frames | TBD (built W4) | Card-art-style frames | UI artist | Pending |
| `WBP_SkillTreeScreen` node art | TBD (built W6) | Skill-node icons + branch art | UI artist | Pending |
| Item icons (32 textures) | royalty-free icon set (Items.md note) | Custom icons matching art direction | UI artist (or icon-specialist) | Pending |

## Environment / Map

| Path | Current | Replaces With | Owner | Status |
|---|---|---|---|---|
| `Content/Maps/Blockout` | greybox prototyping | `Content/Maps/WildPlains_Final` | Environment artist | In Progress |
| Landscape material | Engine default grass | Stylized Wild Plains palette | Environment artist | Pending |
| Foliage | none | Tree/grass/rock instances | Environment artist | Pending |
| Lighting | dynamic placeholder | Baked + dynamic blend, time-of-day fixed | Environment artist | Pending |

---

**Update process:** when an outsource delivery lands, change Status → Received, swap the reference in the relevant BP/Asset, smoke-test in PIE, then mark → Integrated.
