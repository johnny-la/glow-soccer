
#include "CubeProject.h"
#include "CubePawn.h"
#include "Ball.h"

// The amount of time which must elapse for the same actor to hit the ball twice
const float ABall::MULTIPLE_HIT_COOLDOWN = 0.1f;

// Sets the ball's default properties
ABall::ABall()
{
	// Allow the Tick() method to be called every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh component used to render the ball
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	// Set the ball's collision properties
	this->SetActorEnableCollision(true);
	BallMesh->SetEnableGravity(false);
	BallMesh->SetConstraintMode(EDOFMode::YZPlane);

	BallMesh->SetNotifyRigidBodyCollision(true);
	
	// Set the location, rotation and scale to be relative to the world and not the parent
	BallMesh->SetAbsolute(true, true, true);

	// Enable physics on the ball
	BallMesh->SetLinearDamping(0.05f);
	BallMesh->SetSimulatePhysics(true);

	// Call the NotifyActorBeginOverlap() method when an actor hits the ball.
	OnActorBeginOverlap.AddDynamic(this, &ABall::NotifyActorBeginOverlap);

	// Set the speed and direction of the ball to zero
	Direction = FVector(0.0f, 0.0f, 0.0f);
	Speed = 0.0f;
}

// Called when the ball is spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();

	StartMove();
}

// Called every frame
void ABall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Clamp the delta seconds to one hundredth of a second in case of lag
	float DeltaTime = (DeltaSeconds > 0.01f) ? 0.00833f : DeltaSeconds;

	// Retrieve the ball's Transform component
	/*FTransform Transform = BallMesh->GetRelativeTransform();
	// Translate the ball based on its direction and velocity
	Transform.AddToTranslation(Direction * Speed * DeltaTime);
	// Update the ball's transform so that its position is updated
	BallMesh->SetRelativeTransform(Transform);*/

	// Compute the ball's current velocity
	FVector BallVelocity = Direction * Speed;

	// Clamp the ball's maximum speed
	BallVelocity = BallVelocity.GetClampedToMaxSize(MaxSpeed);

	// Update the ball's linear velocity based on the vector computed above.
	BallMesh->SetPhysicsLinearVelocity(BallVelocity);
}

/** Gives the ball an initial jolt when spawned. */
void ABall::StartMove()
{
	// Set the ball's initial velocity
	Direction = FVector(0.0f, 0.5f, 0.5f);
	Speed = DefaultSpeed;
}

/** Called when the ball is hit by another actor. */
void ABall::NotifyHit(UPrimitiveComponent* MyComponent, AActor* Other, UPrimitiveComponent* OtherComponent, 
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Set the ball's velocity to zero. The ball's movement and velocity is controlled manually in the Tick() method.
	/*BallMesh->SetAllPhysicsLinearVelocity(FVector(0, 0, 0));*/

	// Stores true if the same actor did not hit the ball twice 
	const bool bDifferentActorHitBall = (LastActorHit != Other);
	// Stores true if enough time has been passed for the same actor to hit the ball twice
	const bool bCooldownElapsed = (GetWorld()->GetTimeSeconds() - LastHitTime) >= ABall::MULTIPLE_HIT_COOLDOWN;

	// If a different actor hit the ball, or enough time has elapsed for the same actor to hit the ball twice, bounce the ball off the actor which was hit
	if (bDifferentActorHitBall || bCooldownElapsed)
	{
		// Make the ball go in the opposite direction it was hit.
		Direction = Direction.MirrorByVector(HitNormal);

		// If a cube pawn hit the ball
		if (Other && Other->IsA(ACubePawn::StaticClass()))
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Hit pawn");

			// Add the cube's velocity to the ball's direction. Hence, the ball will bounce in the direction the player is moving
			Direction += Other->GetVelocity() * PlayerSpeedBounceFactor
		}
	}

	// Update the last time the ball was hit by an actor
	LastHitTime = GetWorld()->GetTimeSeconds();
	LastActorHit = Other;

}

/** Called when another actor begins to touch the ball. */
void ABall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, "Begin to hit pawn");
}

