#pragma once

#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class CUBEPROJECT_API ABall : public AActor
{
	GENERATED_BODY()

public:
	// Sets the ball's default properties
	ABall();
	
	// Called when the ball is spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** Called when the ball is hit by another actor. */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComponent, 
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Called when another actor begins to touch the ball. */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Called when the ball spawns. Gives the ball an initial push. If bMoveRight is true, the ball is launched to the right of the field. */
	void StartMove(const bool bMoveRight);
    
    /** Resets the ball to its starting position (0,0) */
    void Reset();

	/** The amount of time that must pass for the same player to hit the ball twice. If the player could hit the ball multiple times in
	  * in a short time frame, the physics would be glitchy. */
	static const float MULTIPLE_HIT_COOLDOWN;

private:
	/** Updates the ball's velocity based on the 'Speed' and 'Direction' variables. */
	void UpdateVelocity();

	/** Static mesh used to display the ball. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BallMesh;

	/** The ball's default speed when spawned */
	UPROPERTY(EditAnywhere, Category=BallPhysics)
	float DefaultSpeed = 300.0f;

	/** The ball's minimum speed. */
	UPROPERTY(EditAnywhere, Category = BallPhysics)
	float MinSpeed = 300.0f;
	/** The ball's maximum speed. */
	UPROPERTY(EditAnywhere, Category = BallPhysics)
	float MaxSpeed = 600.0f;

	/** Influences how much player momentum is transferred to the ball when a player hits the ball. The higher the value, the more speed
	  * will be transferred from the player to the ball. */
	UPROPERTY(EditAnywhere, Category = BallPhysics)
	float PlayerSpeedBounceFactor = 0.001f;

	/** The ball's current speed. */
	float Speed;
	/** The direction in which the ball is moving. */
	FVector Direction;

	/** Stores the last actor hit by the ball. Used to avoid bouncing off a player multiple times a second. */
	AActor* LastActorHit;
	/** The last time at which the ball was hit. Used to avoid bouncing off a player multiple times a second. */
	float LastHitTime; 

};

