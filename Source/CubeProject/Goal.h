#pragma once

#include "GameFramework/Actor.h"
#include "Goal.generated.h"

UCLASS()
class CUBEPROJECT_API AGoal : public AActor
{
    GENERATED_BODY()
    
public:
    // Initializes the goal's properties
    AGoal();
    
    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    
    /** Returns true if the goal is on the right-hand side of the field. If so, this goal belongs to player 2. */
    FORCEINLINE bool IsRightHandSideGoal() { return bRightHandSideGoal; };
    
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* TriggerVolume;
    
    /** If true, this goal is on the right-hand side of the field, and thus belongs to player 2. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goal", meta = (AllowPrivateAccess = "true"))
    bool bRightHandSideGoal;
};