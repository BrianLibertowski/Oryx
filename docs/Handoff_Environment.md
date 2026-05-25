# Handoff Brief — Environment Artist

## Target

Replace `Content/Maps/Blockout` (greybox prototyping level) with `Content/Maps/WildPlains_Final` — a stylized dark-fantasy forest map. Already in progress with environment artist; this doc is the integration spec for plugging in gameplay actors when the map lands.

## Style Direction

Refs: Hades + Risk of Rain 2 + LORT — stylized, dark fantasy, readable silhouettes. Wild Plains is the first of 3 biomes (Stage 1 of 3). Theme: bright magical forest, deceptively peaceful (turns hostile when player engages).

- **Bright but dark-fantasy mood** — well-lit gameplay areas, atmospheric/foggy fringes
- **Strong silhouette discipline** — never let foliage obscure enemy character outlines
- **Volumetric fog allowed** but tuned so visibility through it is good (game pillar 1: Stylised Clarity)
- **Color palette**: greens + earth tones primary, with magical accents (luminescent mushrooms, fairy lights) for visual variety

## Gameplay-Critical Zones (Required Placements)

These are non-negotiable in the final map — gameplay won't work without them.

### Player Spawn Area
- Location: clearly recognisable starting zone
- Footprint: ~10×10m flat
- Surrounded by introductory totems (easy difficulty) within 100m walk

### Vendor Footprints (3 fixed positions)
- Each: ~5×5m flat, sheltered (under tree canopy, rock outcrop, ruined structure)
- Visually distinct per vendor (variety = visual interest as player explores)
- Suggested positions: near spawn, mid-map, near boss arena

### Chest Spawn Points (3-5 positions)
- Each: ~3×3m flat
- Hidden / off the obvious path — reward exploration
- Mix of stages later: Stage 1 chests scattered around playable area

### Totem Placement Points (20-30 markers)
- Each: any flat 2×2m surface
- Mix of difficulty zones (easy near spawn, harder farther out)
- Cluster some near outposts to escalate intensity
- Can mark as Empty StaticMeshActor placeholders in the level — we replace with BP_Totem on integration

### Outpost Arenas (2-3 positions)
- Each: ~20×20m flat with defensible perimeter (cliffs, walls, tree-rings)
- Vendor spawn space inside the arena (~5×5m flat)
- Visual: ruined camp / overgrown fort / abandoned shrine — narrative implication of past battle
- One outpost ideally on the route to boss arena

### Elite Arena (1-2 positions)
- Each: ~15×15m circular flat zone
- Distinct visual flair (lit braziers, ritual circles, etc.) — visually telegraphs "big fight here"
- Slightly off main path so player can choose to engage

### Boss Arena (1 position)
- Footprint: ~40×40m flat with cover obstacles (boulders, broken pillars)
- Dramatic backdrop (cliff/sky view / unique ambient lighting)
- Clearly framed entrance trigger — player commits to fight by entering
- Vendor / totems should NOT be inside arena (prevent farming during fight)

## NavMesh Requirements

- Single connected `RecastNavMesh` covering ~95% of walkable surface
- Mesh extends through all gameplay zones, including inside outposts and boss arena
- Steep slopes (>45°) and cliff edges marked as un-walkable
- Doors / interactable obstacles: discuss case-by-case (not in demo scope but flagging)

## Lighting & Time-of-Day

- Day-time setup fixed (no dynamic time-of-day for demo)
- Mood: bright but cinematic, slight golden-hour bias
- Volumetric fog allowed but readable silhouettes preserved
- Lightmap baking allowed where appropriate — confirm static actors before commit

## Foliage

- Density tuned for performance: aim 60fps on RTX 3060-equivalent at 1080p (see Performance Budget below)
- Foliage MUST NOT cast shadows or particles into gameplay-critical zones (above)
- Distinct foliage variation between biome sub-zones (clearing / dense forest / cliffside / cave entrance)

## Performance Budget

- Whole-map draw: <60ms at 1080p on RTX 3060 (gives 16ms total frame at 60fps target)
- Foliage instance count: <10k visible at once via culling
- LOD distance for hero meshes: ~50m default
- Static lighting bake: feasible for placeholder; full bake during P6 polish week

## Deliverables

- [ ] `Content/Maps/WildPlains_Final.umap`
- [ ] Static meshes in `/Game/Environment/WildPlains/` with auto-LOD
- [ ] Materials in `/Game/Environment/Materials/`
- [ ] Foliage instances baked into the level (or via Foliage tool)
- [ ] Lighting / sky setup in the level (or via Sky Atmosphere actor)
- [ ] Empty `BP_TotemMarker` / `BP_ChestMarker` / `BP_VendorMarker` placeholder actors at intended spawn points (we replace with BP_Totem / BP_Chest / BP_Vendor on integration)

Alternative if simpler for you: just place static meshes named `Marker_Totem_01` etc. at the locations — we walk through and convert each.

## Integration Process

When the map lands:
1. Open `WildPlains_Final.umap`
2. Replace each marker actor with the appropriate BP (BP_Totem with Difficulty set, BP_Chest with Stage set, BP_Vendor with ItemPool populated)
3. Verify NavMesh covers all play zones
4. Set as `Content/Maps/Blockout` replacement in Project Settings → Maps & Modes if appropriate
5. Smoke test PIE: spawn → reach boss arena via the most direct path, count time-to-boss target ~30-40min

## Communication

- Check-ins at 30% / 70% / 100% to catch placement issues early
- Ping if any gameplay zone footprint above seems infeasible — we'll adjust placement
- Drop reference / mood-board images in `Content/Art/References/Environment/`

---

**Contact:** [your contact info]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc (game pillars, biomes), AssetReplacementSpec.md
