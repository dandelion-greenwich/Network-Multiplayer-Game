// Fill out your copyright notice in the Description page of Project Settings.


#include "MeteorBase.h"
#include "Engine/StaticMeshActor.h"


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
	Explosion_Implementation();
}

void AMeteorBase::Explosion_Implementation()
{
	FVector StartVector = MeteorComp -> GetComponentLocation();
	FQuat SphereRotation = FQuat::Identity;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackSphereRadius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;
	
	DrawDebugSphere(GetWorld(), StartVector, AttackSphereRadius, 10.f, FColor::Orange, false, 2.0f);
	if (PreviewActorToDestroy != nullptr) PreviewActorToDestroy -> Destroy();
	Destroy();
}

