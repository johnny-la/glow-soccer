// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "CubePawn.generated.h"

/** Denotes a rotation direction (either clockwise or counter-clockwise) */
UENUM(BlueprintType)
namespace ERotationDirection
{
	enum Type
	{
		Clockwise,
		CounterClockwise
	};
}

UCLASS()
class CUBEPROJECT_API ACubePawn : public APawn
{
	GENERATED_BODY()

public:
	/** Sets the default values and components for the pawn. */
	ACubePawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind input to functionality
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Override GetMovementComponent() to return the pawn's custom movement component. */
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	/** Moves the pawn vertically based on the axis input. */
	void MoveY(float AxisValue);
	/** Moves the pawn horizontally based on the strength of the axis input. */
	void MoveX(float AxisValue);

	/** Called when the user releases the spin button. */
	void OnReleaseActionButton();

	/** Adds a force to the pawn, making him move faster in his current movement direction. */
	void AddThrust();

	/** 
	 * Spins the actor 'RotationCount' times in 'Duration' seconds. This is implemented in Blueprint. 
	 * @param RotationCount The amount of times the pawn spins 360 degrees
	 * @param Duration The amount of time it takes for the pawn to perform the spin
	 * @param SpinDirection The direction of the spin (clockwise or counter-clockwise)
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void Spin(int32 RotationCount, float Duration, ERotationDirection::Type SpinDirection);

	/** Stores the collider component that controls the pawn's physics. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pawn)
	class USphereComponent* BaseCollisionComponent;
	/** Stores the static mesh used to display the cube. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pawn)
	class UStaticMeshComponent* CubeMesh;
	/** Controls the pawn's movement. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pawn)
	class UCubePawnMovementComponent* PawnMovementComponent;

	/** The default amount of time it takes an actor to spin 360 degrees. */
	UPROPERTY(EditAnywhere, Category=Spin)
	float BaseSpinDuration;

	/** The default amount of force applied to the pawn when a spin is performed. */
	UPROPERTY(EditAnywhere, Category=Spin)
	float BaseThrustForce;

private:
	/** If true, the pawn is currently spinning. The pawn can't spin again until it is done spinning. */
	bool bSpinning;

	/** The amount of time the pawn has been spinning. */
	float SpinTime;
	/** The amount of time to wait between two successive spins. */
	float SpinCooldown;

};