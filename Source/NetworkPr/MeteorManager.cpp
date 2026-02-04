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

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Has Authority"));
	}
	else if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Doesn't have Authority"));
	}

	if (!HasAuthority()) return;
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,                
		this,                       
		&AMeteorManager::SpawnMeteor,
		5.0,                        
		true,                       
		-1.0);
}

// Called every frame
void AMeteorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeteorManager::SpawnMeteor_Implementation()
{
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this; 
	AMeteorBase* SpawnedActor = GetWorld() -> SpawnActor<AMeteorBase>(MeteorBP, GetActorLocation(), GetActorRotation(), SpawnParameters);
	
	/*GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,                
	this,                       
	&AMeteorManager::SpawnMeteor,
	5.0,                        
	true,                       
	1.0);*/
}

