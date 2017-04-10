// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTankNew.h"
#include "TankBarrel.h"

void UTankBarrel::Elevate(float RelativeSpeed)
{	
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, 1);
	auto ElevationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto Elevation = FMath::Clamp<float>(RelativeRotation.Pitch + ElevationChange, MinElevationDegrees, MaxElevationDegrees);

	SetRelativeRotation(FRotator(Elevation, 0, 0));

}


