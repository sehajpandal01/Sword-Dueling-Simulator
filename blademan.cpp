#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "StickmanCharacter.generated.h"

UENUM(BlueprintType)
enum class EFightState : uint8
{
    Idle,
    Moving,
    Attacking,
    Blocking,
    Stunned,
    Dead
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    LightAttack,
    HeavyAttack,
    ThrustAttack
};

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Health = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MaxHealth = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage = 20.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 150.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float BlockReduction = 0.5f;
    UPROPERTY(BlueprintReadOnly, Category = "State")
    EFightState CurrentState = EFightState::Idle;
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsBlocking = false;
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bCanAttack = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* SwordMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* HeadMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* BodyMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* LeftArmMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* RightArmMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* LeftLegMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* RightLegMesh;
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Attack(EAttackType AttackType);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartBlocking();
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopBlocking();
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeDamage(float Damage, AStickmanCharacter* Attacker);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Die();
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

private:
    void PerformAttack(EAttackType AttackType);
    void CheckForHit();
    void ResetAttackCooldown();
    void SetupStickmanMesh();
    FTimerHandle AttackCooldownTimer;
    FTimerHandle AttackDurationTimer;
    UPROPERTY()
    class AStickmanGameMode* GameMode;
    float LastAttackTime = 0.0f;
    EAttackType CurrentAttackType;
};

#include "StickmanCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "StickmanGameMode.h"

AStickmanCharacter::AStickmanCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->SetCapsuleSize(42.f, 96.0f);
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    SetupStickmanMesh();
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
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

void AStickmanCharacter::BeginPlay()
{
    Super::BeginPlay();
    GameMode = Cast<AStickmanGameMode>(UGameplayStatics::GetGameMode(this));
    Health = MaxHealth;
    CurrentState = EFightState::Idle;
}

void AStickmanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AStickmanCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AStickmanCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AStickmanCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AStickmanCharacter::LookUp);
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
        if (GetVelocity().Size() > 0.1f)
        {
            CurrentState = EFightState::Moving;
        }
        else
        {
            CurrentState = EFightState::Idle;
        }
    }
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
    CurrentAttackType = AttackType;
    CurrentState = EFightState::Attacking;
    bCanAttack = false;
    PerformAttack(AttackType);
    float AttackDuration = (AttackType == EAttackType::HeavyAttack) ? 0.8f : 0.5f;
    GetWorldTimerManager().SetTimer(AttackDurationTimer, this, &AStickmanCharacter::ResetAttackCooldown, AttackDuration, false);
}

void AStickmanCharacter::PerformAttack(EAttackType AttackType)
{
    FRotator SwordRotation = SwordMesh->GetRelativeRotation();
    switch (AttackType)
    {
    case EAttackType::LightAttack:
        SwordRotation.Pitch += 45.0f;
        break;
    case EAttackType::HeavyAttack:
        SwordRotation.Pitch += 90.0f;
        break;
    case EAttackType::ThrustAttack:
        SwordMesh->SetRelativeLocation(SwordMesh->GetRelativeLocation() + FVector(0, 20, 0));
        break;
    }
    SwordMesh->SetRelativeRotation(SwordRotation);
    GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AStickmanCharacter::CheckForHit, 0.2f, false);
}

void AStickmanCharacter::CheckForHit()
{
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation + (GetActorForwardVector() * AttackRange);
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECollisionChannel::ECC_Pawn,QueryParams);
    if (bHit)
    {
        AStickmanCharacter* HitCharacter = Cast<AStickmanCharacter>(HitResult.GetActor());
        if (HitCharacter)
        {
            float Damage = AttackDamage;
            if (CurrentAttackType == EAttackType::HeavyAttack)
                Damage *= 1.5f;
            HitCharacter->TakeDamage(Damage, this);
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
    }
    Health -= FinalDamage;
    Health = FMath::Max(0.0f, Health);
    if (Health <= 0.0f)
    {
        Die();
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
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorRotation(FRotator(0, GetActorRotation().Yaw, 90));
    if (GameMode)
    {
        GameMode->OnPlayerDeath(this);
    }
}

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 RoundsToWin = 3;
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    int32 Player1Wins = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    int32 Player2Wins = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    bool bGameInProgress = false;
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    class AStickmanCharacter* Player1;
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    class AStickmanCharacter* Player2;
    UFUNCTION(BlueprintCallable, Category = "Game Management")
    void StartNewRound();
    UFUNCTION(BlueprintCallable, Category = "Game Management")
    void OnPlayerDeath(class AStickmanCharacter* DeadPlayer);
    UFUNCTION(BlueprintCallable, Category = "Game Management")
    void ResetMatch();

private:
    void SpawnPlayers();
    void CheckForWinner();
    UPROPERTY()
    class AArenaLevel* Arena;
};

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ArenaLevel.generated.h"

UCLASS()
class STICKMANFIGHT_API AArenaLevel : public AActor
{
    GENERATED_BODY()

public:
    AArenaLevel();

protected:
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* FloorMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WallMesh1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WallMesh2;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WallMesh3;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WallMesh4;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena Settings")
    float ArenaSize = 1000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena Settings")
    float WallHeight = 200.0f;

public:
    UFUNCTION(BlueprintCallable, Category = "Arena")
    FVector GetPlayer1SpawnLocation() const;
    UFUNCTION(BlueprintCallable, Category = "Arena")
    FVector GetPlayer2SpawnLocation() const;

private:
    void SetupArenaMesh();
};