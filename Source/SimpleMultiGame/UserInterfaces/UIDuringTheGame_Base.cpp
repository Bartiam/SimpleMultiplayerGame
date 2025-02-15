// Fill out your copyright notice in the Description page of Project Settings.


#include "UIDuringTheGame_Base.h"
#include "../SimpleMultiGameCharacter.h"

void UUIDuringTheGame_Base::NativeConstruct()
{
	CurrentCharacter = Cast<ASimpleMultiGameCharacter>(GetOwningPlayerPawn());

	
}

float UUIDuringTheGame_Base::GetCurrentHealth()
{
	return CurrentCharacter->CurrentHealth;
}

int32 UUIDuringTheGame_Base::GetCurrentAMMO()
{
	return CurrentCharacter->AMMOCount;
}
