#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StickmanHUD.generated.h"

UCLASS()
class STICKMANFIGHT_API UStickmanHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayer1Health(float HealthPercentage);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayer1Experience(float ExperiencePercentage, int32 Level);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayer2Health(float HealthPercentage);
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePlayer2Experience(float ExperiencePercentage, int32 Level);

    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void UpdatePlayer1Stamina(float StaminaPct, float AuraIntensity, float XPMultiplier);
    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void UpdatePlayer2Stamina(float StaminaPct, float AuraIntensity, float XPMultiplier);
};
