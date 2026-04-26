#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OryxGameInstance.generated.h"

UCLASS()
class ORYX_API UOryxGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Class Select")
	TSubclassOf<APawn> SelectedPawnClass;
};
