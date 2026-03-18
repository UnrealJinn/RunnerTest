// Fill out your copyright notice in the Description page of Project Settings.


#include "BeaterFloor.h"
#include "BeaterHurdleParent.h"

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
	SpawnHurdles();
}
// Called every frame
void ABeaterFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ABeaterFloor::OnSpawnTriggerOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnTrigger overlapped by: %s"),*GetNameSafe(OtherActor));
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
	FVector SpawnLocation = FVector(
		GetActorLocation().X + FloorLength,
		0.f,
		GetActorLocation().Z
	);
	ABeaterFloor* NewFloor = GetWorld()->SpawnActor<ABeaterFloor>(NextFloorClass,SpawnLocation,GetActorRotation());
	if (NewFloor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned next floor at: %s"),*SpawnLocation.ToString());
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
	for (ABeaterHurdleParent* Hurdle : SpawnedHurdles)
	{
		if (IsValid(Hurdle))
		{
			Hurdle->Destroy();
		}
	}
	Destroy();
}
void ABeaterFloor::SpawnHurdles()
{
	if (HurdleClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No HurdleClasses assigned"));
		return;
	}
	UWorld* World = GetWorld();
	if (!World) return;
	const FVector FloorLocation = GetActorLocation();
	const FVector RightVector = GetActorRightVector();
	const FVector ForwardVector = GetActorForwardVector();
	float SafeMargin = 100.0f;    // Margin from edges
	for (int32 i = 0; i < LaneIndices.Num(); i++)
	{
		int32 Lane = LaneIndices[i];
		// Randomize the forward offset for each hurdle
		float RandomForwardOffset = FMath::RandRange(SafeMargin, FloorLength - SafeMargin);
		// Calculate spawn position using the floor's orientation
		FVector SpawnLocation = FloorLocation + (ForwardVector * RandomForwardOffset) + (RightVector * (Lane * LaneOffset));
		// Pick random hurdle class
		TSubclassOf<ABeaterHurdleParent> HurdleClass = HurdleClasses[FMath::RandRange(0, HurdleClasses.Num()-1)];
		if (HurdleClass)
		{
			ABeaterHurdleParent* Hurdle = GetWorld()->SpawnActor<ABeaterHurdleParent>(
			   HurdleClass,
			   SpawnLocation, 
			   GetActorRotation() // Or you might want FRotator::ZeroRotator if hurdles should face world forward
			);
			if (Hurdle)
			{
				Hurdle->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				SpawnedHurdles.Add(Hurdle);
			}
		}
	}
}