#include "LegacySystem.h"
#include "Kismet/GameplayStatics.h"

static const FString SLOT_NAME = TEXT("LegacySlot");
static const int32 USER_IDX = 0;

ULegacySaveGame* ULegacySystem::LoadOrCreateLegacy(UObject* WorldContextObject)
{
    if (UGameplayStatics::DoesSaveGameExist(SLOT_NAME, USER_IDX))
    {
        return Cast<ULegacySaveGame>(UGameplayStatics::LoadGameFromSlot(SLOT_NAME, USER_IDX));
    }
    ULegacySaveGame* Save = Cast<ULegacySaveGame>(UGameplayStatics::CreateSaveGameObject(ULegacySaveGame::StaticClass()));
    UGameplayStatics::SaveGameToSlot(Save, SLOT_NAME, USER_IDX);
    return Save;
}

void ULegacySystem::SaveLegacy(ULegacySaveGame* Save)
{
    if (!Save) return;
    UGameplayStatics::SaveGameToSlot(Save, SLOT_NAME, USER_IDX);
}

void ULegacySystem::GrantLegacy(ULegacySaveGame* Save, int32 Points, int32 STR, int32 DEX, int32 END, int32 VIT, int32 FOC)
{
    if (!Save) return;
    Save->TotalLegacyPoints += Points;
    Save->BonusSTR += STR;
    Save->BonusDEX += DEX;
    Save->BonusEND += END;
    Save->BonusVIT += VIT;
    Save->BonusFOC += FOC;
    SaveLegacy(Save);
}
