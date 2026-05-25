# ORYX — Playtester Build

Thanks for trying the demo. This is a **proof-of-concept build** — combat systems are in, art is placeholder. The goal: validate that the gameplay loop is fun and the combat feels right.

## Installation

1. Unzip the folder somewhere convenient.
2. Run `Oryx.exe`.
3. If Windows SmartScreen blocks it: **More info** → **Run anyway**. (Unsigned demo build — final retail will be signed.)

**Requirements:**
- Windows 10/11 64-bit
- ~4 GB free disk space
- DX12-compatible GPU (RTX 2060 / RX 5600 XT or better recommended)
- 8 GB RAM minimum

## Controls

| Action | Key / Mouse |
|---|---|
| Move | WASD |
| Look | Mouse |
| Jump | Space |
| Sprint | Shift |
| Dash / Dodge | Alt |
| Attack (melee/ranged) | LMB |
| Ability (varies by class) | Q |
| Interact (vendors, totems, chests) | F |
| Toggle Stats / Inventory | Tab |
| Pause / Settings | Esc |

Gamepad supported (Xbox layout default). Settings menu lets you tweak.

## What to Try

The demo runs ~30 minutes per attempt. Suggested first run:

1. **Pick a class** on the main menu. Warrior is the easiest first-timer choice (durable, simple).
2. **Spawn in the Blockout** — wander around. Hit enemies. Notice the floating damage numbers.
3. **Find a vendor** — there are 3 in the map. Press F to open. Buy something you can afford.
4. **Hit a totem** (the capsule pillars) — press F to start a fight. Different sizes = different difficulty.
5. **Clear a camp** — every camp gives you a card pick (Strength / Magic / Agility). Pick one.
6. **Find the boss arena** — the big circular zone. Fight Mega Grizzlebark. He has telegraphed heavy attacks; dodge or get behind him.
7. **Extract or Wave Mode** — after the boss dies, choose. Wave Mode is endless XP-only.

If you die, you lose your current run. **Skill points + items earned do NOT carry over to next run yet** in this build — that's planned for the next milestone.

## What's Real vs Placeholder

| ✅ Real (gameplay-locked) | ⚠️ Placeholder (will be replaced) |
|---|---|
| Combat system + damage formula | Character meshes (Mannequin) |
| 23 stats driving everything | Weapon visuals (boxes/cylinders) |
| Status effects (Burn, Chill, Poison) | Enemy meshes (tinted Mannequin) |
| Vendor / inventory / cards | Animations (some are missing) |
| Skill tree architecture (Phase 4 wires fully) | VFX (color-coded placeholders) |
| Save / load (partial) | Sound effects (minimal) |
| Per-class XP routing | Music (royalty-free placeholders) |
| Boss telegraph + follower waves | UI styling (functional but unstyled) |
| 32 unique items | Wild Plains map (greybox) |

## Feedback I Care About

Please tell me:

1. **Did you finish the demo?** If not, where did you stop and why?
2. **What did the combat feel like?** Punchy? Floaty? Confusing? Specific moments where it clicked or didn't.
3. **Was the loop clear?** Did you know where to go and what to do? Where did you get lost?
4. **What was missing?** What's the first thing you'd want changed/added?
5. **Would you back this on Kickstarter?** Why or why not. (Honest answers only — no pressure.)
6. **Pick a class you DIDN'T play and tell me what you expected it to feel like** based on its name + on-screen abilities.

Feedback channels:
- **Discord:** [link]
- **Email:** [your email]
- **Quick form:** [Google form / Tally link]

## Known Issues

- Animations are missing in places — characters slide instead of animating. Known. Animator is queued for funded work.
- Some Niagara VFX are color blobs. Will be replaced.
- Save-and-resume mid-run is incomplete in this build. If you want to keep progress, just stay in the run until you extract or die.
- Performance is unoptimised; some scenes may hitch. Tested at 60fps on RTX 3060 / 1080p.
- Vendor UI text overlaps on items with long descriptions. Cosmetic.
- A few placeholder console logs spam the screen briefly. Ignore.

## Bug Reports

If something hard-crashes the game or blocks progression:

1. Note what you were doing.
2. Find the crash log at `[install path]/Saved/Logs/Oryx.log`.
3. Send the last ~50 lines of the log + your description.

## Telemetry

This build logs gameplay events (deaths, run extracts, time-to-boss) to `[install path]/Saved/Telemetry/<session>.json` locally. Nothing is sent over the network. You can delete those files anytime; they're for me only if you choose to share them.

## Privacy

- No accounts, no online auth, no DRM.
- No data sent off your machine without you explicitly sharing it.
- The Saved/ folder is yours — delete to wipe the demo entirely.

## Thank You

Building a game is hard. Watching someone else play yours and tell you the truth is harder. Thanks for doing it.

— [Your name]
ORYX dev, 2026

---

*This build dated [DATE_FROM_BUILD_TIMESTAMP]. Demo build version: [BUILD_NUMBER].*
