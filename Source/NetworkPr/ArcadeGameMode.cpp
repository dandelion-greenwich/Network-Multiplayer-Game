// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcadeGameMode.h"

#include "MultiplayerSubsystem.h"
#include "NetworkPrGameState.h"

void AArcadeGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	GetWorld()->GetTimerManager().SetTimer(
	TimerHandle,
	this,
	&AArcadeGameMode::TryToStartMatch,
	1.0,false);
}

void AArcadeGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	UMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMultiplayerSubsystem>();
	if (!Subsystem) return;

	FName SessionName = Subsystem->MySessionName;
	FNamedOnlineSession* ExistingSession = Subsystem -> SessionInterface->
		GetNamedSession(SessionName);
	
	if (ExistingSession)
	{
		FString Msg = FString::Printf(TEXT("Destroying session: %s"),
		   * SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Msg);
		Subsystem -> SessionInterface->DestroySession(SessionName);
		GetWorld()->ServerTravel("/Game/Scenes/MainMenu");
	}
}

void AArcadeGameMode::TryToStartMatch()
{
	ANetworkPrGameState* GS = GetGameState<ANetworkPrGameState>();
    
	if (GS && GS->Player1 && GS->Player2)
		OnStartMatch.Broadcast();
}

