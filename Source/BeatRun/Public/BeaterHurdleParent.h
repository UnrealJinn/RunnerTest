// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BeaterHurdleParent.generated.h"

UCLASS()
class BEATRUN_API ABeaterHurdleParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeaterHurdleParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnHurdleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* HurdleMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UBoxComponent* HitBox;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 LaneIndex=0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float LaneOffset=400.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ScoreUpdate=1.0f;
};
