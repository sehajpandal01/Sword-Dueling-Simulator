#include "StickmanGameMode.h"
#include "StickmanCharacter.h"
#include "Kismet/GameplayStatics.h"
AStickmanGameMode::AStickmanGameMode() {}
void AStickmanGameMode::BeginPlay(){ Super::BeginPlay(); CurrentZone = FZoneContext(); }
void AStickmanGameMode::GrantBossTechnique(AStickmanCharacter* Winner, FName TechniqueId)
{
    if (!Winner) return;
    Winner->SetZoneContext(CurrentZone);
    Winner->UnlockTechnique(TechniqueId);
    Winner->GainExperience(150);
}
