// Fill out your copyright notice in the Description page of Project Settings.
#include "BeatPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABeatPlayer::ABeatPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = ForwardSpeed;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	
	Body=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body=GetMesh();
	//Body->SetupAttachment()
	SpringArm=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Body);
	SpringArm->TargetArmLength=600.f;
	SpringArm->bUsePawnControlRotation=false;
	SpringArm->bEnableCameraLag=true;
	SpringArm->CameraLagSpeed=10.f;
	
	Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation=false;
	
	bUseControllerRotationYaw=false;
	GetCharacterMovement()->bOrientRotationToMovement=false;
}

// Called when the game starts or when spawned
void ABeatPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	TargetLocation = GetActorLocation();
}
// Called every frame
void ABeatPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Constant forward motion
	AddMovementInput(GetActorForwardVector(), 1.f);
	// Smooth lane switching
	FVector CurrentLocation = GetActorLocation();
	FVector TargetPos = FVector(CurrentLocation.X, CurrentLane * LaneOffset, CurrentLocation.Z);

	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetPos, DeltaTime, 15.f);
	SetActorLocation(NewLocation);
}
// Called to bind functionality to input
void ABeatPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput =Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(IA_MoveLeft,ETriggerEvent::Triggered,this, &ABeatPlayer::MoveLeft);
		EnhancedInput->BindAction(IA_MoveRight,ETriggerEvent::Triggered,this, &ABeatPlayer::MoveRight);
		EnhancedInput->BindAction(IA_Jump,ETriggerEvent::Triggered,this, &ABeatPlayer::Jump);
		EnhancedInput->BindAction(IA_Slide,ETriggerEvent::Triggered,this, &ABeatPlayer::Slide);
	}

}

void ABeatPlayer::MoveForward(float DeltaTime)
{
	AddMovementInput(FVector::ForwardVector, 1.0f);
}
void ABeatPlayer::MoveLeft(const FInputActionValue& Value)
{
	if (CurrentLane > MinLane)
	{
		CurrentLane--;
		UE_LOG(LogTemp, Warning, TEXT("Moved Left. CurrentLane = %d"), CurrentLane);
	}
}

void ABeatPlayer::MoveRight(const FInputActionValue& Value)
{
	if (CurrentLane < MaxLane)
	{
		CurrentLane++;
		UE_LOG(LogTemp, Warning, TEXT("Moved Right. CurrentLane = %d"), CurrentLane);
	}
}
void ABeatPlayer::Jump()
{
	Super::Jump();
}

void ABeatPlayer::Slide(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsCrouching())
	{
		Crouch();
		// Slide duration can be adjusted
		GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this, &ABeatPlayer::EndSlide, 0.7f, false);
	}
}

void ABeatPlayer::EndSlide()
{
	UnCrouch();
}