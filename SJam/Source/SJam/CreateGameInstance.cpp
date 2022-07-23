// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UCreateGameInstance::UCreateGameInstance()
{
}

void UCreateGameInstance::Init()
{
	Super::Init();

	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCreateGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCreateGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCreateGameInstance::OnJoinSessionComplete);
		}
	}
}

void UCreateGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	if(Succeeded)
	{
		//if server is correctly create we load this map for all player connected.
		GetWorld()->ServerTravel("/Game/Maps/TopDownMap?listen");
	}
}

void UCreateGameInstance::OnFindSessionComplete(bool Succeeded)
{
	if (Succeeded)
	{
		if(SessionSearch != nullptr)
		{
			TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

			if (SearchResults.Num())
			{
				//here we connect to the first correct instance SearchResults[0]
				SessionInterface->JoinSession(0, FName("Create Session"), SearchResults[0]);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Search is still null"));
		}
	}
}

void UCreateGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if(JoinAddress != "")
		{
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UCreateGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;

	SessionInterface->CreateSession(0, FName("Create Session"), SessionSettings);
}

void UCreateGameInstance::JoinServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
