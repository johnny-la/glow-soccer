#include "CubeProject.h"
#include "MovementConstraint.h"

// Sets default values for the component
UMovementConstraint::UMovementConstraint()
{
	// Call BeginPlay()
	bWantsBeginPlay = true;
	// Call Tick() every frame
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMovementConstraint::BeginPlay()
{
	// Setup the component
	Super::BeginPlay();
	
	// Cache the actor that owns this component
	ThisActor = GetOwner();
}

// Called every frame to update the component
void UMovementConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Retrieve the actor's current location
	FVector ActorLocation = ThisActor->GetActorLocation();

	// Lock the constrained axis to its default value
	switch (AxisConstrained)
	{
	case EPlaneConstraintAxisSetting::X:
		ActorLocation.X = this->DefaultAxisValue;
		break;
	case EPlaneConstraintAxisSetting::Y:
		ActorLocation.Y = this->DefaultAxisValue;
		break;
	case EPlaneConstraintAxisSetting::Z:
		ActorLocation.Z = this->DefaultAxisValue;
		break;
	}

	// Update the actor's location to lock one of its axes.
	ThisActor->SetActorLocation(ActorLocation);
}