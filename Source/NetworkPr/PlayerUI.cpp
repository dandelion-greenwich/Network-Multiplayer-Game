// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"
#include "NetworkPrCharacter.h"
#include "HealthComponent.h"
#include "NetworkPrGameState.h"
#include "GameFramework/GameSession.h"

void UPlayerUI::UpdateHealth(AActor* Player, float NewHealth)
{
    // 1. Get GameState to check who is who
    ANetworkPrGameState* GS = GetWorld()->GetGameState<ANetworkPrGameState>();
    if (!GS) return;

    if (Player == GS->Player1)
    {
        FText Player1HealthText = FText::Format(
    FText::FromString("Player 1 Hearts: {0}"), FText::AsNumber(NewHealth));
        
        if (Player1Health) Player1Health->SetText(Player1HealthText);
    }
    else if (Player == GS->Player2)
    {
        FText Player2HealthText = FText::Format(
            FText::FromString("Player 2 Hearts: {0}"), FText::AsNumber(NewHealth));

        if (Player2Health) Player2Health->SetText(Player2HealthText);
    }
}

void UPlayerUI::RemoveWaitingText()
{
    WaitingForSecondPlayerText->SetText(FText::FromString(""));
}
