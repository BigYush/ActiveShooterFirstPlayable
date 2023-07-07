// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CriminalAi.generated.h"

/**
 * 
 */
UCLASS()
class ACTIVESHOOTER_API ACriminalAi : public AAIController
{
	GENERATED_BODY()

	ACriminalAi();

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BTAsset;

	UFUNCTION()
	void SenseUpdated(const TArray<AActor*>& UpdatedActors);

	class UAISenseConfig_Sight* sightConfig;
	class UAISenseConfig_Hearing* hearingConfig;

	UBlackboardComponent* BlackboardComponent;
	
};
