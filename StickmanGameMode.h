#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ElementTypes.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zone") FZoneContext CurrentZone;
    UFUNCTION(BlueprintCallable, Category="Zone")
    void SetZoneElement(EElement InElement, bool bInHiddenOpposite=false)
    { CurrentZone.ZoneElement = InElement; CurrentZone.bHiddenOppositeTrial = bInHiddenOpposite; }

    UFUNCTION(BlueprintCallable, Category="Boss")
    void GrantBossTechnique(class AStickmanCharacter* Winner, FName TechniqueId);
};
