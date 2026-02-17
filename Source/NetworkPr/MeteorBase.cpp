// Fill out your copyright notice in the Description page of Project Settings.


#include "MeteorBase.h"

#include "HealthComponent.h"
#include "NetworkPrCharacter.h"
#include "Engine/OverlapResult.h"


// Sets default values
AMeteorBase::AMeteorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = RootComp;
	
	MeteorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeteorComp -> SetupAttachment(RootComp);
	
	MeteorComp->SetSimulatePhysics(true);
	MeteorComp->SetNotifyRigidBodyCollision(true); 
	MeteorComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeteorComp->OnComponentHit.AddDynamic(this, &AMeteorBase::OnMeteorHit);
	
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AMeteorBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<FHitResult> HitResults;
	FVector Start = GetActorLocation();

	FVector DownVector = -GetActorUpVector();
	FVector End =  Start + (DownVector * 1000.f);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3, 0, 1);

	bool hasHit = GetWorld()->LineTraceMultiByChannel(
		HitResults,
		Start,
		End,
		ECC_Visibility,
		CollisionParams);

	if (!hasHit) return;
	
	for (auto HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor) continue;
		
		if (HitActor && HitActor -> ActorHasTag("Floor"))
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			FVector SpawnedLocation = HitResult.Location;
			PreviewActorToDestroy = GetWorld()->SpawnActor<AActor>(MeteorClass, SpawnedLocation, FRotator::ZeroRotator, SpawnParameters);
			break;
		}
	}
}

void AMeteorBase::OnMeteorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ServerRPC_Explosion_Implementation();
}

void AMeteorBase::ServerRPC_Explosion_Implementation()
{
	FVector StartVector = MeteorComp -> GetComponentLocation();
	FQuat SphereRotation = FQuat::Identity;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackSphereRadius);
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		StartVector,
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);

	if (bHasOverlap)
	{
		for (auto OverlapResult : OverlapResults)
		{
			AActor* HitActor = OverlapResult.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("Explosion Hit actor: %s"), *HitActor->GetActorNameOrLabel());
			if (!HitActor || !HitActor -> ActorHasTag("Player")) continue; // Go to the next iteration of for loop if HitActor is a wall for instance

			ANetworkPrCharacter* Character = Cast<ANetworkPrCharacter>(HitActor);
			if (!Character || !Character -> HealthComp) continue;
			float DistanceDifference = FVector::Dist(StartVector, HitActor->GetActorLocation());

			if (DistanceDifference > AttackSphereRadius / 2)
				Character -> HealthComp -> TakeDamage(0.25f, EDamageType::Explosion);
			else
				Character -> HealthComp -> TakeDamage(0.5f, EDamageType::Explosion);
		}
	}
	
	DrawDebugSphere(GetWorld(), StartVector, AttackSphereRadius, 10.f, FColor::Orange, false, 2.0f);
	if (PreviewActorToDestroy != nullptr) PreviewActorToDestroy -> Destroy();
	Destroy();
}

