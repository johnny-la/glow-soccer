// Fill out your copyright notice in the Description page of Project Settings.

#include "CubeProject.h"
#include "CubeProjectLevelScriptActor.h"

void ACubeProjectLevelScriptActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ACubeProjectLevelScriptActor::BeginPlay()
{
    Super::ReceiveBeginPlay();
}

void ACubeProjectLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}