// Fill out your copyright notice in the Description page of Project Settings.


#include "BeaterFloor.h"

// Sets default values
ABeaterFloor::ABeaterFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root=CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(Root);
	
	Floor=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(Root);
	
	SpawnTrigger=CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnTrigger"));
	SpawnTrigger->SetupAttachment(Root);
	SpawnTrigger->SetBoxExtent(FVector(200.f,500.f,200.f));
	SpawnTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	
	DestroyTrigger=CreateDefaultSubobject<UBoxComponent>(TEXT("DestroyTrigger"));
	DestroyTrigger->SetupAttachment(Root);
	DestroyTrigger->SetBoxExtent(FVector(200.f,500.f,200.f));
	DestroyTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

}

// Called when the game starts or when spawned
void ABeaterFloor::BeginPlay()
{
	Super::BeginPlay();
	SpawnTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABeaterFloor::OnSpawnTriggerOverlap);
	DestroyTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABeaterFloor::OnDestroyTriggerOverlap);
}

// Called every frame
void ABeaterFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABeaterFloor::OnSpawnTriggerOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnTrigger overlapped by: %s"), 
		   *GetNameSafe(OtherActor));

	if (bHasSpawnedNext)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already spawned next floor."));
		return;
	}

	if (!OtherActor || !OtherActor->IsA(APawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap ignored: not a pawn."));
		return;
	}

	if (!NextFloorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("NextFloorClass is not assigned!"));
		return;
	}

	bHasSpawnedNext = true;
	// Calculate floor length along X
	FVector FloorSize = Floor->Bounds.BoxExtent * 2.f;
	float FloorLengthX = FloorSize.X;

	// Spawn aligned along Y = 0
	FVector SpawnLocation = FVector(GetActorLocation().X + FloorLengthX, 0.f, GetActorLocation().Z);

	ABeaterFloor* NewFloor = GetWorld()->SpawnActor<ABeaterFloor>(NextFloorClass,SpawnLocation,GetActorRotation());

	if (NewFloor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned next floor at: %s"), 
			   *SpawnLocation.ToString());
		NewFloor->bHasSpawnedNext = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn next floor!"));
	}
}

void ABeaterFloor::OnDestroyTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//SetLifeSpan(10.0f);
	Destroy();
}
