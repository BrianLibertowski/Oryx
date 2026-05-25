# Handoff Brief — 3D Character Artist

## Target

Replace 4 player character skeletal meshes + boss + 3 stage-1 enemies + 4 weapons. All currently placeholder Mannequin variants or Modeling Mode primitives.

## Skeleton Requirements

- **Compatible with UE5 Mannequin skeleton** (preferred — reuses Engine-provided anim retarget data + IK Rig), OR
- **Custom rig with IK Rig + IK Retargeter setup** delivered alongside the meshes so the existing player anim BPs continue to work.

If custom rig: please include `IK_Rig_<MeshName>` asset and document any non-standard bone names.

## Required Sockets

Per player skeleton, add these sockets:
- `WeaponSocket_R` — right hand, weapon grip alignment
- `WeaponSocket_L` — left hand (off-hand / shield slot, currently unused but reserve)
- `BackStow_Weapon` — between shoulder blades, for stowed weapon visualisation later
- `RootMotion` (optional) — at the actor pivot; UE convention

## Scale & Proportions

- Player capsule: 96cm half-height (192cm total), 42cm radius
- Target eye height matches Mannequin defaults (avoids camera retune)
- Per-class size hints (rough, artistic licence welcome):
  - Warrior: stout, broad shoulders
  - Archer: lean, tall
  - Assassin: smaller, agile silhouette
  - Mage: robed, average height

## Mesh Budget (Demo Stage)

- Player: 10-30k triangles (LODs auto-generated post-import)
- Enemy grunts: 5-15k triangles
- Boss (Mega Grizzlebark): 30-50k triangles
- Weapons: 1-3k triangles each
- LOD plan: 3 LODs each, auto-decimation in UE Static Mesh Editor is fine

## Texture Budget

- Player: 2K diffuse + normal + ARM, optional secondary 1K material
- Weapons: 1K diffuse + normal + ARM
- Enemy / boss: same as player

## File Format

- **FBX** export from Blender/Maya/3DS Max
- Materials baked into separate texture maps (no embedded shaders)
- Unit scale: 1 unit = 1 cm (Unreal convention)
- Up axis: Z-up
- Forward axis: +X (UE default)

## Naming Conventions

| Asset | Prefix | Example |
|---|---|---|
| Skeletal Mesh | SK_ | SK_Player_Warrior |
| Static Mesh (weapon) | SM_Weapon_ | SM_Weapon_Sword |
| Skeleton | SKEL_ | SKEL_Player_Warrior (if custom) |
| Animation Blueprint | ABP_ | ABP_Player_Warrior (if shipped) |
| Materials | M_ / MI_ | M_Player_Warrior_Base / MI_Player_Warrior_Tinted |

## Deliverable List

- [ ] SK_Player_Warrior + textures
- [ ] SK_Player_Archer + textures
- [ ] SK_Player_Assassin + textures
- [ ] SK_Player_Mage + textures
- [ ] SK_Enemy_MossyGoblin + textures
- [ ] SK_Enemy_SwiftSprout + textures
- [ ] SK_Enemy_SlimeBlob + textures
- [ ] SK_Boss_MegaGrizzlebark + textures
- [ ] SM_Weapon_Sword_Warrior
- [ ] SM_Weapon_Bow_Archer
- [ ] SM_Weapon_Dagger_Assassin
- [ ] SM_Weapon_Staff_Mage

## Where Meshes Plug In

- Open the relevant BP (e.g. `BP_Oryx_Warrior`)
- Components panel → click `Mesh` (the inherited skeletal mesh component)
- Details → Skeletal Mesh → assign your `SK_Player_Warrior`
- Anim Class → keep existing `ABP_OryxCharacter` if compatible; replace if you ship a class-specific anim BP

## Communication

- Reference sheets / concept art welcome — drop in `Content/Art/References/`
- Tag review checkpoints — at 30% / 70% / 100% to catch fit issues early
- Ping if any animations would conflict with current montage scaffolds — we can adjust on our end

---

**Contact:** [your contact info here]
**Project repo / drive:** [shared location]
**Reference docs:** Design Vision doc (game pillars), Characters doc (4-class roster)
