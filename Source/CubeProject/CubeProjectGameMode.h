#pragma once

#include "GameFramework/GameMode.h"
#include "CubeProjectGameMode.generated.h"

UCLASS()
class CUBEPROJECT_API ACubeProjectGameMode : public AGameMode
{
    GENERATED_BODY()
    
public:
    // Called to initialize the game mode's properties
    ACubeProjectGameMode();
    
    // Called when the game starts
    virtual void BeginPlay() override;
    
    /** Called when the ball overlaps another actor. If a goal is overlapped, a point is given to the correct player. */
    UFUNCTION()
    void OnBallOverlap(AActor* OtherActor);
    
    /** Called when a player scores and the game restarts. This method displays the "READY, GO!!" message on screen. 
      * Note: This method is called from ACubeProjectGameMode::Tick() when the game is in "RESET" state. */
    UFUNCTION(BlueprintImplementableEvent)
    void ShowGameStartTimer();
    
    /** Gives the ball a small push to start the game. Called from ACubeProjectGameState::Tick() when in PUSH_BALL state. */
    void PushBall();
    
    /** Called when a player scores. Resets the players and the ball at their default locations. Called from ACubeProjectGameState::Tick(). */
    void ResetField();
    
    /** Sets whether or not the players can move their pawns. If true, the users can move their avatars. Called in ACubeProjectGameState.
      * When the timer is enabled and the game is waiting to start, player input is disabled. */
    void SetPlayerInputEnabled(bool bEnabled);
    
    /** Returns the score obtained by the player which starts on the left-hand side of the field. */
    int32 GetLeftPlayerScore() const;
    /** Returns the score obtained by the player which starts on the right-hand side of the field. */
    int32 GetRightPlayerScore() const;
    
    /** The position in which the score text is displayed. (This is the position of the score on the right-hand side) */
    static const FVector SCORE_TEXT_POSITION;

private:
    /** Called when the ball enters one of the goals and one of the players score. 
      * @param bRightPlayerScored true if the right-hand side player (player 2) scored. False if player 1 scored.
     */
    void OnGoal(bool bRightPlayerScored);
    
    /** The pawn blueprint used for the first player. */
    TSubclassOf<class APawn> Player1PawnClass;
    /** The pawn blueprint used for the second player. */
    TSubclassOf<class APawn> Player2PawnClass;
    /** The blueprint used to spawn the ball. */
    TSubclassOf<class ABall> BallClass;
    
    /** The Blueprint used for the score text at the top of the screen. */
    TSubclassOf<class ATextRenderActor> ScoreTextClass;
    
    /** The pawn controlled by the first player. */
    APawn* Player1Pawn;
    /** The pawn controlled by the second player. */
    APawn* Player2Pawn;
    /** The ball currently on the field. */
    class ABall* Ball;
    
    /** The text actor which displays the left-hand score */
    ATextRenderActor* ScoreTextLeft;
    /** The text actor which displays the right-hand score */
    ATextRenderActor* ScoreTextRight;
    
    /** The score for the player on the left. */
    int32 LeftPlayerScore;
    /** The score for the player on the right. */
    int32 RightPlayerScore;
};