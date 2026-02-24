// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "BeatPlayer.generated.h"

class UInputMappingContext;
class UInputAction;
UCLASS()
class BEATRUN_API ABeatPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABeatPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Jump() override;
	void MoveLeft(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
public:
	void MoveForward(float DeltaTime);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_MoveLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_MoveRight;
	
	UFUNCTION()
	void Slide(const FInputActionValue& Value);
	void EndSlide();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Slide;
	// ===== Runner Settings =====
	UPROPERTY(EditAnywhere, Category = "Runner")
	float ForwardSpeed = 800.f;

	UPROPERTY(EditAnywhere, Category = "Runner")
	float LaneOffset = 400.f;
	
	UPROPERTY(editAnywhere, Category = "Runner")
	USkeletalMeshComponent* Body;
	UPROPERTY(EditAnywhere, Category = "Runner")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, Category = "Runner")
	UCameraComponent* Camera;
private:
	int32 CurrentLane = 0; // -1 = left, 0 = center, 1 = right
	const int32 MinLane = -1;
	const int32 MaxLane = 1;
	FVector TargetLocation;
	FTimerHandle SlideTimerHandle;
};
