#include "StickmanCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "StickmanGameMode.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
AStickmanCharacter::AStickmanCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->SetCapsuleSize(42.f, 96.0f);
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    SetupStickmanMesh();
    SetupAudioComponent();

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.f;
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    BaseMaxHealth = MaxHealth;
    BaseAttackDamage = AttackDamage;
}
void AStickmanCharacter::SetupStickmanMesh()
{
    HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(RootComponent);
    HeadMesh->SetRelativeLocation(FVector(0, 0, 70));
    HeadMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootComponent);
    BodyMesh->SetRelativeLocation(FVector(0, 0, 20));
    BodyMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 1.0f));

    LeftArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmMesh"));
    LeftArmMesh->SetupAttachment(BodyMesh);
    LeftArmMesh->SetRelativeLocation(FVector(-15, 0, 20));
    LeftArmMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.6f));

    RightArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmMesh"));
    RightArmMesh->SetupAttachment(BodyMesh);
    RightArmMesh->SetRelativeLocation(FVector(15, 0, 20));
    RightArmMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.6f));

    LeftLegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegMesh"));
    LeftLegMesh->SetupAttachment(BodyMesh);
    LeftLegMesh->SetRelativeLocation(FVector(-10, 0, -40));
    LeftLegMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.8f));

    RightLegMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegMesh"));
    RightLegMesh->SetupAttachment(BodyMesh);
    RightLegMesh->SetRelativeLocation(FVector(10, 0, -40));
    RightLegMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.8f));

    SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
    SwordMesh->SetupAttachment(RightArmMesh);
    SwordMesh->SetRelativeLocation(FVector(0, 0, -30));
    SwordMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 1.5f));
}
void AStickmanCharacter::SetupAudioComponent()
{
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;
}
void AStickmanCharacter::BeginPlay()
{
    Super::BeginPlay();
    GameMode = Cast<AStickmanGameMode>(UGameplayStatics::GetGameMode(this));
    Weakness = GetWeakness(Affinity);

    Health = MaxHealth;
    CurrentState = EFightState::Idle;
    UpdateStatsForLevel();
    RecomputeAura();
}

void AStickmanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AStickmanCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AStickmanCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AStickmanCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AStickmanCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, [this]() { Attack(EAttackType::LightAttack); });
    PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, [this]() { Attack(EAttackType::HeavyAttack); });
    PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AStickmanCharacter::StartBlocking);
    PlayerInputComponent->BindAction("Block", IE_Released, this, &AStickmanCharacter::StopBlocking);
}

void AStickmanCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EFightState::Idle || CurrentState == EFightState::Moving)
    {
        CurrentState = (GetVelocity().Size() > 0.1f) ? EFightState::Moving : EFightState::Idle;
    }

    Stamina = FMath::Min(MaxStamina, Stamina + StaminaRegenPerSec * DeltaTime);
}
void AStickmanCharacter::MoveForward(float Value)
{
    if (CurrentState == EFightState::Attacking || CurrentState == EFightState::Stunned || CurrentState == EFightState::Dead)
        return;
    if (Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}
void AStickmanCharacter::MoveRight(float Value)
{
    if (CurrentState == EFightState::Attacking || CurrentState == EFightState::Stunned || CurrentState == EFightState::Dead)
        return;
    if (Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AStickmanCharacter::Turn(float Value)
{
    if (CurrentState == EFightState::Dead) return;
    AddControllerYawInput(Value);
}

void AStickmanCharacter::LookUp(float Value)
{
    if (CurrentState == EFightState::Dead) return;
    AddControllerPitchInput(Value);
}

void AStickmanCharacter::Attack(EAttackType AttackType)
{
    if (!bCanAttack || CurrentState == EFightState::Dead || CurrentState == EFightState::Stunned)
        return;

    const float Cost = (AttackType == EAttackType::HeavyAttack) ? StaminaCostHeavy : StaminaCostLight;
    if (Stamina < Cost) return;
    Stamina -= Cost;

    CurrentAttackType = AttackType;
    CurrentState = EFightState::Attacking;
    bCanAttack = false;
    PerformAttack(AttackType);

    const float BaseDur = (AttackType == EAttackType::HeavyAttack) ? 0.8f : 0.5f;
    const float DexScale = FMath::Clamp(1.f - (DEX * 0.01f), 0.7f, 1.f);
    const float AttackDuration = BaseDur * DexScale;

    GetWorldTimerManager().SetTimer(AttackDurationTimer, this, &AStickmanCharacter::ResetAttackCooldown, AttackDuration, false);
}

void AStickmanCharacter::PerformAttack(EAttackType AttackType)
{
    FRotator SwordRotation = SwordMesh->GetRelativeRotation();
    switch (AttackType)
    {
    case EAttackType::LightAttack:
        SwordRotation.Pitch += 45.0f;
        PlaySwordSound(LightAttackSound);
        break;
    case EAttackType::HeavyAttack:
        SwordRotation.Pitch += 90.0f;
        PlaySwordSound(HeavyAttackSound);
        break;
    case EAttackType::ThrustAttack:
        SwordMesh->SetRelativeLocation(SwordMesh->GetRelativeLocation() + FVector(0, 20, 0));
        PlaySwordSound(ThrustAttackSound);
        break;
    }
    SwordMesh->SetRelativeRotation(SwordRotation);
    PlaySwordSound(SwordWhooshSound);
    GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AStickmanCharacter::CheckForHit, 0.2f, false);
}

void AStickmanCharacter::CheckForHit()
{
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation + (GetActorForwardVector() * AttackRange);
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Pawn, QueryParams);
    if (bHit)
    {
        AStickmanCharacter* HitCharacter = Cast<AStickmanCharacter>(HitResult.GetActor());
        if (HitCharacter)
        {
            float Damage = AttackDamage;
            if (CurrentAttackType == EAttackType::HeavyAttack)
                Damage *= 1.5f;
            HitCharacter->TakeDamage(Damage, this);
            PlaySwordSound(SwordHitSound);
            GainExperience(10);
        }
    }
}

void AStickmanCharacter::ResetAttackCooldown()
{
    bCanAttack = true;
    CurrentState = EFightState::Idle;
    SwordMesh->SetRelativeRotation(FRotator::ZeroRotator);
    SwordMesh->SetRelativeLocation(FVector(0, 0, -30));
}

void AStickmanCharacter::StartBlocking()
{
    if (CurrentState == EFightState::Dead || CurrentState == EFightState::Attacking)
        return;
    bIsBlocking = true;
    CurrentState = EFightState::Blocking;
    SwordMesh->SetRelativeRotation(FRotator(0, 0, 45));
    PlaySwordSound(SwordBlockSound);
}

void AStickmanCharacter::StopBlocking()
{
    if (CurrentState == EFightState::Dead)
        return;
    bIsBlocking = false;
    CurrentState = EFightState::Idle;
    SwordMesh->SetRelativeRotation(FRotator::ZeroRotator);
}

void AStickmanCharacter::TakeDamage(float Damage, AStickmanCharacter* Attacker)
{
    if (CurrentState == EFightState::Dead)
        return;

    float FinalDamage = Damage;
    if (bIsBlocking)
    {
        FinalDamage *= BlockReduction;
        Stamina = FMath::Max(0.f, Stamina - StaminaCostBlockPerHit);
        PlaySwordSound(SwordBlockSound);
    }
    Health = FMath::Max(0.f, Health - FinalDamage);
    if (Health <= 0.f)
    {
        Die();
        if (Attacker) Attacker->GainExperience(50);
    }
    else
    {
        CurrentState = EFightState::Stunned;
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, [this]() { CurrentState = EFightState::Idle; }, 0.3f, false);
    }
}

void AStickmanCharacter::Die()
{
    CurrentState = EFightState::Dead;
    bCanAttack = false;
    bIsBlocking = false;
    PlaySwordSound(DeathSound);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorRotation(FRotator(0, GetActorRotation().Yaw, 90));
    if (GameMode) { GameMode->OnPlayerDeath(this); }
}

float AStickmanCharacter::GetCurrentXPMultiplier() const
{
    return ComputeXPMultiplier(Affinity, CurrentZoneElement, bHiddenOppositeTrial);
}

void AStickmanCharacter::GainExperience(int32 XP)
{
    const float Mult = GetCurrentXPMultiplier();
    Experience += FMath::RoundToInt(XP * Mult);
    while (Experience >= ExperienceToNextLevel) { LevelUp(); }
}

void AStickmanCharacter::LevelUp()
{
    Experience -= ExperienceToNextLevel;
    Level++;
    ExperienceToNextLevel = FMath::RoundToInt(ExperienceToNextLevel * 1.2f);
    UpdateStatsForLevel();
    RecomputeAura();
    PlaySwordSound(LevelUpSound);
    Health = MaxHealth;
    Stamina = MaxStamina;
}

void AStickmanCharacter::UpdateStatsForLevel()
{
    MaxHealth = 100.f + ((Level - 1) * 20.f) + (VIT * 20.f);
    AttackDamage = 20.f + ((Level - 1) * 5.f) + (STR * 2.f);
    MaxStamina = 100.f + END * 10.f;
    Stamina = FMath::Min(Stamina, MaxStamina);
}

float AStickmanCharacter::GetHealthPercentage() const
{
    return MaxHealth > 0 ? Health / MaxHealth : 0.0f;
}

float AStickmanCharacter::GetExperiencePercentage() const
{
    return ExperienceToNextLevel > 0 ? (float)Experience / (float)ExperienceToNextLevel : 0.0f;
}

void AStickmanCharacter::PlaySwordSound(USoundCue* SoundToPlay)
{
    if (SoundToPlay && AudioComponent)
    {
        AudioComponent->SetSound(SoundToPlay);
        AudioComponent->Play();
    }
}

void AStickmanCharacter::SetZoneContext(const FZoneContext& ctx)
{
    CurrentZoneElement   = ctx.ZoneElement;
    bHiddenOppositeTrial = ctx.bHiddenOppositeTrial;
}

void AStickmanCharacter::UnlockTechnique(FName TechniqueId)
{
    UnlockedTechniques.Add(TechniqueId);
}

void AStickmanCharacter::RecomputeAura()
{
    AuraIntensity = BaseAuraIntensity + AuraPerLevel * (Level - 1);
    AuraIntensity = FMath::Clamp(AuraIntensity, 0.f, 1.5f);
    if (AuraMPC)
    {
        if (UWorld* W = GetWorld())
        {
            if (UMaterialParameterCollectionInstance* Inst = W->GetParameterCollectionInstance(AuraMPC))
            {
                Inst->SetScalarParameterValue(TEXT("AuraIntensity"), AuraIntensity);
            }
        }
    }
}
