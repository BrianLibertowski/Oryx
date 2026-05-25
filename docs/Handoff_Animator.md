# Handoff Brief — Animator

## Target

Animate the 4 player classes + 3 stage-1 enemies + Mega Grizzlebark boss. Current state: placeholder montages with hit-frame notifies but no actual animation data.

## Player Animation Set (per class × 4 classes)

### Locomotion (shared rig OK)
- [ ] Idle (loop)
- [ ] Walk Forward / Backward / Strafe Left / Strafe Right (loop)
- [ ] Run Forward (loop)
- [ ] Sprint Forward (loop) — distinct from Run, faster cadence
- [ ] Jump Start / Apex / Land
- [ ] Falling (loop, mid-air)
- [ ] Dash Forward — single shot, ~0.3s, root motion or in-place
- [ ] Idle-to-Combat transition (stowed → ready stance)
- [ ] Death (single shot)

### Combat (per class — different weapon stance)
- [ ] Light Attack combo 1 / 2 / 3 (chain into each other; melee classes get 3-hit combos, ranged get single shots)
- [ ] Heavy Attack with windup telegraph (player version of the boss heavy)
- [ ] Hit Reaction Light (0.2s stagger)
- [ ] Hit Reaction Heavy (0.5s stagger)

### Abilities (4 abilities per class × 4 classes = 16 unique anims)

| Class | Slot 1 (Primary) | Slot 2 (Secondary) | Slot 3 (Mobility) | Slot 4 (Ultimate) |
|---|---|---|---|---|
| Warrior | Slash | Whirlwind | Charge | Berserk Roar |
| Archer | Aimed Shot | Volley | Roll | Rain of Arrows |
| Assassin | Backstab | Shadow Strike | Vanish | Death Mark |
| Mage | Bolt | Blast | Blink | Meteor |

Each ability anim must:
- Have an `OnHitFrame` AnimNotify at the damage apex
- Sync to ability cooldown duration (use montage Play Rate for tuning, or anim straight to fit cooldown)

## Enemy Animation Set

### Mossy Goblin (Grunt)
- Idle / Walk / Run / Light Attack (single hit) / Hit React / Death

### Swift Sprout (Runner)
- Idle / Sprint / Quick Attack / Hit React / Death (fast, jerky cadence)

### Slime Blob (Tank)
- Idle (bobbing) / Slow walk / Heavy Slam (telegraphed) / Hit React / Death (squish)

### Mega Grizzlebark (Boss)
- Idle (intimidating breath) / Walk / Run / Light Attack (claw swipe) / **Heavy Attack with 1s telegraph windup** (key boss mechanic — see `AOryxBoss::TelegraphAttack`) / Hit React Heavy / Death (long)
- Optional: phase-shift roar (plays at health thresholds 0.7 / 0.4 / 0.1 per D8)

## Animation Notify Requirements

For montages where damage/SFX needs to sync:
- `OnHitFrame` — fires the BP_Ability_MeleeSlash sphere trace + damage apply
- `OnFootstep_L` / `OnFootstep_R` — footstep sound triggers (sync to anim foot-plants)
- `OnTelegraphPeak` (boss only) — visual cue right before the heavy swing lands

## Retargeting

If you ship custom animations on a custom skeleton, please include:
- IK_Rig asset for source skeleton
- IK_Retargeter asset from source → UE5 Mannequin (or whichever target the project ships)

If shipping retargeted-to-Mannequin animations, drop FBX files and we retarget in-engine.

## Naming Conventions

| Asset | Prefix | Example |
|---|---|---|
| Animation Sequence | A_ | A_Player_Warrior_Idle |
| Animation Montage | AM_ | AM_Player_Warrior_Attack_Light_1 |
| Animation Blueprint | ABP_ | ABP_Player_Warrior |
| Blendspace | BS_ | BS_Player_Locomotion |

## Style Direction

Refs: Hades / Risk of Rain 2 / Sea of Stars combat feel — short, snappy, readable poses. Movement leans toward stylized weight not photorealistic.

- Animation cycles: 1-1.5s loops typical
- Combat anims: 0.4-0.8s for light attacks, 1.0-1.5s for heavies
- Telegraph windups: ~1s for boss heavy (designed for player reaction time)

## Mesh Budget (Demo Stage)

Animation file size targets:
- Per anim: <500 KB compressed
- Total ABP pass per class: <5 MB

## Where Animations Plug In

- Sequences land in `Content/Animations/Player/<Class>/` or `Content/Animations/Enemies/<Type>/`
- Montages land in `Content/Animations/Montages/`
- We assemble the AnimBP state machine — animator can ship the Blendspace + sequences and we wire it

## Deliverable Checklist

- [ ] Per-class locomotion set (4 classes × ~12 anims = 48 anims)
- [ ] Per-class light/heavy + 4 abilities (4 × 6 = 24 anims)
- [ ] Player hit react + death (8 anims, light + heavy + death per class)
- [ ] Enemy anims (3 enemies × ~6 anims = 18 anims)
- [ ] Boss anims (~10 anims including phase-shifts)
- [ ] Optional: combat idle / blocked-attack reaction / parry success
- **Total ~110-120 animations** for full vertical slice

---

**Contact:** [your contact info]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc, AssetReplacementSpec.md
