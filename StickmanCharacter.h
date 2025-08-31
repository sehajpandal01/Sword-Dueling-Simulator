#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ElementTypes.h"
#include "StickmanCharacter.generated.h"
UENUM(BlueprintType) enum class EFightState : uint8 { Idle, Moving, Attacking, Blocking, Stunned, Dead };
UENUM(BlueprintType) enum class EAttackType : uint8 { LightAttack, HeavyAttack, ThrustAttack };
UCLASS()
class STICKMANFIGHT_API AStickmanCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AStickmanCharacter();
protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
    virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float Health = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float MaxHealth = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float AttackDamage = 20.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float AttackRange = 150.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float AttackCooldown = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") float BlockReduction = 0.5f;

    UPROPERTY(BlueprintReadOnly, Category="State") EFightState CurrentState = EFightState::Idle;
    UPROPERTY(BlueprintReadOnly, Category="State") bool bIsBlocking = false;
    UPROPERTY(BlueprintReadOnly, Category="State") bool bCanAttack = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Experience") int32 Level = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Experience") int32 Experience = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Experience") int32 ExperienceToNextLevel = 100;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Experience") float LevelUpHealthBonus = 20.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Experience") float LevelUpDamageBonus = 5.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* SwordMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* HeadMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* BodyMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* LeftArmMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* RightArmMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* LeftLegMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") class UStaticMeshComponent* RightLegMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audio") class UAudioComponent* AudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true")) class USpringArmComponent* CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true")) class UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") int32 STR=5, DEX=5, END=5, VIT=5, FOC=5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") float MaxStamina=100.f;
    UPROPERTY(BlueprintReadOnly, Category="Stats") float Stamina=100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") float StaminaRegenPerSec=15.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") float StaminaCostLight=8.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") float StaminaCostHeavy=16.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats") float StaminaCostBlockPerHit=10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Element") EElement Affinity = EElement::Earth;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Element") EElement Weakness = EElement::Fire;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Element") EElement CurrentZoneElement = EElement::Earth;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Element") bool bHiddenOppositeTrial = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura") float BaseAuraIntensity=0.2f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aura") float AuraIntensity=0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura") float AuraPerLevel=0.008f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|VFX") class UMaterialParameterCollection* AuraMPC=nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Techniques") TSet<FName> UnlockedTechniques;

    UFUNCTION(BlueprintCallable, Category="Combat") void Attack(EAttackType AttackType);
    UFUNCTION(BlueprintCallable, Category="Combat") void StartBlocking();
    UFUNCTION(BlueprintCallable, Category="Combat") void StopBlocking();
    UFUNCTION(BlueprintCallable, Category="Combat") void TakeDamage(float Damage, AStickmanCharacter* Attacker);
    UFUNCTION(BlueprintCallable, Category="Combat") void Die();

    UFUNCTION(BlueprintCallable, Category="Experience") void GainExperience(int32 XP);
    UFUNCTION(BlueprintCallable, Category="Experience") void LevelUp();
    UFUNCTION(BlueprintPure,   Category="Experience") float GetHealthPercentage() const;
    UFUNCTION(BlueprintPure,   Category="Experience") float GetExperiencePercentage() const;
    UFUNCTION(BlueprintPure,   Category="Experience") float GetCurrentXPMultiplier() const;

    UFUNCTION(BlueprintCallable, Category="Stats") void AllocateStatPoints(int32 InSTR, int32 InDEX, int32 InEND, int32 InVIT, int32 InFOC);
    UFUNCTION(BlueprintCallable, Category="Element") void SetZoneContext(const FZoneContext& ctx);
    UFUNCTION(BlueprintCallable, Category="Techniques") void UnlockTechnique(FName TechniqueId);
    UFUNCTION(BlueprintCallable, Category="Aura") void RecomputeAura();

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

private:
    void PerformAttack(EAttackType AttackType);
    void CheckForHit();
    void ResetAttackCooldown();
    void SetupStickmanMesh();
    void UpdateStatsForLevel();
    void PlaySwordSound(class USoundCue* SoundToPlay);
    void SetupAudioComponent();
    FTimerHandle AttackCooldownTimer, AttackDurationTimer;
    UPROPERTY() class AStickmanGameMode* GameMode;
    float LastAttackTime = 0.f;
    EAttackType CurrentAttackType;
    float BaseMaxHealth = 100.f;
    float BaseAttackDamage = 20.f;
};
