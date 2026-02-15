// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkPrGameInstance.generated.h"

UENUM()
enum class EGameSessionMode : uint8
{
	NetworkCoop,
	LocalCoop
};

UCLASS()
class NETWORKPR_API UNetworkPrGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	EGameSessionMode CurrentGameMode;
};
