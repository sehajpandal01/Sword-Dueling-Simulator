#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ElementTypes.h"
#include "LegacySystem.h"
#include "StickmanGameMode.generated.h"

UCLASS()
class STICKMANFIGHT_API AStickmanGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AStickmanGameMode();
protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
    bool bTowerMode = false;

    UPROPERTY(BlueprintReadOnly, Category="Tower")
    int32 CurrentFloor = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower")
    int32 FloorsToWin = 10;

    UFUNCTION(BlueprintCallable, Category="Tower")
    void StartTowerRun();

    UFUNCTION(BlueprintCallable, Category="Tower")
    void AdvanceFloor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zone")
    FZoneContext CurrentZone;

    UFUNCTION(BlueprintCallable, Category="Zone")
    void SetZoneElement(EElement InElement, bool bInHiddenOpposite=false)
    {
        CurrentZone.ZoneElement = InElement;
        CurrentZone.bHiddenOppositeTrial = bInHiddenOpposite;
    }

    UFUNCTION(BlueprintCallable, Category="Boss")
    void GrantBossTechnique(class AStickmanCharacter* Winner, FName TechniqueId);

    // Death & Legacy integration
    UFUNCTION(BlueprintCallable, Category="Legacy")
    void OnPlayerDeath(class AStickmanCharacter* DeadPlayer);

    UFUNCTION(BlueprintCallable, Category="Legacy")
    void GrantLegacyOnDeath(class AStickmanCharacter* DeadPlayer);

    // Shared legacy save
    UPROPERTY(BlueprintReadOnly, Category="Legacy")
    ULegacySaveGame* LegacySave = nullptr;
};
