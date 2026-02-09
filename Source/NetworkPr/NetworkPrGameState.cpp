// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkPrGameState.h"
#include "Net/UnrealNetwork.h"

void ANetworkPrGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANetworkPrGameState, Player1);
	DOREPLIFETIME(ANetworkPrGameState, Player2);
}

void ANetworkPrGameState::RegisterPlayer(ANetworkPrCharacter* NewPlayer)
{
	if (!Player1)
	{
		Player1 = NewPlayer;
	}
	else if (!Player2 && NewPlayer != Player1)
	{
		Player2 = NewPlayer;
	}
}