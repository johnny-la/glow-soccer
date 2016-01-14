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
    
    /** Returns the PlayerStart actor where the given player should spawn. This is used to make sure that the first player
      * is always spawned at the right and the second player is always spawned to the left. */
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
    
    /** Called when the ball overlaps another actor. If a goal is overlapped, a point is given to the correct player. */
    UFUNCTION()
    void OnBallOverlap(AActor* OtherActor);
    
    /** Gives the ball a small push to start the game. Called from ACubeProjectGameState::Tick() when in PUSH_BALL state. 
      * @param bMoveRight If true, the ball is pushed to the right. Otherwise, it is pushed to the left
      */
    void PushBall(const bool bMoveRight);
    
    /** Updates the score text to reflect the current game score. Called from ACubeProjectGameState::Tick() when the game is in RESET state. */
    void UpdateScoreText();
    
    /** Called when a player scores. Resets the players and the ball at their default locations. Called from ACubeProjectGameState::Tick(). */
    void ResetField();
    /** Called when the user presses the Restart key. This is called from the ACubePawn::RestartGame() function, which is called when the user
      * pressed the "Restart" key. */
    void RestartGame();
    
    /** Starts the game. Called from ACubePawn::StartGame() when the user presses the start key in the main menu. */
    void StartGame();
    /** Called when the "Quit Main Menu" timer is complete. This timer is a small delay between the time the user presses
      * ENTER in the main menu and the time the game starts. This allows breathing room before the game starts */
    UFUNCTION()
    void OnQuitMainMenuTimerComplete();
    
    /** Sets whether or not the players can move their pawns. If true, the users can move their avatars. Called in ACubeProjectGameState.
      * When the timer is enabled and the game is waiting to start, player input is disabled. */
    void SetPlayerInputEnabled(bool bEnabled);
    
    /** Returns the score obtained by the player which starts on the left-hand side of the field. */
    int32 GetLeftPlayerScore() const;
    /** Returns the score obtained by the player which starts on the right-hand side of the field. */
    int32 GetRightPlayerScore() const;
    
    /** Returns the ball currently on the field. */
    class ABall* GetBall();
    
    /** If true, the right player won last. i.e., the player starting on the right of the field scored the last goal.
     * Used in ACubeProjectGameState::Tick() to determine whether the ball should be launched to the left or right
     * when the game starts. */
    bool DidRightPlayerScoreLast() const;
    
    /** If true, debug messages are displayed to the screen. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameSettings)
    bool bDebugMode = true;
    /** If true, the application is being deployed to a standalone build. If so,
      * text scaling needs to be adjusted in the level Blueprint due to packaging
      * glitches. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameSettings)
    bool bPackagedBuild = false;
    /** The default score needed to win the game. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameSettings)
    int32 DefaultScoreToWin = 3;
    /** The duration of the "Quit Main Menu" timer. This timer is a small delay between the time the user presses
      * ENTER in the main menu and the time the game starts. This allows breathing room before the game starts */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameSettings)
    float QuitMainMenuTimerDuration = 1;
    
    /** The position in which the score text is displayed. (This is the position of the score on the right-hand side) */
    static const FVector SCORE_TEXT_POSITION;
    
    /*****************************************************************************/
    /******************* ASSETS ASSIGNED IN GAMEMODE BLUEPRINT *******************/
    /*****************************************************************************/
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
    /** The sound played when a player wins the game. */
    UPROPERTY(EditDefaultsOnly, Category=Sounds)
    USoundCue* WinGameSound;
    
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
    /*****************************************************************************/

private:
    /** Called when the ball enters one of the goals and one of the players score. 
      * @param bRightPlayerScored true if the right-hand side player (player 2) scored. False if player 1 scored.
     */
    void OnGoal(bool bRightPlayerScored);
    
    /** The name of the map for a two-player match. This is the level loaded once the game restarts. */
    UPROPERTY(EditAnywhere, Category=GameSettings)
    FName TwoPlayerGameMapName;
    
    /** The handle which is in charge of controlling the "Quit Main Menu" timer. This timer is a small delay between
      * the time the user presses enter in the main menu and the time the game starts. This allows breathing room 
      * before the game starts */
    FTimerHandle QuitMainMenuTimerHandle;
    
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