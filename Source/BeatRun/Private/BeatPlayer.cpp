// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// ─────────────────────────────────────────────────────────────────────────────
// Constructor — set up components and sensible movement defaults
// ─────────────────────────────────────────────────────────────────────────────
ABeatPlayer::ABeatPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    // Spring arm holds the camera behind/above the player
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 600.f;         // Camera distance from player
    SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f)); // Raise arm to shoulder height
    SpringArm->bUsePawnControlRotation = false; // We control rotation manually
    SpringArm->bEnableCameraLag = true;         // Smooth camera follow
    SpringArm->CameraLagSpeed = 8.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    // Disable Unreal's built-in rotation-follows-movement so we stay forward-facing
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    // Prevent the character capsule from rotating when we add movement input
    GetCharacterMovement()->bConstrainToPlane = false;
    GetCharacterMovement()->GravityScale = 2.f;       // Snappier jumps
    GetCharacterMovement()->JumpZVelocity = 900.f;
    GetCharacterMovement()->AirControl = 0.f;         // No steering mid-air (runner feel)
}

// ─────────────────────────────────────────────────────────────────────────────
// BeginPlay — bind Enhanced Input mapping context
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Store the spawn Y as the permanent center lane reference.
    // GetLaneY() uses this to calculate left/right lane positions.
    CenterLaneY = GetActorLocation().Y;
    TargetLaneY = CenterLaneY; // Start targeting center

    // Add the input mapping context so our IA_ actions fire
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Tick — drive forward movement and lane lerp every frame
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveForward(DeltaTime);
    SmoothLaneMovement(DeltaTime);
}

// ─────────────────────────────────────────────────────────────────────────────
// SetupPlayerInputComponent — wire Enhanced Input actions to our handlers
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to Enhanced Input component — required for UE5 Input Actions
    UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

    // ETriggerEvent::Started fires once on key/button press (not every frame)
    EIC->BindAction(IA_MoveLeft,  ETriggerEvent::Started,   this, &ABeatPlayer::MoveLeft);
    EIC->BindAction(IA_MoveRight, ETriggerEvent::Started,   this, &ABeatPlayer::MoveRight);
    EIC->BindAction(IA_Jump,      ETriggerEvent::Started,   this, &ABeatPlayer::Jump);
    EIC->BindAction(IA_Slide,     ETriggerEvent::Started,   this, &ABeatPlayer::Slide);
}

void ABeatPlayer::AddScore(float Addition)
{
    PlayerScore=PlayerScore+Addition;
    if (PlayerScore<=0)
        GEngine->AddOnScreenDebugMessage(1,5.0f,FColor::Yellow,TEXT("Playeris Dead"));
}

// ─────────────────────────────────────────────────────────────────────────────
// MoveForward — pushes the character along +X every frame
// We do this manually rather than using AddMovementInput so speed is always
// exactly ForwardSpeed regardless of analog stick pressure.
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::MoveForward(float DeltaTime)
{
    // AddMovementInput feeds into CharacterMovementComponent, which means:
    //   1. Velocity is updated — animation blueprints can read it via Speed
    //   2. Gravity and ground detection still work correctly
    //   3. The movement component handles acceleration curves internally
    // GetActorForwardVector() ensures we move in the direction the character faces,
    // not hardcoded world +X.
    AddMovementInput(GetActorForwardVector(), 1.0f);

    // Override the max walk speed so ForwardSpeed is always respected.
    // We set this every tick so it responds to speed upgrades later (score system).
    GetCharacterMovement()->MaxWalkSpeed = ForwardSpeed;
}

// ─────────────────────────────────────────────────────────────────────────────
// SmoothLaneMovement — each frame lerps the character's Y toward TargetLaneY
//
// Why lerp Y directly instead of using AddMovementInput?
//   AddMovementInput goes through the CharacterMovement component which adds
//   friction/acceleration. For a runner we want crisp, predictable lane snapping.
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::SmoothLaneMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();

    // FInterpTo smoothly closes the gap each frame — never overshoots.
    // We interpolate world Y (lane axis). This works correctly as long as
    // the track runs along the actor's forward (X) axis and lanes are on Y.
    const float NewY = FMath::FInterpTo(
        CurrentLocation.Y,
        TargetLaneY,
        DeltaTime,
        LaneSwitchSpeed
    );

    // Apply only the Y change — X is driven by MoveForward, Z by gravity
    SetActorLocation(FVector(CurrentLocation.X, NewY, CurrentLocation.Z), true);
}

// ─────────────────────────────────────────────────────────────────────────────
// GetLaneY — converts a lane index (-1, 0, 1) to a world-space Y coordinate.
// We anchor to the spawn Y so the lanes are always relative to where the
// character started, not hard-coded world coordinates.
// ─────────────────────────────────────────────────────────────────────────────
float ABeatPlayer::GetLaneY(int32 Lane) const
{
    // CenterLaneY is set ONCE in BeginPlay to the spawn Y position.
    // It never changes — it's our fixed reference point for all 3 lanes.
    // Lane -1 = left,  Lane 0 = center,  Lane +1 = right
    return CenterLaneY + (Lane * LaneOffset);
}

// ─────────────────────────────────────────────────────────────────────────────
// Input — Lane switching
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::MoveLeft(const FInputActionValue& Value)
{
    if (CurrentLane <= MinLane) return; // Already in leftmost lane

    CurrentLane--;
    TargetLaneY = GetLaneY(CurrentLane);
}

void ABeatPlayer::MoveRight(const FInputActionValue& Value)
{
    if (CurrentLane >= MaxLane) return; // Already in rightmost lane

    CurrentLane++;
    TargetLaneY = GetLaneY(CurrentLane);
}

// ─────────────────────────────────────────────────────────────────────────────
// Input — Jump & Slide
// ─────────────────────────────────────────────────────────────────────────────
void ABeatPlayer::Jump()
{
    // Calls ACharacter::Jump() which sets bPressedJump and triggers the jump arc
    Super::Jump();
}

void ABeatPlayer::Slide(const FInputActionValue& Value)
{
    // Guard — if already sliding, ignore the input entirely.
    // Without this, holding the key re-triggers Slide every frame because
    // ETriggerEvent::Started fires continuously on held inputs in UE5.
    if (bIsSliding) return;

    bIsSliding = true;

    // Shrink the capsule so the character physically fits under obstacles.
    // Half-height 40 = crouched, default is usually 88-96 depending on your mesh.
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    Capsule->SetCapsuleHalfHeight(40.f);

    // Shift the mesh up to compensate — otherwise the feet sink into the floor.
    // The offset should be (DefaultHalfHeight - SlideHalfHeight) = ~48-56 units.
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -40.f));

    // Start the 1-second timer to end the slide
    GetWorldTimerManager().SetTimer(
        SlideTimerHandle,
        this,
        &ABeatPlayer::EndSlide,
        1.0f,
        false
    );

    UE_LOG(LogTemp, Log, TEXT("Slide started"));
}

void ABeatPlayer::EndSlide()
{
    bIsSliding = false;

    // Restore capsule and mesh to standing size
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    Capsule->SetCapsuleHalfHeight(88.f); // match your default capsule half-height

    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f)); // restore mesh offset

    UE_LOG(LogTemp, Log, TEXT("Slide ended"));
}

