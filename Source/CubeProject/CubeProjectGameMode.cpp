// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubeProjectGameMode.h"
#include "CubePawn.h"
#include "Ball.h"
#include "Goal.h"
#include "Engine/TextRenderActor.h"
#include "CubeProjectGameState.h"

/** The position in which the score text is displayed. (This is the position of the score on the right-hand side) */
const FVector ACubeProjectGameMode::SCORE_TEXT_POSITION = FVector(0.0f,100.0f,252.0f);

/** The default score needed for a player to win the game. */
const int32 ACubeProjectGameMode::DEFAULT_SCORE_TO_WIN = 1;

// Initializes the default properties for the game mode
ACubeProjectGameMode::ACubeProjectGameMode()
{
    // Sets the Blueprint pawn that will be spawned for player 1
    static ConstructorHelpers::FClassFinder<ACubePawn> Player1PawnBPClass(TEXT("/Game/Blueprints/BP_CubePawn"));
    if(Player1PawnBPClass.Class != NULL)
    {
        Player1PawnClass = Player1PawnBPClass.Class;
    }
    
    // Sets the Blueprint pawn which the second player controls.
    static ConstructorHelpers::FClassFinder<ACubePawn> Player2PawnBPClass(TEXT("/Game/Blueprints/BP_CubePawn_P2"));
    if(Player2PawnBPClass.Class != NULL)
    {
        Player2PawnClass = Player2PawnBPClass.Class;
    }
    
    // Populate the 'BallClass' with the ball's Blueprint. The GameMode will then be able to spawn the ball.
    static ConstructorHelpers::FClassFinder<ABall> BallBPClass(TEXT("/Game/Blueprints/BP_Ball_Large"));
    if(BallBPClass.Class != NULL)
    {
        BallClass = BallBPClass.Class;
    }
    
    // Find the 'ScoreText' Blueprint used to display the score at the top of the screen
    static ConstructorHelpers::FClassFinder<ATextRenderActor> ScoreTextBPClass(TEXT("/Game/Blueprints/BP_Score_text"));
    if(ScoreTextBPClass.Class != NULL)
    {
        ScoreTextClass = ScoreTextBPClass.Class;
    }
    
    // Sets the default Blueprint pawn that will be spawned once the game starts
    DefaultPawnClass = Player1PawnClass;
    // Set the default class used to control game state
    GameStateClass = ACubeProjectGameState::StaticClass();
    
    // Set the score to win to default
    ScoreToWin = DEFAULT_SCORE_TO_WIN;

    // Spawn a spectator pawn initially. The actual player pawns are spawned manually in BeginPlay()
    //DefaultPawnClass = SpectatorClass;
}

// Called when the game mode starts
void ACubeProjectGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Sets the player scores to zero
    LeftPlayerScore = RightPlayerScore = 0;
    
    // Retrieve the UWorld instance
    UWorld* World = GetWorld();
    
    // If BallClass points to a valid Blueprint, spawn this Blueprint
    if(BallClass)
    {
        // Spawn the ball in the world.
        Ball = World->SpawnActor<ABall>(BallClass);
        
        // Call the OnBallOverlap() function when the ball overlaps another actor. Used to determine when the ball hits a goal
        Ball->OnActorBeginOverlap.AddDynamic(this, &ACubeProjectGameMode::OnBallOverlap);
    }
    
    // If the ScoreTextClass points to a valid Blueprint, spawn the score indicators
    if(ScoreTextClass)
    {
        // Spawn the text actors which display the game score
        ScoreTextLeft = World->SpawnActor<ATextRenderActor>(ScoreTextClass);
        ScoreTextRight = World->SpawnActor<ATextRenderActor>(ScoreTextClass);
        
        // Place the score texts at the top of the screen.
        ScoreTextLeft->SetActorLocation(SCORE_TEXT_POSITION * FVector(0,-1,1));
        ScoreTextRight->SetActorLocation(SCORE_TEXT_POSITION);
        
        // Rotate the text so that it faces the camera
        ScoreTextLeft->SetActorRotation(FRotator(0,-180,0));
        ScoreTextRight->SetActorRotation(FRotator(0,-180,0));
    }

    // Retrieve the player which was spawned automatically by the game
    APlayerController* LeftPlayerController = UGameplayStatics::GetPlayerController(World, 0);
    
    // Spawn the second player using the correct Blueprint class
    DefaultPawnClass = Player2PawnClass;
    // Create the second player in the game
    APlayerController* RightPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), 1, true);//UGameplayStatics::GetPlayerController(World, 1);
    
    // Store the two player pawns
    ACubePawn* LeftPlayerPawn = NULL;
    ACubePawn* RightPlayerPawn = NULL;
    
    // Retrieve the pawns for the left and right players
    if(LeftPlayerController)
        LeftPlayerPawn = Cast<ACubePawn>(LeftPlayerController->GetPawn());
    if(RightPlayerController)
        RightPlayerPawn = Cast<ACubePawn>(RightPlayerController->GetPawn());
    
    // Set the left player to control the right player. Since only one pawn can receive keyboard input, the left pawn
    // calls the right pawn's methods when player 2's keys are pressed.
    if(LeftPlayerPawn)
    {
        if(GEngine)
            GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Yellow,"Register P2 with P1");
        
        LeftPlayerPawn->SetPawn_P2(RightPlayerPawn);
        
        LeftPlayerPawn->Rename(TEXT("Player 1 Pawn"));
    }
    
    if(RightPlayerPawn)
    {
        // Rename the right player's name for convenience
        RightPlayerPawn->Rename(TEXT("Player 2 Pawn"));
    }
    
    // Update the pawns controlled by the first and second players
    Player1Pawn = LeftPlayerPawn;
    Player2Pawn = RightPlayerPawn;
}

void ACubeProjectGameMode::OnBallOverlap(AActor* OtherActor)
{
    // If the actor which overlapped the ball is non-null, check if the ball hit a goal.
    if(OtherActor)
    {
        // If the ball hit a goal
        if(OtherActor->IsA(AGoal::StaticClass()))
        {
            // Cast the actor into a Goal
            AGoal* Goal = Cast<AGoal>(OtherActor);
            
            // Store true if the player on the right-hand side scored (player 2)
            bRightPlayerScoredLast = false;
            
            // If the goal does not belong to the right-hand side of the field, the ball ended up in the left player's goal. Thus, the right player scored.
            if(!Goal->IsRightHandSideGoal())
            {
                // The ball just enterred the left-hand side goal. Thus, the right player scored
                bRightPlayerScoredLast = true;
            }
            
            // Inform the game that one of the players scored. This will update the HUD and reset the match.
            OnGoal(bRightPlayerScoredLast);
        }
    }
}

void ACubeProjectGameMode::OnGoal(bool bRightPlayerScored)
{
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Yellow,"GOAL");

    // If the right player scored
    if(bRightPlayerScored)
    {
        // Increment the right player's score
        RightPlayerScore++;
    }
    // Else, if the left-hand side player scored
    else
    {
        // Increment the left player's score
        LeftPlayerScore++;
    }
    
    // Retrieve the object controlling the game's state
    ACubeProjectGameState* GameState = GetGameState<ACubeProjectGameState>();

    // Update the score displayed on screen using the TextRenderActors displaying the game score
    ScoreTextLeft->GetTextRender()->SetText(FText::AsNumber(LeftPlayerScore));
    ScoreTextRight->GetTextRender()->SetText(FText::AsNumber(RightPlayerScore));
    
    // Retrieve the GameState instance controlling the game's state.
    ACubeProjectGameState* CurrentGameState = GetGameState<ACubeProjectGameState>();
    
    
    if(CurrentGameState)
    {
        // If either player reached the score needed to win
        if(LeftPlayerScore >= ScoreToWin || RightPlayerScore >= ScoreToWin)
        {
            // Inform the GameState instance that the game is over.
            GameState->SetState(EGameState::GAME_OVER);
        }
        // Else, if the game still isn't over, reset the ball and the players to their start positions.
        else
        {
            // Change the game state to RESET. This resets the ball and the players at their starting points.
            GameState->SetState(EGameState::RESET);
        }
    }
        
}

void ACubeProjectGameMode::PushBall(const bool bMoveRight)
{
    // Give the ball an initial push to get the game started.
    Ball->StartMove(bMoveRight);
}

void ACubeProjectGameMode::ResetField()
{
    // Reset each pawn and actor on the field to their default locations
    Player1Pawn->Reset();
    Player2Pawn->Reset();
    Ball->Reset();
}

void ACubeProjectGameMode::SetPlayerInputEnabled(bool bEnabled)
{
    // Get the world instance controlling the game
    UWorld* World = GetWorld();
    
    // Set whether or not the players are moveable.
    UGameplayStatics::GetPlayerController(World,0)->SetIgnoreMoveInput(!bEnabled);
    UGameplayStatics::GetPlayerController(World,1)->SetIgnoreMoveInput(!bEnabled);
}

ABall* ACubeProjectGameMode::GetBall()
{
    // Return the ball currently on the field.
    return Ball;
}

/** Returns the score kept by the left-hand side player. */
int32 ACubeProjectGameMode::GetLeftPlayerScore() const
{
    return LeftPlayerScore;
}

/** Returns the score obtained by the player which starts on the right-hand side of the game board. */
int32 ACubeProjectGameMode::GetRightPlayerScore() const
{
    return RightPlayerScore;
}

bool ACubeProjectGameMode::DidRightPlayerScoreLast() const
{
    return bRightPlayerScoredLast;
}