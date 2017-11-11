#pragma once

#include "Components/SceneComponent.h"
#include "MovementConstraint.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CUBEPROJECT_API UMovementConstraint : public USceneComponent
{
    GENERATED_BODY()

public:
    // Default constructor
    UMovementConstraint();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame to update the component
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    /** The movement axis to lock. */
    UPROPERTY(EditAnywhere, Category = "Position Constraint")
    EPlaneConstraintAxisSetting AxisConstrained;

    /** The default value for the locked axis. E.g., if the z-axis is locked and this variable is set to 20, this actor's z-position is locked to 20. */
    UPROPERTY(EditAnywhere, Category = "Position Constraint")
    float DefaultAxisValue;

    /** Caches the actor that owns this component. */
    AActor* ThisActor;
};