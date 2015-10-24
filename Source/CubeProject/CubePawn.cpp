// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubePawn.h"
#include "CubePawnMovementComponent.h"

ACubePawn::ACubePawn()
{
	// Call the pawn's Tick() function every frame
	PrimaryActorTick.bCanEverTick = true;

	// Set the default values for the pawn's spin
	BaseSpinDuration = 0.4f;
	BaseThrustForce = 1000.0f;

	// Create the main sphere collider for the cube's collision detection
	USphereComponent* BaseCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = BaseCollisionComponent;
	BaseCollisionComponent->InitSphereRadius(40.0f);
	BaseCollisionComponent->SetCollisionProfileName(TEXT("PAWN"));

	// Create the static mesh used to visualize the cube
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->AttachTo(BaseCollisionComponent);

	// Create the CubePawnMovement component used to control the pawn's movement
	PawnMovementComponent = CreateDefaultSubobject<UCubePawnMovementComponent>(TEXT("MovementComponent"));
	PawnMovementComponent->UpdatedComponent = RootComponent;

	// Make Player0 control the pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACubePawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACubePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACubePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Bind the button input to the correct member functions.
	InputComponent->BindAction("Spin", IE_Released, this, &ACubePawn::OnReleaseActionButton);

	// Bind the axis input to the correct member functions.
	InputComponent->BindAxis("MoveY", this, &ACubePawn::MoveY);
	InputComponent->BindAxis("MoveX", this, &ACubePawn::MoveX);
}

UPawnMovementComponent* ACubePawn::GetMovementComponent() const
{
	return PawnMovementComponent;
}

void ACubePawn::MoveY(float AxisValue)
{
	// If the pawn's movement component exists and is updated by the root
	if (PawnMovementComponent && (PawnMovementComponent->UpdatedComponent == RootComponent))
	{
		// Add an acceleration vector pointing up at the magnitude of the input axis 
		PawnMovementComponent->AddInputVector(FVector::UpVector * AxisValue);
	}
}

void ACubePawn::MoveX(float AxisValue)
{
	// If the pawn's movement component exists and is being updated by the root component
	if (PawnMovementComponent && (PawnMovementComponent->UpdatedComponent == RootComponent))
	{
		// Add an input vector to the movement component, facing right, with magnitude 'AxisValue'
		PawnMovementComponent->AddInputVector(FVector::RightVector * AxisValue);
	}
}

void ACubePawn::OnReleaseActionButton()
{
	// Stores the direction the pawn will spin.
	ERotationDirection::Type SpinDirection = ERotationDirection::Clockwise;

	// Get the current direction in which the user is pressing
	FVector CurrentInputDirection = PawnMovementComponent->GetLastInputVector();

	// If the pawn is moving to the left, make him spin counter-clockwise
	if (CurrentInputDirection.Y < 0)
		SpinDirection = ERotationDirection::CounterClockwise;

	// Tell the Blueprint to spin the actor.
	Spin(1, BaseSpinDuration, SpinDirection);

	// Apply a thrust to the pawn, making him move faster in his current direction of movement
	AddThrust();
}

void ACubePawn::AddThrust()
{
	// Compute the direction the user is making the pawn move
	FVector CurrentInputDirection = PawnMovementComponent->GetLastInputVector().GetSafeNormal2D();

	// Compute the force to apply on the pawn
	FVector ThrustForce = CurrentInputDirection * BaseThrustForce;

	// Add the desired force to the pawn's current velocity
	PawnMovementComponent->Velocity = PawnMovementComponent->Velocity + ThrustForce;
}