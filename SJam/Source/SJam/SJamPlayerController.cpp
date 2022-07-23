// Copyright Epic Games, Inc. All Rights Reserved.

#include "SJamPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "SJamCharacter.h"
#include "Engine/World.h"

ASJamPlayerController::ASJamPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	isPlaying = false;
	noDoubleClick = true;
}

void ASJamPlayerController::StopHammerTime()
{
	StopMovement();
	isPlaying = false;
}

void ASJamPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		// Direct the Pawn towards that location
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ASJamPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASJamPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASJamPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASJamPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ASJamPlayerController::OnTouchReleased);

}

void ASJamPlayerController::OnSetDestinationPressed()
{
	if(isPlaying)
	{
		// We flag that the input is being pressed
		bInputPressed = true;
		// Just in case the character was moving because of a previous short press we stop it
		StopMovement();
	}
}

void ASJamPlayerController::OnSetDestinationReleased()
{
	if(isPlaying)
	{
		// Player is no longer pressing the input
		bInputPressed = false;

		// If it was a short press
		if(FollowTime <= ShortPressThreshold)
		{
			// We look for the location in the world where the player has pressed the input
			FVector HitLocation = FVector::ZeroVector;
			FHitResult Hit;
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
			HitLocation = Hit.Location;

			// We move there and spawn some particles
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
		isPlaying = false;
	}
}

void ASJamPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void ASJamPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
