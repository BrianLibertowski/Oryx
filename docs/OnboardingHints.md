# Onboarding Hints — First 30 Seconds Tooltip Script

Each hint fires once per save profile. Trigger via `TriggerBox` overlap or game-event broadcast.
Display via `WBP_HintOverlay`: simple TextBlock at center-bottom, auto-fades 4 seconds after spawn.

## Trigger Events + Hint Text

### Hint 01 — Spawn (1.5s delay after pawn possession)
**Trigger:** `BeginPlay` on player character, after camera lock.
**Text:** `WASD to move. SHIFT to sprint. SPACE to jump.`

### Hint 02 — First Aggro (when first enemy notices player)
**Trigger:** First call to `MarkAlerted` against this player from any enemy.
**Text:** `LMB to attack. Q for ability. E to dash.`

### Hint 03 — First Kill (any enemy dies by player damage)
**Trigger:** `OnDeath` broadcast where Instigator is this player.
**Text:** `Enemies drop gold. Run over coins to collect.`

### Hint 04 — Stamina Low (player drops below 25% stamina)
**Trigger:** `GetStaminaFraction() < 0.25` first time per session.
**Text:** `Dashing and sprinting cost stamina. Stop moving to regen.`

### Hint 05 — Health Low (player drops below 30% HP, first time)
**Trigger:** `OnDamaged` broadcast brings `CurrentHealth / MaxHealth < 0.3`.
**Text:** `Low health! Pickups and totems restore health.`

### Hint 06 — Vendor Proximity (within 500cm of any BP_Vendor)
**Trigger:** Sphere overlap on player against `AOryxVendor::InteractSphere` outer radius.
**Text:** `Press F near the vendor to browse items.`

### Hint 07 — First Vendor Interact (vendor screen opens for first time)
**Trigger:** `WBP_VendorScreen.InitForVendor` fires for the first time.
**Text:** `Items add permanent stats while in your inventory. Click Buy.`

### Hint 08 — First Item Bought (inventory first transitions from empty → 1 item)
**Trigger:** `OnInventoryChanged` fires with `GetItemCount() == 1`.
**Text:** `Items stack. Open Tab to see your stats and inventory.`

### Hint 09 — First Tab Press (player presses Tab for first time)
**Trigger:** `OnToggleStatsScreen` BP event first invocation.
**Text:** `All your stats update live. Press Tab again to close.`

### Hint 10 — First Totem Sight (player within 1500cm of any totem they haven't activated)
**Trigger:** Aim line trace hits a `BP_Totem` for the first time.
**Text:** `Totems start a fight when you interact. Easy → Hard difficulty.`

### Hint 11 — First Totem Cleared
**Trigger:** `OnEncounterComplete` fires from a `BP_Totem` for the first time.
**Text:** `You earned gold and XP. Spend gold at vendors and chests.`

### Hint 12 — First Card Pick Available
**Trigger:** `WBP_CardPickerScreen` first spawn.
**Text:** `Cards give a run-only stat boost. They reset on death.`

### Hint 13 — First Level Up
**Trigger:** `OnLevelUp` delegate first broadcast (any class).
**Text:** `You leveled up! Allocate skill points from the main menu.`

### Hint 14 — Boss Arena Approach (within 2000cm of boss arena trigger)
**Trigger:** Sphere overlap with boss arena entry trigger.
**Text:** `Boss arena ahead. Save gold and items for this fight.`

### Hint 15 — First Boss Telegraph
**Trigger:** `AOryxBoss::OnTelegraphAttack` first broadcast.
**Text:** `He's winding up. Get behind him or roll out of range.`

### Hint 16 — First Boss Defeated
**Trigger:** `AOryxRunManager::HandleBossDefeated` first time.
**Text:** `Boss down! Extract for safety, or enter Wave Mode for more XP.`

### Hint 17 — First Death
**Trigger:** `AOryxRunManager::HandlePlayerDeath` first time.
**Text:** `Run ended. Your skill points and items carry over. Try a different build.`

## Implementation Notes

- Store "seen" flags on `UOryxProfileSave` — bool TArray<bool> SeenHints (one bool per hint id, ordered).
- Pass an enum `EOryxHintId` to `WBP_HintOverlay.ShowHint(EOryxHintId)`. Widget looks up display text from a `TMap<EOryxHintId, FText>` so localisation friendly.
- Allow user to disable hints in Settings → Gameplay tab → "Show tutorial hints" toggle.

## Hint Text Style Guide

- 1 sentence each, ≤ 10 words where possible.
- Imperative voice. No second-person hand-holding ("To attack, you can press..."). Just: "LMB to attack."
- Mention the action + the outcome in one breath when possible.
- Skip exclamation marks unless emphasising stakes (low HP, boss telegraph).
