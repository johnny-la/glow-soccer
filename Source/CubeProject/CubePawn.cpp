// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubePawn.h"
#include "CubePawnMovementComponent.h"
#include "CubeProjectGameMode.h"

ACubePawn::ACubePawn()
{
	// Call the pawn's Tick() function every frame
	PrimaryActorTick.bCanEverTick = true;

	// Set the default values for the pawn's spin
	BaseSpinDuration = 0.4f;
	BaseThrustForce = 1000.0f;

	// Create the main sphere collider for the cube's collision detection
	BaseCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = BaseCollisionComponent;
	BaseCollisionComponent->InitSphereRadius(40.0f);
	BaseCollisionComponent->SetCollisionProfileName(TEXT("PAWN"));
	BaseCollisionComponent->SetNotifyRigidBodyCollision(true);
    
    // Create the main box collider for the cube's collision detection
    /*BaseCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
    RootComponent = BaseCollisionComponent;
    BaseCollisionComponent->InitBoxExtent(FVector(0.0f,20.0f,20.0f));
    BaseCollisionComponent->SetCollisionProfileName(TEXT("PAWN"));
    BaseCollisionComponent->SetNotifyRigidBodyCollision(true);*/

	// Create the static mesh used to visualize the cube
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->AttachTo(BaseCollisionComponent);

	// Create the CubePawnMovement component used to control the pawn's movement
	PawnMovementComponent = CreateDefaultSubobject<UCubePawnMovementComponent>(TEXT("MovementComponent"));
	PawnMovementComponent->UpdatedComponent = RootComponent;

	// Make Player0 control the pawn
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACubePawn::BeginPlay()
{
	Super::BeginPlay();
    
    // Store the player's spawn location. This way, the pawn will respawn at the same position when a goal is scored and ACubePawn::Reset() is called
    StartPosition = GetActorLocation();
}

void ACubePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the pawn is spinning
	if (bSpinning)
	{
		// If the spinning cooldown has elapsed
		if (SpinTime > SpinCooldown)
		{
			// Inform the pawn that it has done spinning, and that it can spin again
			bSpinning = false;
			SpinTime = 0;
		}

		// Increment the amount of time the pawn has been spinning
		SpinTime += DeltaTime;
	}
}

void ACubePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Bind the button inputs to the correct member functions.
	InputComponent->BindAction("Spin_P1", IE_Released, this, &ACubePawn::OnReleaseActionButton);
    // Since only one pawn can receive input from the keyboard, we bind the buttons for both players using this single pawn class
    InputComponent->BindAction("Spin_P2", IE_Released, this, &ACubePawn::OnReleaseActionButton_P2);
    InputComponent->BindAction("Restart", IE_Released, this, &ACubePawn::RestartGame);
    InputComponent->BindAction("StartGame", IE_Released, this, &ACubePawn::StartGame);
    
	// Bind the axis inputs to the correct member functions.
	InputComponent->BindAxis("MoveY_P1", this, &ACubePawn::MoveY);
	InputComponent->BindAxis("MoveX_P1", this, &ACubePawn::MoveX);
    // Since only one pawn can receive input from the keyboard, we bind the axes for both players using this single pawn class
    InputComponent->BindAxis("MoveY_P2", this, &ACubePawn::MoveY_P2);
    InputComponent->BindAxis("MoveX_P2", this, &ACubePawn::MoveX_P2);
    
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Yellow,"Setup player input component");
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

/** Moves the second player 'Pawn_P2' in the y-direction. We do this because only a single pawn can receive keyboard input. Thus,
  * this pawn is responsible for moving the second one too. */
void ACubePawn::MoveY_P2(float AxisValue)
{
    // If the second player's pawn has been assigned, tell the second player that his y-input has changed
    if(Pawn_P2)
    {
        // Delegate the MoveY call to the second player's pawn. This must be done through this class since
        // only one pawn can receive keyboard input and thus must call the second pawn's methods manually
        Pawn_P2->MoveY(AxisValue);
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

/** Moves the second player 'Pawn_P2' in the y-direction. We do this because only a single pawn can receive keyboard input. Thus,
  * this pawn is responsible for moving the second one too. */
void ACubePawn::MoveX_P2(float AxisValue)
{
    // If the second player's pawn has been assigned, tell the second player that his x-input has changed
    if(Pawn_P2)
    {
        // Delegate the MoveX call to the second player's pawn. This must be done through this class since
        // only one pawn can receive keyboard input, and thus must call the second pawn's methods manually
        Pawn_P2->MoveX(AxisValue);
    }
}

void ACubePawn::OnReleaseActionButton()
{
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::White,"SPIN P1");
	// If the pawn is already spinning, return. The pawn can't spin again until it is done its current spin.
	if (bSpinning)
		return; 

	// Stores the direction the pawn will spin.
	ERotationDirection::Type SpinDirection = ERotationDirection::Clockwise;

	// Get the current direction in which the user is pressing
	FVector CurrentInputDirection = PawnMovementComponent->GetLastInputVector();

	// If the pawn is moving to the left, make him spin counter-clockwise (y = horizontal)
	if (CurrentInputDirection.Y < 0)
		SpinDirection = ERotationDirection::CounterClockwise;

	// Update the amount of time the user has to wait before performing another spin
	SpinCooldown = BaseSpinDuration;

	// Tell the Blueprint to spin the actor.
	Spin(1, BaseSpinDuration, SpinDirection);

	// Apply a thrust to the pawn, making him move faster in his current direction of movement
	AddThrust();

	// The pawn can't spin again until it is done its current spin
	bSpinning = true;
    
    // Get the game mode which stores the game sounds
    ACubeProjectGameMode* GameMode = GetWorld()->GetAuthGameMode<ACubeProjectGameMode>();
    
    // Play the sound of the player spinning
    if(GameMode->PlayerSpinSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(),GameMode->PlayerSpinSound,GetActorLocation());
    }
    
    // Play particles at the position the player is spinning
    if(GameMode->PlayerSpinParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),GameMode->PlayerSpinParticles,GetActorLocation());
    }
}

/** Called when the second player releases his action button. We need this method because only a single pawn can receive
  * keyboard input. Hence, this pawn class is responsible for moving the second one too. */
void ACubePawn::OnReleaseActionButton_P2()
{
    // If the second player's pawn has been assigned, tell the second player that his action button has been released
    if(Pawn_P2)
    {
        if(GEngine)
            GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::White,"Pressed action button (P2)");
        
        // Tell the second player that his action button has been released. This must be done through this class since
        // only one pawn can receive keyboard input
        Pawn_P2->OnReleaseActionButton();
    }
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

/** Called when the user presses ENTER in the main menu */
void ACubePawn::StartGame()
{
    // Get the game mode controlling the game
    ACubeProjectGameMode* GameMode = GetWorld()->GetAuthGameMode<ACubeProjectGameMode>();
    
    if(GameMode)
    {
        // Tell the game mode to start the game
        GameMode->StartGame();
    }
}

void ACubePawn::RestartGame()
{
    // Get the game mode currently controlling the game
    ACubeProjectGameMode* GameMode = GetWorld()->GetAuthGameMode<ACubeProjectGameMode>();
    
    if(GameMode)
    {
        // Restart the game.
        GameMode->RestartGame();
    }
}

void ACubePawn::Reset()
{
    // Resets the actor at his starting location
    SetActorLocation(StartPosition);
    
    // Resets the cube mesh to zero velocity.
    CubeMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    // Reset the input vector to zero so that the pawn doesn't move due to input.
    PawnMovementComponent->ConsumeInputVector();
    PawnMovementComponent->StopMovementImmediately();
    
}