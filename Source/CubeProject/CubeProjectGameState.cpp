#include "CubeProject.h"
#include "CubeProjectGameState.h"
#include "CubeProjectGameMode.h"

/** The amount of time it takes for the game to restart after a goal */
const float ACubeProjectGameState::GAME_START_TIMER_DURATION = 2.0f;

ACubeProjectGameState::ACubeProjectGameState()
{
    // Call the Tick() function every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // The game always starts in 'Waiting' mode
    CurrentState = EGameState::WAITING_TO_START;
}

void ACubeProjectGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Get the World instance controlling the game
    UWorld* World = GetWorld();
    
    // Get the game mode controlling the rules of the game. This is used to reset the board and make appropriate changes to the game
    ACubeProjectGameMode* GameMode = (ACubeProjectGameMode*)World->GetAuthGameMode();
    
    // If the match is currently in progress
    if(IsMatchInProgress())
    {
        // Switch the current state and update the game state accordingly.
        switch(CurrentState)
        {
            case EGameState::RESET:
            {
                // Reset the game field so that each player is in their starting position
                GameMode->ResetField();
                // Disable player input until the timer elapses
                GameMode->SetPlayerInputEnabled(false);
                // Start a timer which will call OnGameStart once complete. Once this method is called, game state is switched to "PUSH_BALL"
                World->GetTimerManager().SetTimer(GameStartTimerHandle,this,&ACubeProjectGameState::OnGameStart,GAME_START_TIMER_DURATION,false);
                CurrentState = EGameState::WAITING_TO_START;
                break;
            }
            case EGameState::WAITING_TO_START:
            {
                // A timer currently active. Once complete the game will be switched to PUSH_BALL state.
                break;
            }
            case EGameState::PUSH_BALL:
            {
                // Enable player input since the game has started.
                GameMode->SetPlayerInputEnabled(true);
                // Gives the ball an initial push to get the game started.
                GameMode->PushBall();
                CurrentState = EGameState::PLAYING;
                break;
            }
            case EGameState::PLAYING:
            {
                break;
            }
            case EGameState::UPDATE_SCORE:
            {
                CurrentState = EGameState::WAITING_TO_START;
                break;
            }
                
            default:
                // By default, wait for the game to start.
                CurrentState = EGameState::WAITING_TO_START;
                break;
        }
    }
}

/** Called once the timer elapses to start the game. Transitions the game to "PUSH_BALL" state */
void ACubeProjectGameState::OnGameStart()
{
    // Push the ball to start the game.
    CurrentState = EGameState::PUSH_BALL;
}

void ACubeProjectGameState::SetState(EGameState::Type GameState)
{
    // Update the game's current state. The Tick() function then calls the appropriate methods based on this new state.
    CurrentState = GameState;
}