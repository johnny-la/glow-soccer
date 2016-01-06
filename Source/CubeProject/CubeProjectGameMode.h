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
    
    /** Gives the ball a small push to start the game. Called from ACubeProjectGameState::Tick() when in PUSH_BALL state. 
      * @param bMoveRight If true, the ball is pushed to the right. Otherwise, it is pushed to the left
      */
    void PushBall(const bool bMoveRight);
    
    /** Called when a player scores. Resets the players and the ball at their default locations. Called from ACubeProjectGameState::Tick(). */
    void ResetField();
    
    /** Sets whether or not the players can move their pawns. If true, the users can move their avatars. Called in ACubeProjectGameState.
      * When the timer is enabled and the game is waiting to start, player input is disabled. */
    void SetPlayerInputEnabled(bool bEnabled);
    
    /** Returns the score obtained by the player which starts on the left-hand side of the field. */
    int32 GetLeftPlayerScore() const;
    /** Returns the score obtained by the player which starts on the right-hand side of the field. */
    int32 GetRightPlayerScore() const;
    
    /** Returns the ball currently on the field. */
    class ABall* GetBall();
    
    /** If true, the right player won last. i.e., the player starting on the right of the field scored the last goal.
     * Used in ACubeProjectGameState::Tick() to determine whether the ball should be launched to the left or right
     * when the game starts. */
    bool DidRightPlayerScoreLast() const;
    
    /** The sound played when the ball hits a player. */
    UPROPERTY(EditDefaultsOnly, Category=Sounds)
    USoundCue* BallHitPlayerSound;
    /** The sound played when the ball hits a wall. */
    UPROPERTY(EditDefaultsOnly, Category=Sounds)
    USoundCue* BallHitWallSound;
    /** The sound played when the ball enters the goal. */
    UPROPERTY(EditDefaultsOnly, Category=Sounds)
    USoundCue* BallHitGoalSound;
    /** The sound played when a player spins. */
    UPROPERTY(EditDefaultsOnly, Category=Sounds)
    USoundCue* PlayerSpinSound;
    
    /** The particle effect played on the ball when a goal is scored. */
    UPROPERTY(EditDefaultsOnly, Category=Particles)
    UParticleSystem* BallExplosionParticles;
    /** The particle effect played when the ball hits a wall. */
    UPROPERTY(EditDefaultsOnly, Category=Particles)
    UParticleSystem* BallHitWallParticles;
    /** The particle effect played when the ball hits a player. */
    UPROPERTY(EditDefaultsOnly, Category=Particles)
    UParticleSystem* BallHitPlayerParticles;
    /** The particle effect played when a player spins. */
    UPROPERTY(EditDefaultsOnly, Category=Particles)
    UParticleSystem* PlayerSpinParticles;
    
    /** The camera shake played when a player scores a goal. */
    UPROPERTY(EditDefaultsOnly, Category=CameraShake)
    TSubclassOf<UCameraShake> ScoreGoalCameraShake;
    /** The camera shake played when a player hits the ball. */
    UPROPERTY(EditDefaultsOnly, Category=CameraShake)
    TSubclassOf<UCameraShake> BallHitPlayerCameraShake;
    /** The camera shake played when the ball hits a wall. */
    UPROPERTY(EditDefaultsOnly, Category=CameraShake)
    TSubclassOf<UCameraShake> BallHitWallCameraShake;
    
    /** The position in which the score text is displayed. (This is the position of the score on the right-hand side) */
    static const FVector SCORE_TEXT_POSITION;
    
    /** The default score needed to win the game. */
    static const int32 DEFAULT_SCORE_TO_WIN;

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
    
    /** The score a player needs to win the game. */
    int32 ScoreToWin;
    
    /** True if the right player won last (i.e., the player starting on the right of the field scored the last goal). */
    bool bRightPlayerScoredLast = true;
};