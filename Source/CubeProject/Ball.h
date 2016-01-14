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
    UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Called when the ball spawns. Gives the ball an initial push. If bMoveRight is true, the ball is launched to the right of the field. */
	void StartMove(const bool bMoveRight);
    
    /** Enables/disables the ball. If disabled, the ball is no longer rendered on screen. */
    void SetEnabled(const bool bEnabled);
    
    /** Resets the ball to its starting position (0,0) */
    void Reset();    

	/** The amount of time that must pass for the same player to hit the ball twice. If the player could hit the ball multiple times in
	  * in a short time frame, the physics would be glitchy. */
	static constexpr float MULTIPLE_HIT_COOLDOWN = 1.0f;
    
    /** If the angle between the bounce normal and the player velocity is greater than this constant, ignore the player's velocity.
     * That is, do not let the player's velocity impact the ball's bouncing velocity. In fact, if the bounce direction and the
     * player velocity were pointing in opposite directions, the ball would move in a random direction if both vectors would
     * affect the ball's velocity. Thus, we should ignore the player's velocity and only let the hit normal affect the ball's velocity. */
    static constexpr float ANGLE_TO_IGNORE_PLAYER_VELOCITY = 100;

private:
	/** Updates the ball's velocity based on the 'Speed' and 'Direction' variables. */
	void UpdateVelocity();
    
    /** Called when the ball hits a player. Makes the ball bounce in the appropriate direction. */
    void OnHitPlayer(AActor* PlayerHit, FVector HitLocation, FVector HitNormal);

	/** Static mesh used to display the ball. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BallMesh;

	/** The ball's default speed when spawned and when bouncing off a player */
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

