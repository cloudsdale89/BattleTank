// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "Engine/World.h"
#include "TankPawn.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	if (!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("TankPlayerController not possessing ATankPawn"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TankPlayerController possessing:%s"), *ControlledTank->GetName());
	}

}

// Called every frame
void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();

}

ATankPawn* ATankPlayerController::GetControlledTank() const
{
	return Cast<ATankPawn>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank()) { return; }

	FVector HitLocation; // Out parameter
	if(GetSightRayHitLocation(HitLocation))
	{
		// Tell controlled Tank to aim at location
		GetControlledTank()->AimAt(HitLocation);

	}
}

bool ATankPlayerController::GetSightRayHitLocation(FVector & HitLocation) const
{
	// Find crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX*CrossHairXLocation, ViewportSizeY*CrossHairYLocation);

	// "De-project" screen position of crosshair to world direction
	FVector LookDirection;
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along LookDirection, and see what we hit
		return GetLookVectorHitLocation(LookDirection, HitLocation);
	}
	return false;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const
{
	FVector CameraWorldLocation;
	return DeprojectScreenPositionToWorld(ScreenLocation.X,
										  ScreenLocation.Y, 
										  CameraWorldLocation, 
										  LookDirection);
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector & HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);

	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetControlledTank());

	if ( GetWorld()->LineTraceSingleByChannel(HitResult,
											 StartLocation,
											 EndLocation,
											 ECollisionChannel::ECC_Camera,
											 params) )
	{
		// Set hit location
		HitLocation = HitResult.Location;
		UE_LOG(LogTemp, Warning, TEXT("HitLocation:%s"), *HitLocation.ToString());
		return true;
	}
	HitLocation = FVector(0);
	return false;
}
