# Handoff Brief — VFX Artist (Niagara)

## Target

Replace every placeholder Niagara system with stylized final VFX. Current state: color-coded burst placeholders + procedural particles. Tied to gameplay damage types and ability slots.

## Style Direction

Refs: **Risk of Rain 2 / Hades / Ravenswatch** stylized fantasy. NOT photorealistic. Key pillars:

- **Stylised Clarity** — readable silhouettes even in crowd combat (game pillar 1)
- **Strong silhouettes** — VFX shouldn't obscure character poses
- **Color-coded by damage type** — current placeholder palette already in use (Fire=orange, Cold=cyan, Magic=purple, Curse=magenta, Poison=lime, Physical=white)
- **Short read time** — most impact VFX 3-5 frames of peak intensity, total <0.5s lifetime
- **Crit accents** — extra punch (additional flash + larger burst) for crit hits

## Combat VFX List

### Hit Impacts (6 systems — one per damage type)
- [ ] `NS_HitImpact_Physical` — sparks + dust puff, white-yellow
- [ ] `NS_HitImpact_Fire` — embers + flame burst, orange-red
- [ ] `NS_HitImpact_Cold` — frost spikes + chill mist, cyan-white
- [ ] `NS_HitImpact_Magic` — arcane ripples + glow, purple
- [ ] `NS_HitImpact_Curse` — shadow tendrils + dark wisp, magenta-black
- [ ] `NS_HitImpact_Poison` — bubble pop + green mist, lime

### Crit Accent
- [ ] `NS_CritAccent` — additive over the base hit impact, white starburst + slight shockwave

### Ability VFX (16 systems — 4 abilities × 4 classes)

#### Warrior
- [ ] `NS_Ability_Slash` — sword arc trail + sparks
- [ ] `NS_Ability_Whirlwind` — spiral wind + dust ring at radius
- [ ] `NS_Ability_Charge` — speed lines + impact burst at end
- [ ] `NS_Ability_BerserkRoar` — red aura aura around character + ground crack

#### Archer
- [ ] `NS_Ability_AimedShot` — arrow trail + impact emphasis
- [ ] `NS_Ability_Volley` — multiple arrow trails fanning out
- [ ] `NS_Ability_Roll` — dust + motion blur trail
- [ ] `NS_Ability_RainOfArrows` — sky-down arrow rain particles + ground impacts

#### Assassin
- [ ] `NS_Ability_Backstab` — shadow blur from behind + critical hit emphasis
- [ ] `NS_Ability_ShadowStrike` — teleport-style smoke puff before/after
- [ ] `NS_Ability_Vanish` — fade-out smoke + reappear puff
- [ ] `NS_Ability_DeathMark` — skull/curse mark hovering over target

#### Mage
- [ ] `NS_Ability_Bolt` — projectile trail + impact (placeholder while projectiles offline)
- [ ] `NS_Ability_Blast` — radial expansion + screen distortion
- [ ] `NS_Ability_Blink` — teleport flash + arcane sparks
- [ ] `NS_Ability_Meteor` — sky drop streak + ground impact + lingering fire

### Status Effect Visuals (loops, attached to character)
- [ ] `NS_StatusEffect_Burn` — looping flame on character body, fades on expire
- [ ] `NS_StatusEffect_Chill` — frost crystals + blue tint material override
- [ ] `NS_StatusEffect_Poison` — bubbling green particles + sickly tint

### Boss-Specific
- [ ] `NS_Boss_TelegraphWindup` — chargeup VFX during 1s windup (red glow growing, ground tremble particles)
- [ ] `NS_Boss_FollowerSpawn` — summoning portal/burst when Mega Grizzlebark spawns Swift Sprouts at health thresholds
- [ ] `NS_Boss_PhaseShift` — dramatic burst at HP 0.7 / 0.4 / 0.1 thresholds
- [ ] `NS_Boss_Death` — large death effect (multi-stage: stagger → kneel → final burst)

## World VFX List

- [ ] `NS_Pickup_GoldGlow` — looping glow on the spinning coin actor (replace material-only shine)
- [ ] `NS_Pickup_GoldCollect` — burst on overlap when player picks up
- [ ] `NS_Pickup_ItemGlow` — looping glow on item world drops (when added; chests for now)
- [ ] `NS_Chest_Open` — burst when chest opens (with item-rarity tint variant)
- [ ] `NS_Vendor_Interact` — subtle shimmer on vendor when player aims at it (interact-ready cue)
- [ ] `NS_Totem_Activate` — burst when totem starts encounter
- [ ] `NS_Totem_Idle` — subtle ambient particles per difficulty (Easy=faint, Med=medium, Hard=strong)
- [ ] `NS_CardPick_Reveal` — sparkle/swirl when 3-card draw appears

## UI / Feedback VFX

- [ ] `NS_LevelUp_<Class>` — class-colored burst around character on level-up (4 variants if budget allows; 1 generic burst otherwise)

## Total Count

- 6 hit impacts + 1 crit
- 16 ability VFX
- 3 status loops
- ~10 boss-specific
- 8 world VFX
- 1-4 level-up

**Total ~40-45 systems** for full vertical slice. Many can share base particle modules with parameter overrides (Niagara module library = efficient).

## Naming Conventions

| Type | Prefix | Example |
|---|---|---|
| Niagara System | NS_ | NS_HitImpact_Fire |
| Niagara Emitter (reusable) | NE_ | NE_Sparks_Generic |
| Niagara Module | NM_ | NM_BurstSpawn_Radial |
| Material | M_VFX_ | M_VFX_Glow_Additive |

## Performance Budget

- Per-impact VFX: <0.5ms GPU on RTX 3060
- Status loop on character: <0.2ms each, max 3 concurrent
- Boss VFX: bigger allowance (~1-2ms) since it's a hero moment

## Where VFX Plug In

- Land assets in `Content/VFX/Combat/`, `Content/VFX/Abilities/`, `Content/VFX/World/`, `Content/VFX/UI/`
- We wire spawn calls in BP — typically from `HealthComponent.OnDamageReceived` broadcast or ability activation
- For impact-on-damage-type: VFX picks from a TMap<EOryxDamageType, UNiagaraSystem*> on HealthComponent (we add the lookup BP-side)

## Deliverable Checklist

See lists above. Recommend tackling in this order:
1. Hit impacts (6 + crit) — highest visibility, every fight uses them
2. Status loops (3) — also constantly visible
3. Boss telegraph + phase shift — money-shot moments for trailer
4. Pickup/chest/vendor — minor but pervasive feedback
5. Ability VFX — most volume, can stagger delivery
6. Level-up — nice-to-have, last priority

---

**Contact:** [your contact info]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc (game pillars), AssetReplacementSpec.md
