// Fill out your copyright notice in the Description page of Project Settings.


#include "CriminalAi.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ACriminalAi::ACriminalAi()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	hearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	
	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->ConfigureSense(*hearingConfig);
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());

	sightConfig->SightRadius = 3000.f;
	sightConfig->LoseSightRadius =  3050.f;
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->PeripheralVisionAngleDegrees = 90.f;
	sightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
	sightConfig->PointOfViewBackwardOffset = 0.f;
	sightConfig->NearClippingRadius = 0.f;
	sightConfig->SetMaxAge(10.f);

	hearingConfig->HearingRange = 500.f;
	hearingConfig->SetMaxAge(5.f);

	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ACriminalAi::SenseUpdated);
}

void ACriminalAi::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunBehaviorTree(BTAsset);
	BlackboardComponent = GetBlackboardComponent();
	
}

void ACriminalAi::SenseUpdated(const TArray<AActor*>& UpdatedActors)
{
	for(AActor* Value : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		PerceptionComponent -> GetActorsPerception(Value, Info);
		
		if(IsValid(BlackboardComponent))
		{
			const FAIStimulus SightStimulus = Info.LastSensedStimuli[0];
			if (SightStimulus.WasSuccessfullySensed())
			{
				BlackboardComponent-> SetValueAsBool("CanSeePlayer", true);
			}
			else
			{
				BlackboardComponent-> SetValueAsBool("CanSeePlayer", false);
			}

			const FAIStimulus HearingStimulus = Info.LastSensedStimuli[1];
			if (HearingStimulus.WasSuccessfullySensed())
			{
				
				BlackboardComponent-> SetValueAsBool("ShouldInvesigate", true);
				BlackboardComponent-> SetValueAsBool("InvestigateLocation", true);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Blackboard Component is null"));
		}
	}
}
