# ORYX Project Documentation

This folder contains all design + handoff + planning artifacts for the demo through 2026-07-15.

## How to use

Treat each `.md` here as the source of truth for its topic. Update inline when state changes. Cross-link liberally — many docs reference siblings here.

## Index

### Process / Planning
- **[CompileChecklist.md](CompileChecklist.md)** — pre-compile sanity + paths (Live Coding → IDE build → UBT) + expected error catalog + post-compile verification. Includes "Second compile (post-wiring batch)" + audit fix + settings scaffold sections from 2026-05-26.
- **[Settings_BPConsumption.md](Settings_BPConsumption.md)** — BP wire-up reference for `UOryxGameUserSettings`. Per-setting pattern, live consumer recipe (HUD opacity / FOV / screen shake / blood toggle), defaults rationale, recipe for adding new settings.

### Handoff Briefs (W10 — Outsource Pipeline)

These get sent to outsource artists once funded. Drafted ahead so the actual W10 work is fill-in-contact-info, not write-from-scratch.

- **[AssetReplacementSpec.md](AssetReplacementSpec.md)** — master cross-reference. Lists every placeholder asset, which outsource discipline owns it, and replacement status. Updated as deliveries land.
- **[Handoff_3DCharacter.md](Handoff_3DCharacter.md)** — Skeleton + sockets + scale + mesh budget + textures + naming + deliverable checklist for the 4 player classes + 3 enemies + Mega Grizzlebark + 4 weapons.
- **[Handoff_Animator.md](Handoff_Animator.md)** — Full per-class animation list (~110-120 animations across locomotion / combat / 16 abilities / enemies / boss).
- **[Handoff_VFX.md](Handoff_VFX.md)** — ~40-45 Niagara systems (hit impacts × 6 damage types + crit + 16 ability VFX + 3 status loops + boss-specific + world VFX + level-up).
- **[Handoff_Audio.md](Handoff_Audio.md)** — ~80-100 SFX + 3 music tracks (biome ambient / combat layer / boss theme). Sound Class routing for audio sliders.
- **[Handoff_UI.md](Handoff_UI.md)** — Widget inventory (12-15 widgets) + 32 item icons + ~30 stat/ability/class icons + font choices + animation hooks.
- **[Handoff_Environment.md](Handoff_Environment.md)** — Wild Plains map placement spec (spawn / 3 vendors / 3-5 chests / 20-30 totems / 2-3 outposts / 1-2 elite arenas / boss arena), navmesh / lighting / foliage / performance.

### Content Authoring References
- **[SkillTree_Warrior_Full.md](SkillTree_Warrior_Full.md)** — **LEAD skill tree doc (Rev 2, 2026-06-13).** Framework-compliance map + point economy + authoring workflow + Warrior content: class tree (24 nodes, Core/Offense/Defense/Utility) + 5 ability trees (Cleave Slash / Shield Rush / Earthshatter / War Cry / Execution Strike).
- **[SkillTree_Archer_Full.md](SkillTree_Archer_Full.md)** — Archer: class tree (Marksmanship/Skirmisher/Survival/Utility) + Piercing Shot / Volley Rain / Dashstep / Poison Arrow / Falcon Mark. Subclasses: Ranger / Shadow Hunter.
- **[SkillTree_Assassin_Full.md](SkillTree_Assassin_Full.md)** — Assassin: class tree (Agility/Deception/Lethality/Utility) + Shadow Strike / Smoke Veil / Backstab / Shadow Dash / Fan of Blades. Subclasses (3 per mockup): Blade Dancer / Shadowblade / Trickster.
- **[SkillTree_Mage_Full.md](SkillTree_Mage_Full.md)** — Mage: class tree (Elemental/Arcane/Defense/Utility) + Arcane Bolt / Frost Nova / Fireball / Lightning Storm / Blink. Subclasses (3 per mockup): Sorcerer / Spellblade / Runemaster. ⚠️ CastSpeed needs a consumer (P5).
- **[SkillTree_Example_Warrior.md](SkillTree_Example_Warrior.md)** — superseded for content; still useful for the in-editor authoring walk-through.
- **[OnboardingHints.md](OnboardingHints.md)** — 17 first-30-second hints with exact text + trigger conditions. Implementation pattern + style guide.

### Gameplay Design Specs (added 2026-05-25)
- **[Design_CombatSynergies.md](Design_CombatSynergies.md)** — D22 chain reaction combos (6 pairs incl. Shatter, Plague, Pyroclasm, Wither, Hex, Pyrotoxin) + D23 Curse Shrine system (8-curse pool with mechanics). Authoring + tuning notes.
- **[Design_Subclasses.md](Design_Subclasses.md)** — 8 subclass concepts (Berserker / Knight, Hunter / Ranger, Shadow / Acolyte, Elementalist / Stormcaller). Identity briefs, 5-ability lists, tree themes per subclass.
- **[Design_Difficulty.md](Design_Difficulty.md)** — Hybrid adaptive difficulty spec per D25. 4 explicit tiers (Wanderer/Adventurer/Hero/Nightmare) with concrete stat multipliers + hidden catch-up scaling per consecutive-death count + co-op difficulty scaling.
- **[Achievements.md](Achievements.md)** — 48 Steam achievements across 8 categories (Easy/Onboarding, Class Progression, Content Milestones, Difficulty, Combat Mastery, Currency, Skill Tree, Hidden). Trigger conditions per achievement.

### Marketing / Funding (W10)
- **[PitchDeck_Outline.md](PitchDeck_Outline.md)** — 12 slides, investor-facing. Title / identity / loop / classes / Wild Plains / combat / items / cards / skill trees / competitor positioning / team / funding ask + roadmap. Style direction + tool recommendations.
- **[Kickstarter_Draft.md](Kickstarter_Draft.md)** — Long-form Kickstarter campaign page. Pitch / video / core loop / class summary / skill trees / items + cards / boss / why fund / 7 funding tiers / 5 stretch goals / roadmap / team / FAQ / risks. Embed points marked for gifs/screenshots.
- **[README_Playtester.md](README_Playtester.md)** — Ships in the demo build zip. Installation / controls / what to try / real vs placeholder / feedback channels / known issues / privacy.

## Editing Conventions

- Markdown only. Easy to convert to PDF / Google Doc / Kickstarter rich-text later.
- Update inline as state changes. Don't make new copies — overwrite.
- Cross-link with relative paths (`[Handoff_VFX.md](Handoff_VFX.md)`) so docs are portable.
- Date stamps at the top of each doc where they're informative (`Drafted 2026-05-25`).

## Doc Status Snapshot (as of 2026-05-25)

| Doc | Status |
|---|---|
| AssetReplacementSpec.md | Stub drafted, fill Status as deliveries land |
| Handoff_3DCharacter.md | Full draft, awaiting contact info |
| Handoff_Animator.md | Full draft, awaiting contact info |
| Handoff_VFX.md | Full draft, awaiting contact info |
| Handoff_Audio.md | Full draft, awaiting contact info |
| Handoff_UI.md | Full draft, awaiting contact info |
| Handoff_Environment.md | Full draft, awaiting contact info |
| CompileChecklist.md | Ready for tomorrow's compile pass |
| SkillTree_Example_Warrior.md | D21-refactored: Cleave Mastery fully spec'd + 4 skeletons. Template for other 3 classes. |
| Settings_BPConsumption.md | Full BP wire-up reference for the expanded UOryxGameUserSettings scaffold. |
| OnboardingHints.md | 17 hints scripted, awaiting WBP_HintOverlay implementation |
| PitchDeck_Outline.md | 12-slide content, awaiting deck assembly |
| Kickstarter_Draft.md | Full long-form draft, awaiting media embeds |
| README_Playtester.md | Final draft, awaiting build timestamp / version stamp |

## Cross-references

- The **demo gate** (2026-07-15 — Funding-ready PoC) requires all these docs final, the playable build packaged, and pitch materials produced.
- Tracked in Asana project gid **`1214560813037824`**. See ROADMAP anchor task for week-by-week breakdown.
- Memory cross-reference: `C:\Users\Arzuh\.claude\memory\project_oryx.md` + `project_oryx_plan_2month.md`.
