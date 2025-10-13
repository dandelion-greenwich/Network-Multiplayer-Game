// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PrototypeCharacter.generated.h"

UCLASS()
class NETWORKPR_API APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APrototypeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction_Player1;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MoveAction_Player2;

	void Move(const FInputActionValue& Value);

};
