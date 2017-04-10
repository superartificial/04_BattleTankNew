// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTankNew.h"
#include "TankAimingComponent.h"
#include "TankBarrel.h"
#include "TankTurret.h"
#include "Projectile.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void UTankAimingComponent::Initialise(UTankTurret * TurretToSet, UTankBarrel * BarrelToSet)
{
	if (TurretToSet) Turret = TurretToSet;
	if (BarrelToSet) Barrel = BarrelToSet;
}

void UTankAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (AmmoCount <= 0) {
		FiringStatus = EFiringStatus::OutOfAmmo;
	} else if ((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds) {
		FiringStatus = EFiringStatus::Reloading;
	}
	else if (IsBarrelMoving()) {
		FiringStatus = EFiringStatus::Aiming;
	}
	else {
		FiringStatus = EFiringStatus::Locked;
	}
}

void UTankAimingComponent::BeginPlay()
{
	LastFireTime = FPlatformTime::Seconds();	
}

bool UTankAimingComponent::IsBarrelMoving()
{
	if (!ensure(Barrel)) return false;
	return !AimDirection.Equals(Barrel->GetForwardVector(),0.01f);
}

EFiringStatus UTankAimingComponent::GetFiringState() const
{
	return FiringStatus;
}

void UTankAimingComponent::Fire()
{

	if (FiringStatus == EFiringStatus::Locked && FiringStatus == EFiringStatus::Aiming) {
		if (!ensure(Barrel && ProjectileBlueprint)) return;
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileBlueprint, Barrel->GetSocketLocation(FName("Projectile")), Barrel->GetSocketRotation(FName("Projectile")));
		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
		AmmoCount--;
		UE_LOG(LogTemp, Warning, TEXT("AmmoCount: %i"), AmmoCount);
	}

}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Barrel)) return;
	
	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
	
	if (UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	)) {
		AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveTowardsAimDirection();
	}

}

void UTankAimingComponent::MoveTowardsAimDirection()
{

	if (!ensure(Barrel && Turret)) return;

	// Work-out difference between current barrel roation, and AimDirection
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;
	float RotateAngle = DeltaRotator.Yaw;

	if (RotateAngle > 180) {
		RotateAngle -= 360;
	} else if (RotateAngle < -180) {
		RotateAngle += 360;
	}

	Turret->Rotate(RotateAngle);
	Barrel->Elevate(DeltaRotator.Pitch);
}


