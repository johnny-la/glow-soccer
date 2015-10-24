// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubePawn.h"
#include "CubePawnMovementComponent.h"

ACubePawn::ACubePawn()
{
	// Call the pawn's Tick() function every frame
	PrimaryActorTick.bCanEverTick = true;

	// Set the default amount of time it takes the actor to spin 360 degrees
	BaseSpinDuration = 1.0f;

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

// Called when the game starts or when spawned
void ACubePawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACubePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind input to functionality
void ACubePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Bind the button input to the correct member functions.
	InputComponent->BindAction("Spin", IE_Released, this, &ACubePawn::StartSpin);

	// Bind the axis input to the correct member functions.
	InputComponent->BindAxis("MoveY", this, &ACubePawn::MoveY);
	InputComponent->BindAxis("MoveX", this, &ACubePawn::MoveX);
}

/** Returns the custom movement component used by this pawn. */
UPawnMovementComponent* ACubePawn::GetMovementComponent() const
{
	return PawnMovementComponent;
}

/** Moves the pawn vertically based on the axis input. */
void ACubePawn::MoveY(float AxisValue)
{
	// If the pawn's movement component exists and is updated by the root
	if (PawnMovementComponent && (PawnMovementComponent->UpdatedComponent == RootComponent))
	{
		// Add an acceleration vector pointing up at the magnitude of the input axis 
		PawnMovementComponent->AddInputVector(FVector::UpVector * AxisValue);
	}
}

/** Moves the pawn horizontally based on the strength of the axis input. */
void ACubePawn::MoveX(float AxisValue)
{
	// If the pawn's movement component exists and is being updated by the root component
	if (PawnMovementComponent && (PawnMovementComponent->UpdatedComponent == RootComponent))
	{
		// Add an input vector to the movement component, facing right, with magnitude 'AxisValue'
		PawnMovementComponent->AddInputVector(FVector::RightVector * AxisValue);
	}
}

/** Called when the spinning button is released. */
void ACubePawn::StartSpin()
{
	// Tell the Blueprint to spin the actor.
	Spin(1, BaseSpinDuration);
}