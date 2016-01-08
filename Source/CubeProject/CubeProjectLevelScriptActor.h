// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "CubeProjectLevelScriptActor.generated.h"

/**
 * Custom level Blueprint for the game
 */
UCLASS()
class CUBEPROJECT_API ACubeProjectLevelScriptActor : public ALevelScriptActor
{
    GENERATED_BODY()
    
public:
    // Called every frame
    virtual void Tick(float DeltaSeconds) override;
    
    /** Called when a player scores and the game restarts. This method displays the "READY, GO!!" message on screen.
     * Note: This method is called from ACubeProjectGameMode::Tick() when the game is in "RESET" state. */
    UFUNCTION(BlueprintImplementableEvent)
    void ShowGameStartTimer();
    
    /** Show the winning message for the appropriate player that won the game. */
    UFUNCTION(BlueprintImplementableEvent)
    void ShowWinMessage(const bool bYellowPlayerWins);
    
    /** Called when the game is restarted. Resets and hides all the game's animations. */
    UFUNCTION(BlueprintImplementableEvent)
    void RestartGame();
    
protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    // Called when the game ends
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
