#pragma once

#include "GameFramework/GameState.h"
#include "CubeProjectGameState.generated.h"

/** Determines the current state of the game */
namespace EGameState
{
    enum Type
    {
        RESET,
        WAITING_TO_START,
        PUSH_BALL,
        PLAYING,
        UPDATE_SCORE
    };
}

UCLASS()
class CUBEPROJECT_API ACubeProjectGameState : public AGameState
{
    GENERATED_BODY()
    
public:
    // Initializes the game state's properties
    ACubeProjectGameState();
    
    // Called every frame to update the game's state
    virtual void Tick(float DeltaTime) override;
    
    /** Sets the current state of the game. This affects the logic in the Tick() method. */
    void SetState(EGameState::Type NewState);
    
    /** The amount of time it takes for the game to restart after a goal */
    static const float GAME_START_TIMER_DURATION;
    
private:
    /** Called once the timer to start the game elapses. Transitions the game to "PUSH_BALL" state and unlocks player input. */
    void OnGameStart();
    
    /** Handle to manage the timer displayed when the game is about to start. */
    FTimerHandle GameStartTimerHandle;
    
    /** Stores the current state of the game. */
    EGameState::Type CurrentState;
};
