// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetworkPrCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetworkPrCharacter : public ACharacter
{
	GENERATED_BODY()
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

public:
	ANetworkPrCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	void SetCamera();
	void PrintString(const FString& String); // Faster debugging
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetCamera(AActor* NewCamera);
	void Attack();
	
	FTimerHandle TimerHandle; // Timer for PC to load
	UPROPERTY(BlueprintReadWrite)
	float AttackCapsuleRadius;
	UPROPERTY(BlueprintReadWrite)
	float AttackCapsuleHalfHeight;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerRPCFunction(int MyArg);
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPCFunction();
	UPROPERTY(EditAnywhere)
	UStaticMesh* Sphere;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraSystem;
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

