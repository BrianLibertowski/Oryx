# Steam Achievements — Demo + Full Release List

48 achievements split across difficulty tiers, content milestones, and challenge runs. Steam display order should put easy/early achievements first (per Steam best practice — surfaces accomplishments quickly).

---

## Easy / Onboarding (8 — every player gets these)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_first_kill | **First Blood** | Defeat your first enemy. | First successful damage event resulting in enemy death. |
| ach_first_buy | **Open for Business** | Buy your first item from a vendor. | First successful RequestBuyItem. |
| ach_first_totem | **Trial Survivor** | Clear your first totem. | First Totem encounter completed. |
| ach_first_card | **Hand of Fate** | Pick your first card after camp clear. | First WBP_CardPickerScreen "accept" click. |
| ach_first_chest | **Treasure Hunter** | Open your first chest. | First chest interact success. |
| ach_first_levelup | **Growing Stronger** | Reach level 2 in any class. | OnLevelUp fires for the first time. |
| ach_first_runend | **And So It Begins** | Complete your first run (extract or die — doesn't matter). | RunManager.OnRunStateChanged → RunComplete OR Dead, first time. |
| ach_first_boss | **Mountain Climbed** | Defeat the boss for the first time. | RunManager.HandleBossDefeated, first invocation. |

---

## Class progression (8 — 2 per class)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_warrior_lvl10 | **Steel Resolve** | Reach level 10 with the Warrior. | LevelComponent.GetCurrentLevel(Warrior) >= 10 |
| ach_warrior_lvl20 | **Iron Bound** | Reach level 20 (max) with the Warrior. | LevelComponent.GetCurrentLevel(Warrior) >= 20 |
| ach_archer_lvl10 | **Steady Hand** | Reach level 10 with the Archer. | (parallel) |
| ach_archer_lvl20 | **True Marksman** | Reach level 20 with the Archer. | (parallel) |
| ach_assassin_lvl10 | **Shadowed Path** | Reach level 10 with the Assassin. | (parallel) |
| ach_assassin_lvl20 | **Phantom** | Reach level 20 with the Assassin. | (parallel) |
| ach_mage_lvl10 | **Arcane Initiate** | Reach level 10 with the Mage. | (parallel) |
| ach_mage_lvl20 | **Archmage** | Reach level 20 with the Mage. | (parallel) |

---

## Content milestones (8)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_outpost_first | **Liberator** | Clear your first outpost encounter. | Outpost OnEncounterComplete, first time. |
| ach_elite_first | **Giant Slayer** | Defeat your first elite enemy. | EliteArena OnEncounterComplete, first time. |
| ach_wave_10 | **Wave Walker** | Survive to Wave 10 in Wave Mode. | RunManager during WaveMode, wave counter >= 10. |
| ach_wave_25 | **Wave Master** | Survive to Wave 25 in Wave Mode. | (parallel, >= 25) |
| ach_extract | **Wise Departure** | Choose to extract instead of entering Wave Mode after a boss kill. | RunManager.HandleExtract while state == BossDefeated. |
| ach_collect_15 | **Collector** | Accumulate 15 unique items in a single run. | InventoryComponent.GetItemCount when CountUniqueItems(OwnedItems) reaches 15. |
| ach_max_stack | **Stockpile** | Own 5 of the same item in a single run. | Any item count >= 5. |
| ach_curse_pact | **Pact Made** | Accept your first curse from a shrine. | First shrine curse applied. |

---

## Difficulty achievements (8)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_easy_clear | **Wanderer's Journey** | Extract from a run on Easy difficulty. | RunComplete on Wanderer tier. |
| ach_normal_clear | **Adventurer's Victory** | Extract from a run on Normal difficulty. | RunComplete on Adventurer tier. |
| ach_hard_clear | **Hero's Triumph** | Extract from a run on Hard difficulty. | RunComplete on Hero tier. |
| ach_nightmare_clear | **Legend Reborn** | Extract from a run on Nightmare difficulty. | RunComplete on Nightmare tier. |
| ach_no_death | **Untouchable** | Complete a run on Normal+ without dying. (Counts as run extract.) | RunComplete + DeathCountThisRun == 0 + difficulty >= Adventurer. |
| ach_speedrun | **Lightning Strike** | Complete a Normal+ run in under 25 minutes. | RunComplete + RunDuration < 1500 + difficulty >= Adventurer. |
| ach_no_items | **Skill Diff** | Defeat the boss on Normal+ without buying any items. | BossDefeated + InventoryItemsBoughtThisRun == 0 + difficulty >= Adventurer. |
| ach_pacifist | **No Coin Spent** | Defeat the boss on Normal+ without spending any gold. | BossDefeated + GoldSpentThisRun == 0 + difficulty >= Adventurer. |

---

## Combat mastery (8)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_crit_streak | **Critical Streak** | Land 10 crits in a row. | Internal counter in HealthComponent.OnDamageReceived; reset on non-crit. |
| ach_shatter | **Glass-Hearted** | Trigger the Shatter combo (Burn + Chill). | First Shatter combo event. |
| ach_plague | **Plaguebringer** | Trigger the Plague combo (Poison + Curse). | First Plague combo event. |
| ach_all_combos | **Master of Synergy** | Trigger every chain reaction combo at least once. | All 6 combo events fired. |
| ach_status_30 | **Status Stack** | Have 30+ active status effects on enemies simultaneously. | Frame check across all enemies (debounced 0.5s). |
| ach_one_shot | **One-Shot Wonder** | Defeat any non-boss enemy in a single hit dealing 1000+ damage. | OnDamageReceived event with FinalDamage >= 1000 + target.IsDead(). |
| ach_dodge_master | **Dance of Steel** | Successfully dodge 10 boss telegraphed attacks in a single boss fight. | Counter increments on dodge during boss telegraph; >=10 in single fight. |
| ach_perfect_combo | **Flawless** | Complete a totem encounter without taking damage. | Track damage taken during encounter; if 0 on completion → award. |

---

## Currency / Economy (4)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_gold_10k | **Penny Pincher** | Earn 10,000 gold across all runs. | Lifetime gold counter on ProfileSave reaches 10000. |
| ach_gold_100k | **Wealthy** | Earn 100,000 gold across all runs. | Lifetime gold counter reaches 100000. |
| ach_legendary | **Mythic Find** | Own a Legendary item. | Inventory contains an item with Rarity == Legendary. |
| ach_full_set | **Connoisseur** | Own every Common item simultaneously in one run. | All 10 Common item IDs present in InventoryComponent.OwnedItems. |

---

## Skill tree / build (4)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_first_skill | **First Step Forward** | Invest your first skill point. | First successful SkillTreeComponent.Invest. |
| ach_skill_max | **Path Walker** | Fully invest one entire skill tree. | GetTotalInvestedInTree == tree.MaxInvestedInTree. |
| ach_respec | **Second Thoughts** | Use the free respec feature. | First SkillTreeComponent.RefundAll call. |
| ach_subclass | **Class Evolved** | Pick your first subclass at level 20. | Subclass selection event (post-Steam-EA). |

---

## Hidden / Discovery (4)

| Id | Name | Description | Trigger |
|---|---|---|---|
| ach_hidden_lore | **Secrets Found** | Find your first hidden lore tablet. | LoreTablet pickup event (post-funding content). |
| ach_all_classes | **Versatile** | Win a run with all 4 classes. | RunComplete fired for Warrior/Archer/Assassin/Mage. |
| ach_meta_grizzlebark | **Bark Bigger Than Bite** | Defeat Mega Grizzlebark on Hard+ difficulty. | BossDefeated + difficulty >= Hero. |
| ach_completionist | **Done It All** | Earn every other achievement. | All other 47 unlocked. |

---

## Implementation notes

### Steamworks side

- Each achievement gets a 64×64 icon (locked grey + unlocked color version)
- Hidden achievements should be flagged as `Hidden = true` in Steamworks dashboard
- Stat counters tracked via Steamworks stats API (gold lifetime, etc.)

### C++ side

- `UOryxAchievementsSystem` UGameInstanceSubsystem
- Hooks into existing delegates (`OnDeath`, `OnLevelUp`, `OnEncounterComplete`, etc.)
- For lifetime counters, persist on `UOryxProfileSave`
- On unlock: `UGameplayStatics::SetAchievementProgress` (Steamworks online subsystem)

### Local testing

- Pre-Steam-integration: log achievement unlocks to console + write to `Saved/Achievements/Unlocked.json`
- After Steamworks integration: real Steam popups

### Difficulty fairness

Per `Design_Difficulty.md`: achievements marked "Normal+" check the active difficulty AND check ConsecutiveDeaths < 5 (so catch-up assistance doesn't auto-unlock prestige achievements).

### Hidden achievements display

Hidden achievement names + descriptions only revealed on unlock. Use icons like `???` and "????" until unlocked.

---

## Achievement count summary

- **48 total** achievements
- **8 easy** (every player gets) — surface accomplishments quickly
- **16 mid-tier** — progression through content
- **20 challenging** — combat mastery + difficulty
- **4 hidden** — Easter eggs / completionism

This count + distribution is in line with most successful roguelite Steam pages (Hades has ~50, RoR2 has ~93, Slay the Spire has ~95). Don't over-design here — 48 is plenty.
