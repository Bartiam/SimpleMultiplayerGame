// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleMultiGameGameMode.h"
#include "SimpleMultiGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASimpleMultiGameGameMode::ASimpleMultiGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
