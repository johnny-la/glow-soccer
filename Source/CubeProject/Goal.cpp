#include "CubeProject.h"
#include "Goal.h"

AGoal::AGoal()
{
    // Call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // Create the trigger box used to detect a ball in the goal
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;
    
    // Set the trigger volume to overlap over every object
    TriggerVolume->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerVolume->GetBodyInstance()->SetObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerVolume->GetBodyInstance()->SetResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void AGoal::BeginPlay()
{
    Super::BeginPlay();
}

void AGoal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}