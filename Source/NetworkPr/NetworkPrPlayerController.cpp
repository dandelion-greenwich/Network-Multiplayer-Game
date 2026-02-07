// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPrPlayerController.h"

void ANetworkPrPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && PlayerWidgetClass)
	{
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(this, PlayerWidgetClass);

		PlayerWidgetReference = Cast<UPlayerUI>(CreatedWidget);
		if (PlayerWidgetReference)
		{
			PlayerWidgetReference->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("UI Created for %s"), *GetName());
		}
	}
}
