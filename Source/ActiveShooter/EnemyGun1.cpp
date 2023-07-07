// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGun1.h"
#include "PlayerEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PlayerBase.h"
#include "Components/SceneComponent.h"

AEnemyGun1::AEnemyGun1()
{
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//Mesh->SetupAttachment(GetRootComponent());

	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShootPoint"));
	//Mesh->SetupAttachment(Mesh);
}

void AEnemyGun1::PullTrigger()
{

	UE_LOG(LogTemp, Warning, TEXT("PullTriggerCalled"));

	USceneComponent* ShootPoint = nullptr;
	TArray<USceneComponent*> ShotPointTagComponents;
	GetComponents(ShotPointTagComponents);
	
	for (USceneComponent* Value : ShotPointTagComponents)
	{
		if(Value->GetName() == "ShootPoint")
		{
			ShootPoint = Value;
		}
		UE_LOG(LogTemp, Warning, TEXT("Component Name %s"), *Value->GetName());
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Number of scene components for gun %d"), ShotPointTagComponents.Num());
	
	if(IsValid(ShootPoint))
	{
		FHitResult Hit;
		FVector ShootLocation = ShootPoint->GetComponentLocation();
		FRotator ShootRotation = ShootPoint->GetComponentRotation();
		FVector EndPoint = ShootLocation + ShootRotation.Vector() * MaxRange;

		DrawDebugLine(GetWorld(), ShootLocation, EndPoint, FColor::Blue, false, 5.f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
	
		if (GetWorld()->LineTraceSingleByChannel(Hit, ShootLocation, EndPoint, ECC_Visibility, Params))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, ShootLocation, ShootRotation);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MuzzleSound, ShootLocation);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShootRotation);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		
			AActor* HitActor = Hit.GetActor();
			AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
			if(IsValid(HitActor))
			{
				if(IsValid(Player))
				{
					UE_LOG(LogTemp, Warning, TEXT("ShootingWorks"));
					//Make sure to change the type of Hit.GetActor() to a pawn to equate otherwise the return may always return false.
					//They are pointers tho still so idk if they are going to work
					if (Player == HitActor){
						if(APlayerEnemyBase* PlayerEnemyBase = Cast<APlayerEnemyBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
						{
							if(APlayerBase* PlayerBase = Cast<APlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
							{
								AActor* ParentActor = GetParentActor();
								if(IsValid(ParentActor))
								{
									PlayerBase->SetIsPlayerDead(true);
									PlayerEnemyBase->PlayerKiller = ParentActor;
									UE_LOG(LogTemp, Warning, TEXT("OwnerName: %s"), *ParentActor->GetName());
									UE_LOG(LogTemp, Warning, TEXT("PlayerDetected"));
								}
								else{UE_LOG(LogTemp, Warning, TEXT("OwnerNotValid"));}
							}
						}
						else{UE_LOG(LogTemp, Warning, TEXT("PlayerPawnToPlayerEnemyBaseNotValid"));}
					}
					else {UE_LOG(LogTemp, Warning, TEXT("Did not hit player"));}
				
					UE_LOG(LogTemp, Warning, TEXT("PlayerNameFromHitActor: %s"), *Hit.GetActor()->GetName());
					UE_LOG(LogTemp, Warning, TEXT("PlayerNameFromVariable: %s"), *Player->GetName());
				}
				else {UE_LOG(LogTemp, Warning, TEXT("PlayerCast not valid"));}
			}
			else {UE_LOG(LogTemp, Warning, TEXT("HitActor not valid"));}
		}
	}
	else {UE_LOG(LogTemp, Warning, TEXT("ShootPoint not valid"));}
}