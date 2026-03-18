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
    ABeatPlayer();

protected:
    virtual void BeginPlay() override;

    // ── Input handlers ────────────────────────────────────────────────────────
    virtual void Jump() override;
    void MoveLeft(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void Slide(const FInputActionValue& Value);
    void EndSlide();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ── Input assets (assign in Blueprint/editor) ─────────────────────────────
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_MoveLeft;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_MoveRight;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Jump;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Slide;

    // ── Runner tuning ─────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Runner")
    float ForwardSpeed = 800.f;

    // Distance between lane centers in world units (400 = ~4m apart)
    UPROPERTY(EditAnywhere, Category = "Runner")
    float LaneOffset = 400.f;

    // Higher = snappier lane switch lerp
    UPROPERTY(EditAnywhere, Category = "Runner")
    float LaneSwitchSpeed = 15.f;

    // ── Components ────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "Runner")
    USkeletalMeshComponent* Body;

    UPROPERTY(EditAnywhere, Category = "Runner")
    USpringArmComponent* SpringArm;

    UCameraComponent* Camera;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Runner")
    bool bIsSliding = false;
private:
    // ── Lane state ────────────────────────────────────────────────────────────
    int32 CurrentLane = 0;          // -1 left | 0 center | 1 right
    static constexpr int32 MinLane = -1;
    static constexpr int32 MaxLane = 1;

    // Captured once at spawn — permanent Y reference for lane 0 (center)
    float CenterLaneY = 0.f;

    // World-space Y position we're smoothly lerping toward
    float TargetLaneY = 0.f;

    // ── Helpers ───────────────────────────────────────────────────────────────
    void MoveForward(float DeltaTime);
    void SmoothLaneMovement(float DeltaTime);
    float GetLaneY(int32 Lane) const;  // Converts lane index → world Y

    FTimerHandle SlideTimerHandle;

};