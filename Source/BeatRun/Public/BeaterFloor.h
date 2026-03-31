// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BeaterHurdleParent.h"
#include "BeaterFloor.generated.h"

UCLASS()
class BEATRUN_API ABeaterFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeaterFloor();
	UFUNCTION()
	void OnSpawnTriggerOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION()
	void OnDestroyTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Components")
	USceneComponent* Root;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Components")
	UStaticMeshComponent* Floor;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Components")
	UBoxComponent* SpawnTrigger;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Components")
	UBoxComponent* DestroyTrigger;
	// ===== Floor Spawning =====
	UPROPERTY(EditDefaultsOnly, Category = "Floor")
	TSubclassOf<ABeaterFloor> NextFloorClass;

	UPROPERTY(EditAnywhere, Category = "Floor")
	float FloorLength = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Floor")
	bool bHasSpawnedNext = false;
	// ===== Hurdle Spawning =====
	//UFUNCTION(BlueprintImplementableEvent, Category = "Floor")
	void SpawnHurdles();
	UPROPERTY(EditDefaultsOnly, Category="Hurdles")
	TArray<TSubclassOf<ABeaterHurdleParent>> HurdleClasses;
	UPROPERTY(EditAnywhere, Category="Hurdles")
	float HurdleSpawnOffsetX = 400.f;

	TArray<int32> LaneIndices = { -1, 0, 1 }; 
	UPROPERTY(EditAnywhere, Category="Hurdles")
	float LaneOffset = 500.f; 

	UPROPERTY(EditDefaultsOnly, Category="Lanes")
	float HurdleForwardOffset = 400.f;
	UPROPERTY()
	TArray<class ABeaterHurdleParent*> SpawnedHurdles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor Setup")
	float FloorWidth = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor Setup")
	float HurdleSafeMargin = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor Setup")
	int32 MinHurdlesPerFloor = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor Setup")
	int32 MaxHurdlesPerFloor = 5;
private:
	virtual void Tick(float DeltaTime) override;

};
