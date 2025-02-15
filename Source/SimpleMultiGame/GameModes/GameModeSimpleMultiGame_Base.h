// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameModeSimpleMultiGame_Base.generated.h"



UCLASS()
class SIMPLEMULTIGAME_API AGameModeSimpleMultiGame_Base : public AGameMode
{
	GENERATED_BODY()
	

public: // Public functions

	UFUNCTION()
	void RespawnCharacter(AController* Controller);

public: // Public Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ASimpleMultiGameCharacter> NewCharacter_Class;
};
