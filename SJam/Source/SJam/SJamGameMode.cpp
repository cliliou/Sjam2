// Copyright Epic Games, Inc. All Rights Reserved.

#include "SJamGameMode.h"

#include "EngineUtils.h"
#include "SJamPlayerController.h"
#include "SJamCharacter.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "UObject/ConstructorHelpers.h"

ASJamGameMode::ASJamGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASJamPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("I'm the host mother fucker"));

	/*
	if(GetWorld() != nullptr)
	{
		int32 toto = GetWorld()->GetNumPlayerControllers();
		UE_LOG(LogTemp, Warning, TEXT("number of player = %n"), toto);
		for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			//ASJamPlayerController* Controller = Cast<ASJamPlayerController>(Iterator->Get());
			UE_LOG(LogTemp, Warning, TEXT("FILS DE PUTE"));
		}
	}
	*/

	
}


