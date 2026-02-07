// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"
#include "NetworkPrCharacter.h"
#include "HealthComponent.h"

void UPlayerUI::NativeConstruct()
{
    Super::NativeConstruct();

    ANetworkPrCharacter* MyChar = Cast<ANetworkPrCharacter>(GetOwningPlayerPawn());
    if (MyChar && MyChar->HealthComp)
    {
        MyChar->HealthComp->OnHealthChanged.AddDynamic(this, &UPlayerUI::UpdateHealth);
        UpdateHealth(MyChar->HealthComp->CurrentHealth);
    }

}

void UPlayerUI::UpdateHealth(float CurrentHealth)
{
    if (CurrentHealthText)
    {
        CurrentHealthText->SetText(FText::AsNumber(CurrentHealth));
    }
}