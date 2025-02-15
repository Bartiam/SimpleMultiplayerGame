// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDSimpleMultiGame_Base.h"
#include "../UserInterfaces/UIDuringTheGame_Base.h"

void AHUDSimpleMultiGame_Base::BeginPlay()
{
	Super::BeginPlay();

	auto NewWidget = CreateWidget<UUIDuringTheGame_Base>(GetWorld(), UIDuringTheGame_Class);
	NewWidget->AddToViewport();

}