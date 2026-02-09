// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetworkPrCharacter.h"
#include "NetworkPrGameState.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPR_API ANetworkPrGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Replicated)
	ANetworkPrCharacter* Player1;
	UPROPERTY(Replicated)
	ANetworkPrCharacter* Player2;

	// Helper to register players (server only)
	void RegisterPlayer(ANetworkPrCharacter* NewPlayer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
