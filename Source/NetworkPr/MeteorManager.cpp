// Fill out your copyright notice in the Description page of Project Settings.


#include "MeteorManager.h"
#include "MeteorBase.h"

// Sets default values
AMeteorManager::AMeteorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AMeteorManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,                
		this,                       
		&AMeteorManager::SpawnMeteor,
		TimeToSpawn,                        
		true,                       
		-1.0);
}

void AMeteorManager::SpawnMeteor_Implementation()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	int32 RandX = FMath::RandRange(-RandRange, RandRange);
	int32 RandY = FMath::RandRange(-RandRange, RandRange);
	FVector CurrentLocation = GetActorLocation();
	FVector SpawnLocation(CurrentLocation.X + RandX, CurrentLocation.Y + RandY, CurrentLocation.Z);
	
	AMeteorBase* SpawnedActor = GetWorld() -> SpawnActor<AMeteorBase>(MeteorBP, SpawnLocation, GetActorRotation(), SpawnParameters);
}

