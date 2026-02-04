// Fill out your copyright notice in the Description page of Project Settings.


#include "MeteorBase.h"

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
	
}

// Called every frame
void AMeteorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeteorBase::OnMeteorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Explosion();
}

void AMeteorBase::Explosion_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "OnMeteorHit");
	
	FVector StartVector = MeteorComp -> GetComponentLocation();
	float AttackSphereRadius = 100.f; // JUST FOR EXAMPLE WILL HAVE TO BE CONFIGURED IN BLUEPRINT
	FQuat SphereRotation = FQuat::Identity;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackSphereRadius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;
	
	DrawDebugSphere(GetWorld(), StartVector, AttackSphereRadius, 10.f, FColor::Orange, false, 2.0f);
	Destroy();
}

