#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/SaveGame.h"
#include "LegacySystem.generated.h"
UCLASS()
class STICKMANFIGHT_API ULegacySaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 TotalLegacyPoints = 0;
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 BonusSTR = 0;
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 BonusDEX = 0;
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 BonusEND = 0;
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 BonusVIT = 0;
    UPROPERTY(VisibleAnywhere, Category="Legacy")
    int32 BonusFOC = 0;
};
UCLASS()
class STICKMANFIGHT_API ULegacySystem : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Legacy")
    static ULegacySaveGame* LoadOrCreateLegacy(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category="Legacy")
    static void SaveLegacy(ULegacySaveGame* Save);

    UFUNCTION(BlueprintCallable, Category="Legacy")
    static void GrantLegacy(ULegacySaveGame* Save, int32 Points, int32 STR=0, int32 DEX=0, int32 END=0, int32 VIT=0, int32 FOC=0);
};
