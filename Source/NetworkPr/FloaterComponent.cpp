// Fill out your copyright notice in the Description page of Project Settings.


#include "FloaterComponent.h"

// Sets default values for this component's properties
UFloaterComponent::UFloaterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFloaterComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		StartLocation = Owner->GetActorLocation();
	}

	PhaseOffset = FMath::RandRange(0.0f, PI * 2.0f);
}


// Called every frame
void UFloaterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Get the current time
		float RunningTime = GetWorld()->GetTimeSeconds();
		// Calculate the new vertical offset using a Sine wave
		float DeltaHeight = (FMath::Sin((RunningTime * FloatSpeed) + PhaseOffset) * FloatHeight);

		FVector NewLocation = StartLocation;
		NewLocation.Z += DeltaHeight;
		Owner->SetActorLocation(NewLocation);
	}
}

