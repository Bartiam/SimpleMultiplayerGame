// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDSimpleMultiGame_Base.generated.h"



UCLASS()
class SIMPLEMULTIGAME_API AHUDSimpleMultiGame_Base : public AHUD
{
	GENERATED_BODY()
	
protected: // Protected variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UUIDuringTheGame_Base> UIDuringTheGame_Class;

protected: // Protected Functions

	virtual void BeginPlay() override;

};
