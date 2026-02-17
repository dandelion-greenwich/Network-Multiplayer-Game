// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Camera/CameraShakeBase.h"
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

	/** ServerRPC_Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

public:
	ANetworkPrCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	void SetCamera();
	UFUNCTION()
	void ShakeCamera(AActor* Player, float NewHealth);
	void PrintString(const FString& String); // Faster debugging
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetCamera(AActor* NewCamera);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRPC_Attack();
	void RespawnPlayer();
	void ResetPush();
	
	FTimerHandle TimerHandle; // Timer for PC to load
	UPROPERTY(BlueprintReadWrite)
	float AttackSphereRadius;
	UPROPERTY(VisibleAnywhere)
	class UHealthComponent* HealthComp;
	FVector RespawnPos;
	bool CanPush;
	UPROPERTY(EditAnywhere)
	float PushResetTime;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

