// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include <Kismet/GameplayStatics.h>

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("MoveForward/Backwards"), this, &APlayerBase::MoveForwardBack);
	PlayerInputComponent->BindAxis(TEXT("MoveRight/Left"), this, &APlayerBase::MoveRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUp/Down"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookLeft/Right"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAction(TEXT("JumpAction"), EInputEvent::IE_Pressed, this, &APlayerBase::Jump);
	PlayerInputComponent->BindAction(TEXT("CrouchAction"), EInputEvent::IE_Pressed, this, &APlayerBase::Crouch);
	PlayerInputComponent->BindAction(TEXT("SprintAction"), EInputEvent::IE_Pressed, this, &APlayerBase::Sprint);
	PlayerInputComponent->BindAction(TEXT("SprintAction"), EInputEvent::IE_Released, this, &APlayerBase::StopSprinting);
	PlayerInputComponent->BindAction(TEXT("FPSCameraActivateAction"), EInputEvent::IE_Pressed, this, &APlayerBase::ActivateSwitchCamera);
	PlayerInputComponent->BindAction(TEXT("FPSCameraActivateAction"), EInputEvent::IE_Released, this, &APlayerBase::DisableSwitchCamera);
	PlayerInputComponent->BindAction(TEXT("SwitchFPS/TPS_Permanently"), EInputEvent::IE_Pressed, this, &APlayerBase::PermanentlySwitchCamera);

}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	MaxWalkSpeed = 300.0f;

	UBoxComponent* SpringArmCollisonComponent = nullptr;
	TArray<USceneComponent*> AllBoxComponents;
	GetComponents(AllBoxComponents);

	for (USceneComponent* Value : AllBoxComponents)
	{
		if(Value->GetName() == "SpringArmCollison")
		{
			SpringArmCollisonComponent = Cast<UBoxComponent>(Value);
		}
	}

	if(IsValid(SpringArmCollisonComponent))
	{
		SpringArmCollisonComponent-> OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnBoxBeginOverlap);
		UE_LOG(LogTemp, Warning, TEXT("Spring Arm Collision is valid"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spring Arm Collision is null"));
	}
}

void APlayerBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UActorComponent* CameraComponentActor = GetComponentByClass(USpringArmComponent::StaticClass()); //UActorComponent Parent Type of CameraComponent
	//USpringArmComponent* CameraComponent = Cast<USpringArmComponent>(CameraComponentActor);

	
	//AddActorLocalRotation(FRotator(0.f, 90.f, 0.f));

	//Think about another way to solve this problem
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerBase::MoveForwardBack(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void APlayerBase::MoveRightLeft(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void APlayerBase::Crouch()
{
	if (IsValid(GetCharacterMovement()))
	{
		UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
		if (CharacterMovementComponent->IsCrouching())
		{
			ACharacter::UnCrouch();
		}
		else
		{
			ACharacter::Crouch();
		}
	}
}

void APlayerBase::Jump()
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	
	if (IsValid(CharacterMovementComponent))
	{
		if(CharacterMovementComponent ->IsCrouching())
		{
			FTimerHandle TimerHandle;
			
			ACharacter::UnCrouch();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACharacter::Jump, 0.1f);
		}
		else
		{
			ACharacter::Jump();
		}
	}
}

bool APlayerBase::GetIsPlayerActingDead() const
{
	return PlayerActingDead;
}

void APlayerBase::ActDead()
{
	PlayerActingDead = !PlayerActingDead;
}

bool APlayerBase::GetIsPlayerCrawling() const
{
	return bIsCrawl;
}

void APlayerBase::Crawl()
{
	bIsCrawl = !bIsCrawl;
	//Change Player Half Height
}

void APlayerBase::ActivateSwitchCamera()
{
	SetCameraPositionTemporary(true);
}

void APlayerBase::DisableSwitchCamera()
{
	SetCameraPositionTemporary(false);
}

void APlayerBase::Sprint()
{
	const UCharacterMovementComponent* CharacterMovementComponent  = GetCharacterMovement();
	
	if (IsValid(CharacterMovementComponent))
	{
		if (GetCharacterMovement()->IsCrouching())
		{
			ACharacter::UnCrouch();
		}
	}

	MaxWalkSpeed = 600.0f;
}

void APlayerBase::StopSprinting()
{
	MaxWalkSpeed = 300.0f;
}

void APlayerBase::SetCameraPositionTemporary(bool FPS_Mode)
{
	UActorComponent* SpringArmComponentActor= AActor::GetComponentByClass(USpringArmComponent::StaticClass()); // UActorComponent Base Class of the Spring Arm
	USpringArmComponent* SpringArmComponent= Cast<USpringArmComponent>(SpringArmComponentActor);

	UActorComponent* CameraComponentActor= AActor::GetComponentByClass(UCameraComponent::StaticClass()); // UActorComponent Base Class of the Camera
	UCameraComponent* CameraComponent= Cast<UCameraComponent>(CameraComponentActor);
	
	if(IsValid(SpringArmComponent))
	{
		if(IsValid(CameraComponent))
		{
			if (!FPS_Default) {
				if (FPS_Mode)
				{
					SpringArmComponent-> TargetArmLength = -40.f;
					SpringArmComponent-> TargetOffset = FVector(0.f, 0.f, 60.f);
					CameraComponent-> bUsePawnControlRotation = true;
				}
				
				else
				{
					SpringArmComponent-> TargetArmLength = 300.f;
					SpringArmComponent-> TargetOffset = FVector(0.f, 70.f, 80.f);
					CameraComponent-> bUsePawnControlRotation = false;
					CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
				}
			}

			else
			{
				if (FPS_Mode)
				{
					SpringArmComponent-> TargetArmLength = 300.f;
					SpringArmComponent-> TargetOffset = FVector(0.f, 70.f, 80.f);
					CameraComponent-> bUsePawnControlRotation = true;
				}
				else
				{
					SpringArmComponent-> TargetArmLength = -40.f;
					SpringArmComponent-> TargetOffset = FVector(0.f, 0.f, 60.f);
					CameraComponent-> bUsePawnControlRotation = false;
					CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
				}
				
			}
		}
	}
}

void APlayerBase::PermanentlySwitchCamera()
{
	FPS_Default= !FPS_Default;
	
	UActorComponent* SpringArmComponentActor= AActor::GetComponentByClass(USpringArmComponent::StaticClass()); // UActorComponent Type of the Spring Arm
	USpringArmComponent* SpringArmComponent= Cast<USpringArmComponent>(SpringArmComponentActor);

	UActorComponent* CameraComponentActor= AActor::GetComponentByClass(UCameraComponent::StaticClass()); // UActorComponent Type of the Spring Arm
	UCameraComponent* CameraComponent= Cast<UCameraComponent>(CameraComponentActor);
	
	if(IsValid(SpringArmComponent))
	{
		if(IsValid(CameraComponent))
		{
			if(FPS_Default)
			{
				SpringArmComponent-> TargetArmLength = -40.f;
				SpringArmComponent-> TargetOffset = FVector(0.f, 0.f, 60.f);
				CameraComponent-> bUsePawnControlRotation = false;
				CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
			}

			else
			{
				SpringArmComponent-> TargetArmLength = 300.f;
				SpringArmComponent-> TargetOffset = FVector(0.f, 70.f, 80.f);
				CameraComponent-> bUsePawnControlRotation = false;
				CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
			}
		}
	}
}

bool APlayerBase::GetIsPlayerDead() const
{
	return PlayerDead;
}

void APlayerBase::SetIsPlayerDead(bool isDead)
{
	PlayerDead = isDead;
}

AActor* APlayerBase::GetPlayerKiller() const
{
	return PlayerKiller;
}
