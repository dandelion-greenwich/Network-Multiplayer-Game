// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkPrGameState.h"
#include "ArcadeGameMode.h"
#include "NetworkPrPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ANetworkPrGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANetworkPrGameState, Player1);
	DOREPLIFETIME(ANetworkPrGameState, Player2);
	DOREPLIFETIME(ANetworkPrGameState, CurrentGameState);
}

void ANetworkPrGameState::RegisterPlayer(ANetworkPrCharacter* NewPlayer)
{
	if (!HasAuthority()) return;
	
	if (!Player1)
	{
		Player1 = NewPlayer;
		AArcadeGameMode* GM = GetWorld()->GetAuthGameMode<AArcadeGameMode>();
		if (GM) GM -> WaitForTheSecondPlayer();
	}
	else if (!Player2 && NewPlayer != Player1)
	{
		Player2 = NewPlayer;
		AArcadeGameMode* GM = GetWorld()->GetAuthGameMode<AArcadeGameMode>();
		if (GM) GM -> ContinueGame();

		GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,                
		this,                       
		&ANetworkPrGameState::TimerToLoadPCToRemoveWaitingUI,
		0.5,                        
		false,                       
		-1.0);
	}
}

void ANetworkPrGameState::TimerToLoadPCToRemoveWaitingUI()
{
	// Remove waiting text for Player 1
	ANetworkPrPlayerController* PC1 = Cast<ANetworkPrPlayerController>(Player1->GetController());
	if (PC1)
		PC1->ClientRPC_SetWaitingText();
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "PC of Player 1 is null in GameState");

	// Remove waiting text for Player 2
	ANetworkPrPlayerController* PC2 = Cast<ANetworkPrPlayerController>(Player2->GetController());
	if (PC2)
		PC2->ClientRPC_SetWaitingText();
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "PC of Player 2 is null in GameState");

	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ANetworkPrGameState::Multicast_Wait_Implementation()
{
	CurrentGameState = EGameState::Waiting;
	SetFreezeTime(true);
}

void ANetworkPrGameState::Multicast_Play_Implementation()
{
	CurrentGameState = EGameState::Playing;
	SetFreezeTime(false);
}

void ANetworkPrGameState::Multicast_GameOver_Implementation()
{
	CurrentGameState = EGameState::GameOver;
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,                
	this,                       
	&ANetworkPrGameState::GameOverTimer,
	0.2,                        
	false,                       
	-1.0);
}

void ANetworkPrGameState::GameOverTimer()
{
	SetFreezeTime(true);
	// TODO: Data saving related functionality
}

void ANetworkPrGameState::SetFreezeTime(bool bFreeze)
{
	if (bFreeze)
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
	else
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}
