#pragma once
#include "CoreMinimal.h"
#include "ElementTypes.generated.h"

UENUM(BlueprintType)
enum class EElement : uint8
{
    Fire, Water, Earth, Air, Lightning, Shadow, Light
};

USTRUCT(BlueprintType)
struct FZoneContext
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EElement ZoneElement = EElement::Earth;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHiddenOppositeTrial = false;
};

inline EElement GetWeakness(EElement E)
{
    switch (E)
    {
    case EElement::Fire:      return EElement::Water;
    case EElement::Water:     return EElement::Lightning;
    case EElement::Earth:     return EElement::Fire;
    case EElement::Air:       return EElement::Earth;
    case EElement::Lightning: return EElement::Air;
    default:                  return EElement::Earth;
    }
}
inline bool IsHidden(EElement E){ return (E==EElement::Shadow||E==EElement::Light); }
inline float ComputeXPMultiplier(EElement PlayerAffinity, EElement ZoneElem, bool bHiddenOppositeTrial)
{
    if (bHiddenOppositeTrial) return 0.75f;
    if (ZoneElem == PlayerAffinity) return 1.15f;
    if (ZoneElem == GetWeakness(PlayerAffinity)) return 0.85f;
    return 1.0f;
}
