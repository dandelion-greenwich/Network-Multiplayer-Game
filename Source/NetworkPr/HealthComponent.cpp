// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(GetOwner(), CurrentHealth);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
	if (!GetOwner()->HasAuthority() || CurrentHealth <= 0.0f) return;

	// Break CurrentHealth into to variables to not deal damage to the next heart
	float WholeHearts = FMath::FloorToFloat(CurrentHealth);
	float Fragment = CurrentHealth - WholeHearts;
	if (CurrentHealth > 0 && Fragment <= 0)
	{
		WholeHearts -= 1.0f;
		Fragment = 1.0f;
	}

	float NewFragment = FMath::Clamp(Fragment - DamageAmount, 0.0f, 1.0f);
	CurrentHealth = WholeHearts + NewFragment;
	OnRep_Health(); // Update Server UI manually
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(GetOwner(), CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		OnDeath.Broadcast(GetOwner());
	}
}

