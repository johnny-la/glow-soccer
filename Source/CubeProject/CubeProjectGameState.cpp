#include "CubeProject.h"
#include "CubeProjectGameState.h"
#include "CubeProjectGameMode.h"
#include "CubeProjectLevelScriptActor.h"
#include "Ball.h"

/** The amount of time it takes for the game to restart after a goal */
const float ACubeProjectGameState::GAME_START_TIMER_DURATION = 1.0f;

ACubeProjectGameState::ACubeProjectGameState()
{
    // Call the Tick() function every frame
    PrimaryActorTick.bCanEverTick = true;
    
    // The game always starts in 'BOOT' mode
    CurrentState = EGameState::GAME_BOOT;
}

void ACubeProjectGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Get the World instance controlling the game
    UWorld* World = GetWorld();
    
    // Obtain the level blueprint. We call its methods to display Matinee animations such as "READY, GO!!"
    ACubeProjectLevelScriptActor* LevelBlueprint = Cast<ACubeProjectLevelScriptActor>(World->GetLevelScriptActor());
    
    // Get the game mode controlling the rules of the game. This is used to reset the board and make appropriate changes to the game
    ACubeProjectGameMode* GameMode = (ACubeProjectGameMode*)World->GetAuthGameMode();
    
    // If the match is currently in progress
    if(IsMatchInProgress())
    {
        // Switch the current state and update the game state accordingly.
        switch(CurrentState)
        {
            case EGameState::GAME_BOOT:
            {
                // Disable player input when the game starts
                GameMode->SetPlayerInputEnabled(false);
                // Transition to the main menu when the game boots
                CurrentState = EGameState::MAIN_MENU;
                break;
            }
            case EGameState::MAIN_MENU:
            {
                // Wait for the user to press enter
                break;
            }
            case EGameState::RESET:
            {
                // Reset the game field so that each player is in their starting position
                GameMode->ResetField();
                // Update the text displaying the score
                GameMode->UpdateScoreText();
                // Disable player input until the timer elapses
                GameMode->SetPlayerInputEnabled(false);
                // Start a timer which will call OnGameStart once complete. Once this method is called, game state is switched to "PUSH_BALL"
                World->GetTimerManager().SetTimer(GameStartTimerHandle,this,&ACubeProjectGameState::OnGameStart,GAME_START_TIMER_DURATION,false);
                if(LevelBlueprint)
                    // Tell the level Blueprint to display the "READY, GO!!" message.
                    LevelBlueprint->ShowGameStartTimer();
                // Wait until the timer elapses before starting the game 
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
                // Gives the ball an initial push to get the game started. If the right-most player scored last, shoot the ball to the left
                GameMode->PushBall(!GameMode->DidRightPlayerScoreLast());
                CurrentState = EGameState::PLAYING;
                break;
            }
            case EGameState::PLAYING:
            {
                // Wait for a player to score
                break;
            }
            case EGameState::GAME_OVER:
            {
                if(LevelBlueprint)
                    // Tell the level blueprint to display the "WIN" message for the player that won. If the right player scored last,
                    // the yellow player won the game. Thus, pass in true so that "Yellow Player Wins" is displayed.
                    LevelBlueprint->ShowWinMessage(GameMode->DidRightPlayerScoreLast());
                
                // Update the text displaying the score
                GameMode->UpdateScoreText();
                
                // Disable player input
                GameMode->SetPlayerInputEnabled(false);
                // Disable the ball until the game is restarted
                GameMode->GetBall()->SetEnabled(false);
                // Wait for the user to press the restart key
                CurrentState = EGameState::WAITING_TO_RESTART;
                break;
            }
            case EGameState::WAITING_TO_RESTART:
            {
                // Wait for the user to press  the restart key. At that point, the game will be transitioned to RESET state
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

EGameState::Type ACubeProjectGameState::GetState() const
{
    return CurrentState;
}

void ACubeProjectGameState::SetState(EGameState::Type GameState)
{
    // Update the game's current state. The Tick() function then calls the appropriate methods based on this new state.
    CurrentState = GameState;
}