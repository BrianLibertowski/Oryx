"""
generate_items.py — one-shot generator for the 32 Stage-1 UOryxItem DataAssets.

Run from inside the UE5 editor (Python Editor Script Plugin must be enabled):
    1. Tools -> Run Python Script... -> pick this file
    OR in Output Log Cmd window:
    2. py "C:/Users/Arzuh/Documents/Unreal Projects/Oryx/Content/Python/generate_items.py"

Outputs:
    /Game/Items/<Rarity>/<ItemId>.uasset  (one per row in ITEMS)

Re-runnable: existing assets at the same path are overwritten with the catalog values,
so the generator is the single source of truth — edit the ITEMS list, re-run.
"""

import unreal

# ---- Rarity / Op / Stat enums (mirrors C++) ----
RARITY = {
    "Common":    unreal.OryxItemRarity.COMMON,
    "Uncommon":  unreal.OryxItemRarity.UNCOMMON,
    "Rare":      unreal.OryxItemRarity.RARE,
    "Epic":      unreal.OryxItemRarity.EPIC,
    "Legendary": unreal.OryxItemRarity.LEGENDARY,
}

ADD = unreal.OryxModOp.ADDITIVE
MUL = unreal.OryxModOp.MULTIPLICATIVE

S = unreal.OryxStat  # alias for brevity

# ---- Catalog (32 items, matches Items.md) ----
# Each entry: (item_id, display_name, rarity, description, modifiers, icon_filename)
# modifiers: list of (stat, op, value) tuples
ITEMS = [
    # === Common (10) ===
    ("Potion_06",                "Lifebloom Sap",       "Common",
     "+25 Health",
     [(S.MAX_HEALTH, ADD, 25.0)],
     "Potion_06"),

    ("Blacksmith_18_iron_patch", "Iron Token",          "Common",
     "+10 Armour",
     [(S.ARMOR, ADD, 10.0)],
     "Blacksmith_18_iron_patch"),

    ("Tailoring_24_socks",       "Pebble of the Quick", "Common",
     "+7.5% Walk Speed",
     [(S.MOVEMENT_SPEED, MUL, 0.075)],
     "Tailoring_24_socks"),

    ("Alchemy_01_tea",           "Stitched Cowl",       "Common",
     "+25 Stamina",
     [(S.MAX_STAMINA, ADD, 25.0)],
     "Alchemy_01_tea"),

    ("Jewelry_28_aztec_ring",    "Bramblepouch",        "Common",
     "+5% Critical Chance",
     [(S.CRIT_CHANCE, ADD, 0.05)],
     "Jewelry_28_aztec_ring"),

    ("skinning_47_wildteeth",    "Wildteeth",           "Common",
     "+10% Damage",
     [(S.DAMAGE, ADD, 0.10)],
     "skinning_47_wildteeth"),

    ("Alchemy_02_mortar",        "Spark Ring",          "Common",
     "+10% Magic Damage",
     [(S.MAGIC_DAMAGE, ADD, 0.10)],
     "Alchemy_02_mortar"),

    ("skinning_50_fin",          "Hunter's Whetstone",  "Common",
     "+15% Physical Power",
     [(S.PHYSICAL_POWER, ADD, 0.15)],
     "skinning_50_fin"),

    ("Jewelry_30_green_ring",    "Smoke Vial",          "Common",
     "+5% Attack Speed",
     [(S.ATTACK_SPEED, MUL, 0.05)],
     "Jewelry_30_green_ring"),

    ("Cloth11_Head",             "Wickwood Charm",      "Common",
     "+5% Cast Speed",
     [(S.CAST_SPEED, MUL, 0.05)],
     "Cloth11_Head"),

    # === Uncommon (7) ===
    ("Relic_v2_08",                "Hexspark",             "Uncommon",
     "+15% Fire, +5% Crit Chance",
     [(S.FIRE_DAMAGE, ADD, 0.15), (S.CRIT_CHANCE, ADD, 0.05)],
     "Relic_v2_08"),

    ("Enchantment_52_rune_stoune", "Frostbite Locket",     "Uncommon",
     "+15% Cold, +10% Crit Damage",
     [(S.COLD_DAMAGE, ADD, 0.15), (S.CRIT_DAMAGE, ADD, 0.10)],
     "Enchantment_52_rune_stoune"),

    ("Enchantment_54_rune_stoune", "Toxin Brand",          "Uncommon",
     "+15% Poison, +10% Damage",
     [(S.POISON_DAMAGE, ADD, 0.15), (S.DAMAGE, ADD, 0.10)],
     "Enchantment_54_rune_stoune"),

    ("Enchantment_50_rune_stoune", "Plagueheart",          "Uncommon",
     "+15% Curse, +25 Max Health",
     [(S.CURSE_DAMAGE, ADD, 0.15), (S.MAX_HEALTH, ADD, 25.0)],
     "Enchantment_50_rune_stoune"),

    ("GoldCoin",                   "Coin of the Vagabond", "Uncommon",
     "+7.5% Luck",
     [(S.LUCK, ADD, 0.075)],
     "GoldCoin"),

    ("Loot_62_bag",                "Lucky Bone",           "Uncommon",
     "+10% Gold Gain",
     [(S.GOLD_GAIN, ADD, 0.10)],
     "Loot_62_bag"),

    ("Bracer_06",                  "Dasher's Brace",       "Uncommon",
     "+5% Crit, +5% Crit Damage",
     [(S.CRIT_CHANCE, ADD, 0.05), (S.CRIT_DAMAGE, ADD, 0.05)],
     "Bracer_06"),

    # === Rare (7) ===
    ("Blacksmith_19_flake_patch",     "Bulwark Sigil",   "Rare",
     "+50 Health, +5 Health Regen",
     [(S.MAX_HEALTH, ADD, 50.0), (S.HEALTH_REGEN, ADD, 5.0)],
     "Blacksmith_19_flake_patch"),

    ("Enchantment_39_mana_scroll",    "Mage's Tome",     "Rare",
     "+50 Mana, +5 Mana Regen, +10% Cast Speed",
     [(S.MAX_MANA, ADD, 50.0), (S.MANA_REGEN, ADD, 5.0), (S.CAST_SPEED, MUL, 0.10)],
     "Enchantment_39_mana_scroll"),

    ("Blacksmith_33_magic_horseshoe", "Wildlung Charm",  "Rare",
     "+10% Move Speed, +10 Stamina Regen",
     [(S.MOVEMENT_SPEED, MUL, 0.10), (S.STAMINA_REGEN, ADD, 10.0)],
     "Blacksmith_33_magic_horseshoe"),

    ("Jewelry_49_blackironneck",      "Stalker's Eye",   "Rare",
     "+15% Crit Chance, +10% Crit Damage",
     [(S.CRIT_CHANCE, ADD, 0.15), (S.CRIT_DAMAGE, ADD, 0.10)],
     "Jewelry_49_blackironneck"),

    ("Jewelry_50_skullneck",          "Bloodletter Edge","Rare",
     "+10% all elemental",
     [(S.FIRE_DAMAGE, ADD, 0.10), (S.COLD_DAMAGE, ADD, 0.10),
      (S.MAGIC_DAMAGE, ADD, 0.10), (S.CURSE_DAMAGE, ADD, 0.10),
      (S.POISON_DAMAGE, ADD, 0.10)],
     "Jewelry_50_skullneck"),

    ("Enchantment_61_runeponeti",     "Reaper's Token",  "Rare",
     "+20% Crit Chance, -25% Max Health",
     [(S.CRIT_CHANCE, ADD, 0.20), (S.MAX_HEALTH, MUL, -0.25)],
     "Enchantment_61_runeponeti"),

    ("Jewelry_60_immortal_neck",      "Gwand Wazard",    "Rare",
     "+10% Damage Reduction",
     [(S.DAMAGE_REDUCTION, ADD, 0.10)],
     "Jewelry_60_immortal_neck"),

    # === Epic (5) ===
    ("Res_86",                     "Heart of the Mountain", "Epic",
     "+100 Health, +25 Armour, +10% Damage Reduction",
     [(S.MAX_HEALTH, ADD, 100.0), (S.ARMOR, ADD, 25.0), (S.DAMAGE_REDUCTION, ADD, 0.10)],
     "Res_86"),

    ("Jewelry_57_mystic_neck",     "Cinderlung",            "Epic",
     "+25% Fire, +15% Cast Speed, +10% CDR",
     [(S.FIRE_DAMAGE, ADD, 0.25), (S.CAST_SPEED, MUL, 0.15), (S.COOLDOWN_REDUCTION, ADD, 0.10)],
     "Jewelry_57_mystic_neck"),

    ("Enchantment_58_runecrystal", "Phoenix Hide",          "Epic",
     "+50 Health, +10 Health Regen, +5% Crit",
     [(S.MAX_HEALTH, ADD, 50.0), (S.HEALTH_REGEN, ADD, 10.0), (S.CRIT_CHANCE, ADD, 0.05)],
     "Enchantment_58_runecrystal"),

    ("Relic_v2_07",                "Stormbearer",           "Epic",
     "+30% Magic, +10% Attack Speed, +5% CDR",
     [(S.MAGIC_DAMAGE, ADD, 0.30), (S.ATTACK_SPEED, MUL, 0.10), (S.COOLDOWN_REDUCTION, ADD, 0.05)],
     "Relic_v2_07"),

    ("Relic_v2_02",                "Glasscannon Sigil",     "Epic",
     "+50% Damage, -50% Max Health",
     [(S.DAMAGE, ADD, 0.50), (S.MAX_HEALTH, MUL, -0.50)],
     "Relic_v2_02"),

    # === Legendary (3) ===
    ("Blacksmith_60_magic_chest", "Chest of Malicious Greed", "Legendary",
     "+50% Gold Gain, +25% Luck",
     [(S.GOLD_GAIN, ADD, 0.50), (S.LUCK, ADD, 0.25)],
     "Blacksmith_60_magic_chest"),

    ("Quest_141_flag",            "Last Stand",              "Legendary",
     "+50% Damage, -25% Max Health",
     [(S.DAMAGE, ADD, 0.50), (S.MAX_HEALTH, MUL, -0.25)],
     "Quest_141_flag"),

    ("Book_v2_39",                "Soul Ledger",             "Legendary",
     "+50% Damage, +25% Crit Damage",
     [(S.DAMAGE, ADD, 0.50), (S.CRIT_DAMAGE, ADD, 0.25)],
     "Book_v2_39"),
]


def find_icon_path(icon_filename, rarity):
    """Locate the icon UTexture2D path. Tries the rarity subfolder first, then loose root."""
    candidates = [
        f"/Game/UI/ItemIcons/{rarity}/{icon_filename}",
        f"/Game/UI/ItemIcons/{icon_filename}",
    ]
    for path in candidates:
        if unreal.EditorAssetLibrary.does_asset_exist(path):
            return path
    return None


def make_modifier(stat, op, value):
    mod = unreal.OryxStatModifier()
    mod.set_editor_property("stat", stat)
    mod.set_editor_property("op", op)
    mod.set_editor_property("value", float(value))
    # SourceId left invalid (zero GUID) — InventoryComponent generates a fresh one at apply time.
    return mod


def create_item(item_id, display_name, rarity_name, description, modifier_tuples, icon_filename):
    rarity_enum = RARITY[rarity_name]
    package_path = f"/Game/Items/{rarity_name}"
    asset_name = item_id

    # Delete existing so re-runs are clean (avoids partial state from old runs).
    existing_path = f"{package_path}/{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(existing_path):
        unreal.EditorAssetLibrary.delete_asset(existing_path)

    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", unreal.OryxItem)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_asset = asset_tools.create_asset(asset_name, package_path, unreal.OryxItem, factory)
    if new_asset is None:
        unreal.log_error(f"[items] Failed to create {asset_name}")
        return False

    new_asset.set_editor_property("item_id", unreal.Name(item_id))
    new_asset.set_editor_property("display_name", display_name)
    new_asset.set_editor_property("description", description)
    new_asset.set_editor_property("rarity", rarity_enum)

    # Icon: load and assign as TSoftObjectPtr (assigning the loaded object works in UE5 Python).
    icon_path = find_icon_path(icon_filename, rarity_name)
    if icon_path:
        icon_tex = unreal.EditorAssetLibrary.load_asset(icon_path)
        if icon_tex:
            new_asset.set_editor_property("icon", icon_tex)
    else:
        unreal.log_warning(f"[items] Icon not found for {item_id} (looked under /Game/UI/ItemIcons/{rarity_name}/)")

    mods = [make_modifier(stat, op, value) for (stat, op, value) in modifier_tuples]
    new_asset.set_editor_property("stat_modifiers", mods)

    unreal.EditorAssetLibrary.save_loaded_asset(new_asset)
    return True


def main():
    ok = 0
    fail = 0
    for entry in ITEMS:
        if create_item(*entry):
            ok += 1
        else:
            fail += 1
    unreal.log(f"[items] Generated {ok} items ({fail} failed). Total {len(ITEMS)}.")


main()
