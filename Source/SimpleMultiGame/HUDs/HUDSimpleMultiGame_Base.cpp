// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDSimpleMultiGame_Base.h"
#include "../UserInterfaces/UIDuringTheGame_Base.h"

void AHUDSimpleMultiGame_Base::DrawUIDuringTheGame()
{
	UIDuringTheGame = CreateWidget<UUIDuringTheGame_Base>(PlayerOwner, UIDuringTheGame_Class);
	UIDuringTheGame->AddToViewport();
}

void AHUDSimpleMultiGame_Base::DeleteUIDuringTheGame()
{
	UIDuringTheGame->RemoveFromParent();
	UIDuringTheGame = nullptr;
}

void AHUDSimpleMultiGame_Base::BeginPlay()
{
	Super::BeginPlay();
}