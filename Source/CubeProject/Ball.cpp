
#include "CubeProject.h"
#include "CubePawn.h"
#include "Ball.h"
#include "CubeProjectGameMode.h"


// Sets the ball's default properties
ABall::ABall()
{
    PrimaryActorTick.bCanEverTick = true;

    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    RootComponent = BallMesh;

    // Set the ball's collision properties
    this->SetActorEnableCollision(true);
    BallMesh->SetEnableGravity(false);
    BallMesh->SetConstraintMode(EDOFMode::YZPlane);

    BallMesh->SetNotifyRigidBodyCollision(true);
    
    // Set the location, rotation and scale to be relative to the world and not the parent
    BallMesh->SetAbsolute(true, true, true);

    BallMesh->SetLinearDamping(0.05f);
    BallMesh->SetSimulatePhysics(true);

    OnActorBeginOverlap.AddDynamic(this, &ABall::NotifyActorBeginOverlap);

    // Reset the ball at its starting position with zero velocity.
    Reset();
}

// Called when the ball is spawned
void ABall::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABall::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Clamp the delta seconds to one hundredth of a second in case of lag
    float DeltaTime = (DeltaSeconds > 0.01f) ? 0.00833f : DeltaSeconds;
}

/** Gives the ball an initial jolt when spawned. */
void ABall::StartMove(const bool bMoveRight)
{
    // Stores the x-direction the ball will move in (this is the horizontal component of the ball's normalized velocity)
    float DirectionX = 0.0f;
    
    if(bMoveRight)
    {
        // Choose a random positive direction (to move right)
        DirectionX = FMath::FRandRange(0.0,1.0f);
    }
    else
    {
        // Choose a random negative direction (to move left)
        DirectionX = FMath::FRandRange(-1.0f,0.0f);
    }
    
    // Set the ball's initial velocity
    Direction = FVector(0.0f, DirectionX, FMath::FRandRange(-0.5f,0.5f)); // Choose a random starting direction
    Direction.Normalize();
    Speed = DefaultSpeed;

    // Apply the new speed and direction to the ball's physics velocity
    UpdateVelocity();
}

void ABall::SetEnabled(const bool bEnabled)
{
    // Set the ball's properties based on the given boolean
    SetActorHiddenInGame(!bEnabled);
    SetActorEnableCollision(bEnabled);
    SetActorTickEnabled(bEnabled);
}

/** Resets the ball at its starting position (0,0) */
void ABall::Reset()
{
    SetEnabled(true);
    SetActorLocation(FVector::ZeroVector);
    
    BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Direction = FVector(0.0f, 0.0f, 0.0f);
    Speed = 0.0f;
}

/** Update the ball's velocity to match the 'Speed' and 'Direction' variables. */
void ABall::UpdateVelocity()
{
    FVector BallVelocity = Direction * Speed;

    // Clamp the ball's maximum speed
    BallVelocity = BallVelocity.GetClampedToSize(MinSpeed, MaxSpeed);
    BallMesh->SetPhysicsLinearVelocity(BallVelocity);
}

/** Called when the ball is hit by another actor. */
void ABall::NotifyHit(UPrimitiveComponent* MyComponent, AActor* Other, UPrimitiveComponent* OtherComponent, 
    bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{  
    UWorld* World = GetWorld();
    ACubeProjectGameMode* GameMode = World->GetAuthGameMode<ACubeProjectGameMode>();

    // If a cube pawn hit the ball
    if (Other && Other->IsA(ACubePawn::StaticClass()))
    {
        // Inform the ball that it has hit a player. Bounces the ball in the appropriate direction. 
        OnHitPlayer(Other,HitLocation,HitNormal);
    }
    // Else, if anything other than a player hit the ball
    else
    {
        // Make the ball go in the opposite direction it was hit.
        Direction = Direction.MirrorByVector(HitNormal);

        Speed = BallMesh->GetPhysicsLinearVelocity().Size();

        // Play the sound of the ball hitting a wall
        if(GameMode->BallHitWallSound)
        {
            UGameplayStatics::PlaySoundAtLocation(World,GameMode->BallHitWallSound,GetActorLocation());
        }
        
        // Spawn particles where the ball hit the wall.
        if(GameMode->BallHitWallParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(World,GameMode->BallHitWallParticles,HitLocation);
        }
        
        // Play a camera shake when the ball hits a wall
        if(GameMode->BallHitWallCameraShake)
        {
            World->GetFirstPlayerController()->ClientPlayCameraShake_Implementation(GameMode->BallHitWallCameraShake, 1.0f, ECameraAnimPlaySpace::World,
                                                                                    FRotator::ZeroRotator);
        }

        // Update the last actor hit by the ball.
        LastActorHit = Other;
        
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("New speed %d"), Speed));
    }

    // Update the ball's velocity based on the 'Speed' and 'Direction' variables.
    UpdateVelocity();
}

void ABall::OnHitPlayer(AActor* PlayerHit, FVector HitLocation, FVector HitNormal)
{
    UWorld* World = GetWorld();
    ACubeProjectGameMode* GameMode = World->GetAuthGameMode<ACubeProjectGameMode>();
    
    // Stores true if the same actor did not hit the ball twice
    const bool bDifferentActorHitBall = (LastActorHit != PlayerHit);
    // Stores true if enough time has been passed for the same actor to hit the ball twice
    const bool bCooldownElapsed = (World->GetTimeSeconds() - LastHitTime) >= ABall::MULTIPLE_HIT_COOLDOWN;
    
    // If a different actor hit the ball, or enough time has elapsed for the same actor to hit the ball twice, bounce the ball off the actor which was hit
    if (bDifferentActorHitBall || bCooldownElapsed)
    {
        // Get the direction from the player's center to the ball's center. This is the direction the ball will bounce
        FVector BounceDirection = (this->GetActorLocation() - PlayerHit->GetActorLocation()).GetSafeNormal();
        Direction = BounceDirection;
        Speed = DefaultSpeed;
        
        float AngleBetweenBounceAndVelocity_Radians = acosf(FVector::DotProduct(BounceDirection,PlayerHit->GetVelocity().GetSafeNormal()));
        float AngleBetweenBounceAndVelocity_Degrees = FMath::RadiansToDegrees(AngleBetweenBounceAndVelocity_Radians);
        
        if(GEngine)
            GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::White,FString::Printf(TEXT("Angle between bounce direction and player velocity: %f"),AngleBetweenBounceAndVelocity_Degrees));
        
        const bool bIgnorePlayerVelocity = (AngleBetweenBounceAndVelocity_Degrees > ANGLE_TO_IGNORE_PLAYER_VELOCITY);
        
        // If the player's velocity should affect the ball's bounce velocity
        if(!bIgnorePlayerVelocity)
        {
            // Add the cube's velocity to the ball's direction. Hence, the ball will bounce in the direction the player is moving
            Direction += PlayerHit->GetVelocity() * PlayerSpeedBounceFactor;
        }
        
        // Play the sound of the ball hitting a player
        if(GameMode->BallHitPlayerSound)
        {
            UGameplayStatics::PlaySoundAtLocation(World,GameMode->BallHitPlayerSound,GetActorLocation());
        }
        
        // Spawn particles where the ball hit the player.
        if(GameMode->BallHitPlayerParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(World,GameMode->BallHitPlayerParticles,PlayerHit->GetActorLocation());
        }
        
        // Play a camera shake when the ball hits a player
        if(GameMode->BallHitPlayerCameraShake)
        {
            World->GetFirstPlayerController()->ClientPlayCameraShake(GameMode->BallHitPlayerCameraShake,1.0f,ECameraAnimPlaySpace::World,
                                                                     FRotator::ZeroRotator);
        }
        
        // Update the last time the ball was hit by an actor
        LastHitTime = World->GetTimeSeconds();
        LastActorHit = PlayerHit;
    }
}

void ABall::NotifyActorBeginOverlap(AActor* Other)
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Begin to hit pawn");

        // Add the cube's velocity to the ball's direction. Hence, the ball will bounce in the direction the player is moving
        Direction += Other->GetVelocity() * PlayerSpeedBounceFactor;
    }

    // Update the ball's velocity based on the 'Speed' and 'Direction' variables.
    UpdateVelocity();
}

