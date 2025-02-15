// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIDuringTheGame_Base.generated.h"



UCLASS()
class SIMPLEMULTIGAME_API UUIDuringTheGame_Base : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

	class ASimpleMultiGameCharacter* CurrentCharacter = nullptr;

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHealth();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentAMMO();
};
