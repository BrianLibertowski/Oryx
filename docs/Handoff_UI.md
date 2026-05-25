# Handoff Brief — UI Artist

## Target

Replace placeholder UI widgets (gradient backgrounds, system fonts, unstyled buttons) with finished UI matching the game's "arcana / dark fantasy heroic" art direction.

## Style Direction

From Design Vision: **"light calm + fantasy + heroism, arcana-inspired magical detailing — feels embroidered into the game, not imported"**. Refs: Hades / Hollow Knight / Sea of Stars UI aesthetic.

- Clean minimal HUD that stays out of the way during combat
- Menu UI more elaborate — moments where player is calmly browsing (vendor, skill tree)
- Color palette echoes biome (Stage 1 = warm forest greens + golds; later stages introduce darker tones)
- Subtle magical detailing — runes, illuminated borders, soft glows

## Widget Inventory

### Menu / Out-of-Combat
- [ ] `WBP_MainMenu` — title screen, class select carousel preview, Play / Settings / Quit
- [ ] `WBP_CharacterSelection` — 4 class portraits in row, hover details
- [ ] `WBP_Settings` — Display / Audio / Controls / Gameplay tabs (already structured, needs visual upgrade)
- [ ] `WBP_PauseMenu` — Resume / Settings / Save / Leave Run / Quit

### In-Game HUD
- [ ] `WBP_PlayerHUD` — Health/Mana/Stamina radial bars (currently M_RadialBar_Combined material; replace with stylized rings) + Gold counter + Inventory bar top-left
- [ ] `WBP_FloatingDamageNumber` — color per damage type (palette already set, can keep)

### Vendor & Inventory
- [ ] `WBP_VendorScreen` — vendor name + gold + 6 stock rows + close button
- [ ] `WBP_VendorStockEntry` — single row: icon / name / mods / cost / stock / Buy button
- [ ] `WBP_InventoryIcon` — small (~48×48) item icon with stack count badge + tooltip
- [ ] `WBP_StatsScreen` — Tab-toggled, 23 stat rows + inventory section (Tab inventory ships in P6)

### Run-Lifecycle Screens
- [ ] `WBP_RewardScreen` — 3-choice picker (Vitality/Swiftness/Restore — existing camp reward)
- [ ] `WBP_CardPickerScreen` — 3-card pick (Strength/Magic/Agility), card-art-style frames
- [ ] `WBP_StageCleared` — Extract / Wave Mode buttons
- [ ] `WBP_RunTally` — end-of-run summary (kills/gold/items/time)
- [ ] `WBP_HintOverlay` — onboarding tooltip popups (auto-fade)

### Phase 4 (W6 — drafts will exist by handoff time)
- [ ] `WBP_SkillTreeScreen` — passive linear tree (20 nodes for demo) + 5 ability cluster trees per class
- [ ] Skill node icons (per node) — pure stat upgrade visuals

## Item Icons (32 textures)

Currently using royalty-free placeholder icon set (catalogued in Items.md). Custom icons matching art direction:
- Per item (32 total in Stage 1 demo)
- 256×256 png with alpha
- Frame-style: simple silhouette + bold color, NOT photorealistic
- Rarity-coloured glow accent (Common = grey, Uncommon = green, Rare = blue, Epic = purple, Legendary = orange/gold)

## Iconography Patterns

- **Stat icons** — for HUD, Stats screen, tooltips. Need icons for: Health, Mana, Stamina, Gold, Damage, AttackSpeed, CritChance, CritDamage, MoveSpeed, Armor, plus each damage type (6) and regen variants (3).
- **Ability icons** — 16 (4 abilities × 4 classes), 256×256 with frame
- **Class crests** — 1 per class for class-select + main menu
- **Boss emblem** — Mega Grizzlebark heraldry-style mark

## Font Choices

Suggest two faces:
- **Display / Headings** — fantasy serif or modern-with-fantasy-touches (e.g. Cinzel, Cormorant Garamond)
- **Body / Stats** — clean sans-serif optimized for in-game readability at small sizes (e.g. Inter, Lato, or a humanist sans)

Provide font files (TTF/OTF) and check licensing for game use.

## Animation Hooks

UI artist can also provide subtle animation patterns:
- Button hover: gentle glow expansion (0.15s)
- Vendor row Buy success: brief green pulse on row
- Vendor row Buy fail: red shake (insufficient gold)
- Card pick: card flip-reveal animation
- Level-up: HUD ring pulse outward
- Boss arena entry: vignette pulse

These can be implemented as UMG Animations once the visual assets are designed.

## Deliverables

- [ ] All widget visual upgrades (12-15 widgets total)
- [ ] 32 item icons (256×256 with alpha)
- [ ] ~30 stat / ability / class icons
- [ ] Font files + license
- [ ] Color palette + style guide doc (1-2 pages, hex codes + usage rules)
- [ ] Optional: UMG animation guide (recommended timing / curves)

Priority order if budget is tight:
1. Vendor screen + stock entry + Inventory icon (most-used in-game)
2. HUD radial bars + gold counter (always visible)
3. Item icons (32 — could split across multiple commissions)
4. Main menu + character select (first impression for funder)
5. Card picker + reward screens (signature moments)
6. Skill tree screen (W6 delivery, ships after Phase 4 builds out the BP)

## Where Widgets Plug In

- Material instances live in `Content/UI/Materials/`
- Textures in `Content/UI/Textures/` (mirror existing ItemIcons subfolder pattern by rarity)
- Fonts in `Content/UI/Fonts/`
- UI artist can either:
  - **Ship Photoshop / Figma mockups** — we implement in UMG ourselves
  - **Ship implemented UMG widgets** — we accept the WBP_* directly
  - **Hybrid** — mockups for first pass, then implementation for the moneyshots (skill tree, card picker)

---

**Contact:** [your contact info]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc, AssetReplacementSpec.md
