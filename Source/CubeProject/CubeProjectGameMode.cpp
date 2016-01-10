// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubeProjectGameMode.h"
#include "CubePawn.h"
#include "Ball.h"
#include "Goal.h"
#include "Engine/TextRenderActor.h"
#include "CubeProjectGameState.h"
#include "CubeProjectLevelScriptActor.h"

/** The position in which the score text is displayed. (This is the position of the score on the right-hand side) */
const FVector ACubeProjectGameMode::SCORE_TEXT_POSITION = FVector(0.0f,100.0f,252.0f);

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

    // Spawn a spectator pawn initially. The actual player pawns are spawned manually in BeginPlay()
    //DefaultPawnClass = SpectatorClass;
}

// Called when the game mode starts
void ACubeProjectGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // If debug mode is off, do not display debug messages.
    if(GEngine)
    {
        GEngine->bEnableOnScreenDebugMessages = bDebugMode;
    }
    
    // Sets the player scores to zero
    LeftPlayerScore = RightPlayerScore = 0;
    
    // Set the score to win to default
    ScoreToWin = DefaultScoreToWin;
    
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
    


    // Retrieve the player which was spawned automatically by the game
    APlayerController* LeftPlayerController = UGameplayStatics::GetPlayerController(World, 0);
    
    // Spawn the second player using the correct Blueprint class
    DefaultPawnClass = Player2PawnClass;
    // Create the second player in the game
    APlayerController* RightPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), 1, true);//UGameplayStatics::GetPlayerController(World, 1);
    
    // Enable the mouse cursor for a better user experience
    LeftPlayerController->bShowMouseCursor = true;
    LeftPlayerController->bEnableClickEvents = true;
    LeftPlayerController->bEnableMouseOverEvents = true;
    
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

}

AActor* ACubeProjectGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // Get the UWorld instance controlling the game
    UWorld* World = GetWorld();
    
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,"CHOOSE SPAWN POINT");
    
    // Iterate through each player start in the game and choose an appropriate one for the given player
    for(TActorIterator<APlayerStart> PlayerStartIterator(World); PlayerStartIterator; ++PlayerStartIterator)
    {
        // Spawn the first player (player 0) at the player start with tag "0"
        if(Player == UGameplayStatics::GetPlayerController(World,0) && PlayerStartIterator->PlayerStartTag == "0")
        {
            if(GEngine)
                GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,"Spawn player 0");
            
            return *PlayerStartIterator;
        }
        // Spawn the second player at the player start with tag "1" (set in the details panel of the player start actor)
        else if(Player == UGameplayStatics::GetPlayerController(World,1) && PlayerStartIterator->PlayerStartTag == "1")
        {
            if(GEngine)
                GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,"Spawn player 1");
            
            return *PlayerStartIterator;
        }
    }
    
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,"Choose default spawn");
    
    // If no player start has been chosen, let Unreal choose it by default
    return Super::ChoosePlayerStart(Player);
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
    
    // Retrieve the UWorld instance controlling the game
    UWorld* World = GetWorld();
    // Retrieve the object controlling the game's state
    ACubeProjectGameState* GameState = GetGameState<ACubeProjectGameState>();

    // Retrieve the GameState instance controlling the game's state.
    ACubeProjectGameState* CurrentGameState = GetGameState<ACubeProjectGameState>();
    
    if(CurrentGameState)
    {
        // If either player reached the score needed to win
        if(LeftPlayerScore >= ScoreToWin || RightPlayerScore >= ScoreToWin)
        {
            // Inform the GameState instance that the game is over.
            GameState->SetState(EGameState::GAME_OVER);
            
            // Play the game-winning sound
            if(WinGameSound)
            {
                UGameplayStatics::PlaySoundAtLocation(World,WinGameSound,FVector::ZeroVector);
            }
        }
        // Else, if the game still isn't over, reset the ball and the players to their start positions.
        else
        {
            // Change the game state to RESET. This resets the ball and the players at their starting points.
            GameState->SetState(EGameState::RESET);
        }
    }
    
    // Play the sound of the ball hitting a goal
    if(BallHitGoalSound)
    {
        UGameplayStatics::PlaySoundAtLocation(World,BallHitGoalSound,Ball->GetActorLocation());
    }
    
    // Play an explosion particle effect on the ball since a goal was just scored
    if(BallExplosionParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(World,BallExplosionParticles,Ball->GetActorLocation());
    }
    
    // Play a camera shake when the user scores a goal
    if(ScoreGoalCameraShake)
    {
        World->GetFirstPlayerController()->ClientPlayCameraShake(ScoreGoalCameraShake, 1.0f, ECameraAnimPlaySpace::World,
                                                                                FRotator::ZeroRotator);
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

/** Called from ACubePawn::StartGame() when the user presses the ENTER key in the main menu. */
void ACubeProjectGameMode::StartGame()
{
    // Retrieve the object controlling the game's state
    ACubeProjectGameState* GameState = GetGameState<ACubeProjectGameState>();
    
    // Only start the game if the user is in the main menu
    if(GameState->GetState() == EGameState::MAIN_MENU)
    {
        // Get the level Blueprint controlling the game
        ACubeProjectLevelScriptActor* LevelScript = Cast<ACubeProjectLevelScriptActor>(GetWorld()->GetLevelScriptActor());
    
        if(GEngine)
            GEngine->AddOnScreenDebugMessage(-1,4.0f,FColor::White,"HIDE MAIN MENU");
        
        // Tell the level Blueprint to hide the main menu to start the game.
        LevelScript->HideMainMenu();
        
        // Start a timer which will call OnQuitMainMenuTimerComplete() once complete.
        // Once this method is called, game state is switched to "RESET" and the game starts
        GetWorld()->GetTimerManager().SetTimer(QuitMainMenuTimerHandle,this,&ACubeProjectGameMode::OnQuitMainMenuTimerComplete,QuitMainMenuTimerDuration,false);
    }
}

void ACubeProjectGameMode::OnQuitMainMenuTimerComplete()
{
    // Retrieve the object controlling the game's state
    ACubeProjectGameState* GameState = GetGameState<ACubeProjectGameState>();
    // Tell the GameState instance to reset the field and start the game
    GameState->SetState(EGameState::RESET);
    
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,10,FColor::Blue,"START GAME");
}

void ACubeProjectGameMode::RestartGame()
{
    // Reload the game level
    //UGameplayStatics::OpenLevel(GetWorld(),TwoPlayerGameMapName);
    
    // Retrieve the object controlling the game's state
    ACubeProjectGameState* GameState = GetGameState<ACubeProjectGameState>();
    // Tell the GameState instance to reset the field
    GameState->SetState(EGameState::RESET);
    
    // Reset the field and the score.
    LeftPlayerScore = RightPlayerScore = 0;
    
    // Obtain the level Blueprint used to play Matinee animations
    ACubeProjectLevelScriptActor* LevelScript = Cast<ACubeProjectLevelScriptActor>(GetWorld()->GetLevelScriptActor());
    if(LevelScript)
    {
        // Hide and reset the animations in the game
        LevelScript->RestartGame();
    }
    
}

void ACubeProjectGameMode::UpdateScoreText()
{
    // Update the score displayed on screen using the TextRenderActors displaying the game score
    ScoreTextLeft->GetTextRender()->SetText(FText::AsNumber(LeftPlayerScore));
    ScoreTextRight->GetTextRender()->SetText(FText::AsNumber(RightPlayerScore));

}

void ACubeProjectGameMode::SetPlayerInputEnabled(bool bEnabled)
{
    // Get the world instance controlling the game
    UWorld* World = GetWorld();
    
    // If player input should be enabled
    if(bEnabled)
    {
        // Enable player input
        UGameplayStatics::GetPlayerController(World,0)->ResetIgnoreMoveInput();
        UGameplayStatics::GetPlayerController(World,1)->ResetIgnoreMoveInput();
    }
    // Else, if player input should be disabled
    else
    {
        // Disable player input
        UGameplayStatics::GetPlayerController(World,0)->SetIgnoreMoveInput(true);
        UGameplayStatics::GetPlayerController(World,1)->SetIgnoreMoveInput(true);
    }
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