// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "CubePawn.generated.h"

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

	/** Called when the presses the spin button. */
	void StartSpin();

	/** Spins the actor 'RotationCount' times in 'Duration' seconds. This is implemented in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent)
	void Spin(int32 RotationCount, float Duration);

	/** Stores the static mesh used to display the cube. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pawn)
	class UStaticMeshComponent* CubeMesh;
	/** Controls the pawn's movement. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pawn)
	class UCubePawnMovementComponent* PawnMovementComponent;

	/** The default amount of time it takes an actor to spin 360 degrees. */
	UPROPERTY(EditAnywhere, Category=Spin)
	float BaseSpinDuration;

};