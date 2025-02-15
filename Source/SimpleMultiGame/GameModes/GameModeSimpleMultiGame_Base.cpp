// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeSimpleMultiGame_Base.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "../SimpleMultiGameCharacter.h"

void AGameModeSimpleMultiGame_Base::RespawnCharacter(AController* Controller)
{
	if (IsValid(Controller->GetPawn()))
	{
		Controller->GetPawn()->Destroy();
	}

	TArray<AActor*> PlayerStarts;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	int RandomPositionToStartPlayer = FMath::RandRange(0, (PlayerStarts.Num() - 1));
	FTransform NewTransform = PlayerStarts[RandomPositionToStartPlayer]->GetTransform();

	auto NewCharacter = GetWorld()->SpawnActor<ASimpleMultiGameCharacter>(NewCharacter_Class, NewTransform);

	Controller->Possess(NewCharacter);
}
