#pragma once

#include "CoreMinimal.h"
#include "Component/Stats/OryxStatTypes.h"
#include "OryxCardTypes.generated.h"

/**
 *  Card categories per D15. Strength / Magic / Agility — one card drawn from each on
 *  every camp clear. Crit Chance lives in BOTH Strength and Agility pools (intentional).
 */
UENUM(BlueprintType)
enum class EOryxCardCategory : uint8
{
	Strength UMETA(DisplayName = "Strength"),
	Magic    UMETA(DisplayName = "Magic"),
	Agility  UMETA(DisplayName = "Agility")
};

/**
 *  Data definition for a card option. Stateless — represents "what could be drawn".
 *  Applied cards become FOryxStatModifier entries on the player's StatsComponent,
 *  tagged with a SourceId in the "RunBuff" namespace for run-end cleanup.
 *
 *  Magnitude follows common-item tier per D15 (e.g. +25 HP, +5% crit, +10% damage).
 */
USTRUCT(BlueprintType)
struct ORYX_API FOryxCard
{
	GENERATED_BODY()

	/** Stable id for save/load + dedup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CardId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOryxCardCategory Category = EOryxCardCategory::Strength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;

	/** The stat modifier this card grants when picked. SourceId is generated at apply time. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FOryxStatModifier Modifier;
};
