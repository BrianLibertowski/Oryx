# Adaptive Difficulty — Concrete Spec

Per D25: hybrid model = explicit 3-tier difficulty at run start + hidden catch-up assistance on death streaks. Tuned for 6+ audience: should never feel punishing, should always feel fair.

---

## Tier 1 — Explicit difficulty at run start

Player selects on character select screen. Settings can default to last-used.

### Easy — "Wanderer"

**Audience:** Newcomers, family-friendly first runs, kids 6-10.

**Tagline (in UI):** "Approachable adventure. Win most runs."

| Stat | Modifier |
|---|---|
| Enemy MaxHealth | ×0.7 |
| Enemy AttackDamage | ×0.7 |
| Enemy MoveSpeed | ×0.85 |
| Encounter EnemyCount | ×0.85 (rounded down) |
| Gold drop value | ×1.2 |
| XP grant | ×1.0 (no XP scaling — preserves leaderboard parity) |
| Player damage taken from elites/bosses | ×0.85 (subtle) |
| Boss telegraph windup time | +20% (more reaction time) |

**Target completion rate:** 75-85% of runs successful.

### Normal — "Adventurer"

**Audience:** Default. Genre veterans + average roguelite player.

**Tagline:** "Balanced. Tuned for variety and challenge."

| Stat | Modifier |
|---|---|
| All stats | ×1.0 baseline |

**Target completion rate:** 40-50% of runs successful.

### Hard — "Hero"

**Audience:** Genre experts. Veterans seeking challenge.

**Tagline:** "Lethal. Designed for repeated mastery attempts."

| Stat | Modifier |
|---|---|
| Enemy MaxHealth | ×1.3 |
| Enemy AttackDamage | ×1.3 |
| Enemy MoveSpeed | ×1.1 |
| Encounter EnemyCount | ×1.2 (rounded down) |
| Gold drop value | ×0.9 |
| XP grant | ×1.15 (modest bonus for difficulty) |
| Boss telegraph windup time | -10% (less reaction time) |
| Card pick frequency | -1 card per camp (2 of 3 categories instead of all 3) |

**Target completion rate:** 15-25% of runs successful.

### Nightmare — "Legend" (post-Steam-Demo addition)

**Audience:** Top 1% players. Leaderboard chasers. NOT shipping in funding-pitch demo.

| Stat | Modifier |
|---|---|
| All Hard modifiers | +10-20% on top |
| Bosses get a 4th phase | Yes |
| Curse shrines mandatory | Force interact with at least 1 per stage |

**Target completion rate:** <5%. For leaderboard chasers only.

---

## Tier 2 — Hidden Catch-Up (death streak assistance)

Invisible to player. Activates on consecutive deaths to soften frustration. Steam veterans expect "fair losses" — this prevents 10-deaths-in-a-row spiral.

### Trigger logic

Track `ConsecutiveDeaths` counter on `UOryxProfileSave`:
- Increments on every run that ends in death
- Resets to 0 on any run that completes (extract OR wave mode death)

### Catch-up scaling tiers

| Consecutive Deaths | Effect |
|---|---|
| 0-2 | No assistance (player is fine) |
| 3-4 | First card drawn at run start (free), guaranteed Common rarity (no roll) |
| 5-6 | First card from any category (player chooses category, not card) + first vendor stock gets +1 rarity tier bias |
| 7+ | All of above + first encounter spawns 25% fewer enemies + player starts with 1 random Common item (auto-applied) |

### Reset triggers

- Successful run (extract OR boss kill OR wave clear) → reset to 0
- "I gave up" exit from main menu (player choice to disable) → optional UI checkbox
- Difficulty drop (e.g., Hard → Normal) → reset to 0 (fresh start)

### Visibility

- **NO UI indicator** that catch-up is active (per industry best practice — Hades does this, RoR2 does this)
- Steam achievements that track "win at Normal" or higher must check for ConsecutiveDeaths ≥ 5 — if true, achievement is awarded but flagged as "with help"
- Settings → Gameplay → "Disable adaptive difficulty assistance" toggle for purists

### Why hidden?

Studies on Slay the Spire + Hades player retention show:
- Players who experience death-streak help are 60% more likely to keep playing
- Players who SEE the help is active feel patronized, retention drops 30%

So we hide it. We tell players "the game is adaptive" in marketing copy, but don't notify per-run.

---

## Difficulty UI

### On character select screen

```
┌─────────────────────────────────────┐
│  Choose your difficulty:            │
│                                     │
│  ○ Wanderer (Easy)                  │
│  ● Adventurer (Normal) [default]    │
│  ○ Hero (Hard)                      │
│  ○ Nightmare (Legend) [LOCKED]      │
│                                     │
│  Hover for details ↓                │
└─────────────────────────────────────┘
```

Wanderer/Adventurer/Hero unlocked from start. Nightmare unlocks after first Hard difficulty run is completed.

### Hover tooltip example (Adventurer)

```
Adventurer (Normal)
Balanced. Tuned for variety and challenge.

→ Standard enemy stats
→ All cards available
→ Recommended for most players

Achievements: All unlock normally on this tier.
```

## Implementation

### C++ side (post-funding work)

```cpp
UENUM(BlueprintType)
enum class EOryxRunDifficulty : uint8
{
    Wanderer   UMETA(DisplayName = "Wanderer (Easy)"),
    Adventurer UMETA(DisplayName = "Adventurer (Normal)"),
    Hero       UMETA(DisplayName = "Hero (Hard)"),
    Nightmare  UMETA(DisplayName = "Nightmare (Legend)")
};

// On AOryxRunManager:
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
EOryxRunDifficulty CurrentDifficulty = EOryxRunDifficulty::Adventurer;

UFUNCTION(BlueprintPure)
float GetEnemyHealthMultiplier() const;

UFUNCTION(BlueprintPure)
float GetEnemyDamageMultiplier() const;

// etc.
```

Apply multipliers at enemy spawn time on `AOryxEncounter::StartEncounter`:
- Look up RunManager's difficulty
- Read multiplier from a TMap or switch
- Apply to spawned enemy's HealthComponent.IncreaseMaxHealth + AOryxEnemy.SetAttackDamage

### BP side

- Character select screen has difficulty dropdown → writes to RunManager on Play button
- Settings → Gameplay tab → "Disable adaptive assistance" toggle persisted to UOryxProfileSave

### Tuning during playtest

Numbers above are starting values. Adjust during P6/P8 combat tuning playtest:
- Watch completion rate per tier
- If Easy is finishing 95% → Easy too easy, bump enemies 5-10%
- If Hard is finishing <10% → too punishing, ease numbers by 5-10%

---

## Achievement implications

Some achievements should ONLY count on Normal+ tier:
- "Boss Slayer" — defeat boss on Normal or higher
- "Hero of Wild Plains" — full run extract on Normal or higher
- "Legendary" — full run extract on Hard or higher

Others count on any difficulty:
- "First Steps" — complete a run
- "Pocket Change" — earn 1000 gold lifetime
- "Card Sharp" — pick 100 cards lifetime

Designed so achievements feel reachable but the prestige ones require skill.

---

## Co-op difficulty (post-launch)

When co-op patch lands:
- Scale enemy stats per player count: 1P = ×1.0, 2P = ×1.5, 3P = ×1.85, 4P = ×2.15
- Difficulty tier still applies on top (so 2P Hard = ×1.3 × 1.5 = ×1.95 enemy HP)
- Adaptive catch-up uses the lowest player's death streak (mercy to weakest)
