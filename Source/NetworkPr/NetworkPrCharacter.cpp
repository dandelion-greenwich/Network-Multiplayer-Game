// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkPrCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetworkPrCharacter

ANetworkPrCharacter::ANetworkPrCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANetworkPrCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ANetworkPrCharacter::PrintString(const FString& String) 
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, String);
}

void ANetworkPrCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ANetworkPrCharacter::SetCamera,
		0.5,false);
}


void ANetworkPrCharacter::SetCamera()
{
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MainCamera"), FoundCameras);

	if (FoundCameras.Num() > 0)
	{
		AActor* NewCamera = FoundCameras[0];
		PrintString("Camera found");
		ClientRPC_SetCamera(NewCamera);
	}
	else
	{
		PrintString("No camera found");
	}
}

void ANetworkPrCharacter::ClientRPC_SetCamera_Implementation(AActor* NewCamera)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		PrintString("Player controller is null");
		return;
	}

	PC->SetViewTarget(NewCamera);
}

void ANetworkPrCharacter::Attack()
{
	// Parameters for SweepMultiByChannel
	FVector StartVector = GetActorLocation() + (GetActorForwardVector() * 60.f );
	FVector EndVector = StartVector + (GetActorForwardVector() * 500.f);
	AttackCapsuleRadius = 60.f; // JUST FOR EXAMPLE WILL HAVE TO BE CONFIGURED IN BLUEPRINT
	AttackCapsuleHalfHeight = 90.f;
	FQuat CapsuleRotation = FQuat::Identity;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AttackCapsuleRadius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;
	
	DrawDebugSphere(GetWorld(), StartVector, AttackCapsuleRadius, 10.f, FColor::Red, false, 2.0f);
	// Checks if any objects are within the radius of a multicast
	bool HasHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartVector,
		StartVector,
		CapsuleRotation,
		ECC_Pawn,
		SphereShape,
		QueryParams);
	
	if (!HasHit) 
		return;
	
	// If has hit successful - apply damage and then destroy blueprint
	for (auto HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetActorNameOrLabel());
		/*if (HitActor->GetClass()->ImplementsInterface(UIDamagable::StaticClass()))
		{
			FDamageInfo DamageInfo;
			DamageInfo.DamageAmount = 50.f;
			DamageInfo.DamageType = EDamageType::Explosion;
			IIDamagable::Execute_TakeDamage(HitActor, DamageInfo);
		}*/
	}
}

void ANetworkPrCharacter::ServerRPCFunction_Implementation(int MyArg)
{
	if (!HasAuthority() || !Sphere)
		return;
	
#if 0
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
			TEXT("Server: ServerRPCFunction_Implementation"));
#endif

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	AStaticMeshActor* StaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnParameters);
	if (StaticMeshActor)
	{
		StaticMeshActor->SetReplicates(true);
		StaticMeshActor->SetReplicateMovement(true);
		StaticMeshActor->SetMobility(EComponentMobility::Movable);
		FVector SpawnLocation = GetActorLocation() + GetActorRotation().Vector() * 200.0f + GetActorUpVector() * 150.0f;
		StaticMeshActor->SetActorLocation(SpawnLocation);

		UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetIsReplicated(true);
			StaticMeshComponent->SetSimulatePhysics(true);
			StaticMeshComponent->SetStaticMesh(Sphere);
		}
	}
}

void ANetworkPrCharacter::ClientRPCFunction_Implementation()
{
	if (NiagaraSystem)
	{
		FVector NewLocation = GetActorLocation();
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, NewLocation,
			FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease);
	}

}

bool ANetworkPrCharacter::ServerRPCFunction_Validate(int MyArg)
{
	if (MyArg >= 0 && MyArg <= 100)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetworkPrCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ANetworkPrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkPrCharacter::Move);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ANetworkPrCharacter::Attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetworkPrCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X); 
	}
}

void ANetworkPrCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
