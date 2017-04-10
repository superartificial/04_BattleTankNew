// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTankNew.h"
#include "TankAimingComponent.h"
#include "TankAIController.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = GetPawn()->GetWorld()->GetFirstPlayerController();
	if (!ensure(PlayerController)) { UE_LOG(LogTemp, Warning, TEXT("NO PLAYER CONTROLLER")); return;  }
	APawn* PlayerTank =  PlayerController->GetPawn();
	if (!ensure(PlayerTank)) { UE_LOG(LogTemp, Warning, TEXT("NO PlayerTank")); return;  }

	MoveToActor(PlayerTank, AcceptanceRadius);

	APawn* ControlledTank = GetPawn();

	UTankAimingComponent* TankAimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(TankAimingComponent && ControlledTank)) return;
	TankAimingComponent->AimAt(PlayerTank->GetActorLocation());
	if (TankAimingComponent->GetFiringState() == EFiringStatus::Locked) {
		TankAimingComponent->Fire();
	}

}
