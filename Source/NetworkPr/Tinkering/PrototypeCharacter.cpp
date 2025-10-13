// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

// Sets default values
APrototypeCharacter::APrototypeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APrototypeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController -> GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

// Called every frame
void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APrototypeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("QUIOYEDOIqwkledjh"));	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (PlayerController)
			{
				int32 LocalId = PlayerController->GetLocalPlayer()->GetControllerId();
				if (LocalId == 0)
				{
					EnhancedInputComponent->BindAction(MoveAction_Player1, ETriggerEvent::Triggered, this, &APrototypeCharacter::Move);
				}
				else if (LocalId == 1)
				{
					EnhancedInputComponent->BindAction(MoveAction_Player2, ETriggerEvent::Triggered, this, &APrototypeCharacter::Move);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enhanced input component is an issue"));
	}
}

void APrototypeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Move"));
	UE_LOG(LogTemp, Warning, TEXT("Wordks"));
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

