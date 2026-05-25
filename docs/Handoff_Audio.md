# Handoff Brief — Sound Designer / Composer

## Target

Replace placeholder SFX (royalty-free / Engine sounds) + placeholder music tracks with original or library-curated final audio. Game currently has minimum-viable audio coverage in P6 — this brief covers the upgrade pass.

## Style Direction

Refs: **Hades / Sea of Stars / Hollow Knight** — dark fantasy, orchestral + percussive. Music drives mood; SFX carries combat feedback. Wild Plains (Stage 1) bias: lighter than later biomes (Stage 2 = Blighted Depths, Stage 3 = Pits of Hell — final tracks for those come later).

## SFX Checklist

### Player Movement
- [ ] Footsteps × surface variants (grass, stone, wood) — loop layer
- [ ] Jump grunt
- [ ] Land thud (light / heavy variants)
- [ ] Sprint loop (wind / breath layer)
- [ ] Dash whoosh (~0.3s sharp swoosh)

### Combat — Player
- [ ] Light attack swing (per weapon: sword / bow / dagger / staff)
- [ ] Heavy attack swing (per weapon, deeper register)
- [ ] Ability cast (per ability per class — 16 unique stings, can share base layer with variation)
- [ ] Hit-taken grunts (light / heavy)
- [ ] Low-HP heartbeat loop (kicks in <25% HP, fades on heal)
- [ ] Player death cry

### Combat — Impact
- [ ] Weapon impact × damage type (physical thwack, fire sizzle, cold crack, magic chime, curse hiss, poison squelch)
- [ ] Critical hit emphasis (extra punch + ringing tail)
- [ ] Hit reaction grunts × enemy variants

### Enemies
- [ ] Aggro alert per enemy type (Goblin chitter, Sprout chirp, Slime gurgle)
- [ ] Attack windup cues
- [ ] Attack swing impact
- [ ] Hit-taken
- [ ] Death (per type)

### Boss
- [ ] Mega Grizzlebark idle rumble (subtle, looping presence)
- [ ] Boss aggro roar (room-shaking, plays on arena entry)
- [ ] Telegraph windup rumble (1s, layered with VFX cue — see `AOryxBoss::TelegraphAttack`)
- [ ] Heavy attack impact (massive)
- [ ] Phase-shift sting (plays at HP thresholds 0.7 / 0.4 / 0.1)
- [ ] Follower spawn screech (when Swift Sprouts spawn)
- [ ] Boss death (long, multi-stage to match anim)

### UI
- [ ] Button hover
- [ ] Button click (confirm / cancel variants)
- [ ] Vendor screen open / close
- [ ] Vendor buy success + buy fail (insufficient gold)
- [ ] Sell confirm
- [ ] Card pick (per category: Strength = heavy, Magic = ethereal, Agility = light)
- [ ] Level-up sting (per class? — or universal triumph)
- [ ] Achievement-style sting (e.g. boss kill, run extract)
- [ ] Pause / unpause whoosh

### World
- [ ] Gold pickup chime
- [ ] Item pickup sparkle
- [ ] Chest open creak + reveal
- [ ] Totem activate (escalating tone per difficulty)
- [ ] Card reveal sparkle
- [ ] Ambient footstep layer for biome (forest creaks, wind, distant fauna)

## Music

Three layered tracks for Wild Plains (looping with intro/outro stems where possible):

### Biome Ambient (calm exploration)
- 2-3 min loop
- Sparse instrumentation: strings, woodwinds, soft percussion
- Mood: peaceful but slightly off (forest is deceptively dangerous)

### Combat Layer (kicks in on enemy aggro)
- Same key/tempo as ambient — layers on top dynamically
- Adds drums, bass, more aggressive strings
- Fades back to ambient 5-10s after last engagement
- Implementation: separate SoundCue routed through Combat layer that interpolates volume on aggro state

### Boss Theme (separate looped track for boss arena)
- 3-4 min loop, more dramatic
- Driving percussion + heroic brass
- Distinct from ambient/combat — clear "this is a boss" cue
- Intro stem (5-10s) plays on arena enter; loop kicks in after

## Naming Conventions

| Type | Prefix | Example |
|---|---|---|
| Sound Wave | A_ | A_Combat_HitImpact_Fire |
| Sound Cue | SC_ | SC_Combat_HitImpact_Fire |
| Sound Class | SCL_ | SCL_Combat / SCL_UI / SCL_Music |
| Sound Mix | SMix_ | SMix_Default |

Route all sounds through their appropriate Sound Class so audio sliders (Master/SFX/Music/UI) work — those Sound Classes are wired in `WBP_Settings`.

## File Format

- WAV uncompressed, 48kHz, 16-bit (UE will compress on cook)
- Mono for point-source SFX (combat / world)
- Stereo for music and ambient layers

## Deliverable Counts

- ~80-100 SFX assets (split across categories above)
- 3 music tracks (ambient / combat layer / boss)
- Optional: 1-2 stinger tracks (level-up, run extract, victory)

Tackle in this priority order if budget is tight:
1. Combat hit impacts + swings + player damage taken (every fight uses these)
2. UI sounds (every interaction uses these)
3. Boss music + boss-specific SFX (signature moment for trailer)
4. Ambient + combat music loops
5. Misc world SFX
6. Enemy-specific variations (last priority)

## Where Audio Plugs In

- Land assets in `Content/Audio/Combat/`, `Content/Audio/UI/`, `Content/Audio/World/`, `Content/Audio/Music/`
- We wire spawn calls in BP (UGameplayStatics::PlaySoundAtLocation or USoundComponent on key actors)
- For impact-on-damage-type: SoundCue routed via Sound Class, picked from a TMap<EOryxDamageType, USoundCue*> on HealthComponent

---

**Contact:** [your contact info]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc, AssetReplacementSpec.md
