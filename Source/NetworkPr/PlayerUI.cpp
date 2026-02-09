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
        if (Player1Health) Player1Health->SetText(FText::AsNumber(NewHealth));
    }
    else if (Player == GS->Player2)
    {
        if (Player2Health) Player2Health->SetText(FText::AsNumber(NewHealth));
    }
}
