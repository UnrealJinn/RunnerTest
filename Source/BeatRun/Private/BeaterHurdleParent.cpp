// Fill out your copyright notice in the Description page of Project Settings.


#include "BeaterHurdleParent.h"

#include "BeatPlayer.h"

// Sets default values
ABeaterHurdleParent::ABeaterHurdleParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root=CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	HurdleMesh=CreateDefaultSubobject<UStaticMeshComponent>("HurdleMesh");
	HurdleMesh->SetupAttachment(Root);
	HurdleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox=CreateDefaultSubobject<UBoxComponent>("HitBox");
	HitBox->SetupAttachment(Root);
	HitBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}
// Called when the game starts or when spawned

void ABeaterHurdleParent::BeginPlay()
{
	Super::BeginPlay();
//	FVector Loc=GetActorLocation();
//	Loc.Y=LaneIndex*LaneOffset;
//	SetActorLocation(Loc);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ABeaterHurdleParent::OnHurdleOverlap);
}

void ABeaterHurdleParent::OnHurdleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABeatPlayer* Player=Cast<ABeatPlayer>(OtherActor);
	if (!Player) return;
	UE_LOG(LogTemp,Warning, TEXT("Player hit Hurdle: %s"),*GetName());	
	Player->AddScore(ScoreUpdate);
}

// Called every frame
void ABeaterHurdleParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

