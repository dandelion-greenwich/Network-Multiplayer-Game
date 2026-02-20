// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

public:
	ANetworkPrCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	void SetCamera();
	UFUNCTION(Client, Unreliable)
	void ClientRpc_ShakeCamera();
	void PrintString(const FString& String); // Faster debugging
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetCamera(AActor* NewCamera);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerRPC_Attack();
	void RespawnPlayer();
	void ResetPush();
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPushVFX();
	UFUNCTION(NetMulticast, Unreliable)
	void SetHitMaterial();
	UFUNCTION(NetMulticast, Unreliable)
	void SetDefaultMaterial();

	FTimerHandle TimerHandle; // Timer for PC to load
	UPROPERTY(BlueprintReadWrite, Category = "Class")
	float AttackSphereRadius;
	UPROPERTY(VisibleAnywhere, Category = "Class")
	class UHealthComponent* HealthComp;
	FVector RespawnPos;
	bool CanPush;
	UPROPERTY(EditAnywhere, Category = "Class")
	float PushResetTime;
	UPROPERTY(EditAnywhere, Category = "Class")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;
	UPROPERTY(EditAnywhere, Category = "Class")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = "Class")
	UNiagaraComponent* PushEffect;
	UPROPERTY(EditAnywhere, Category = "Class")
	UMaterialInterface* HitMaterial;
	UMaterialInterface* DefaultMaterial;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

