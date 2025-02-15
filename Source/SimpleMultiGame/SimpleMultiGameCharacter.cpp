// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleMultiGameCharacter.h"
#include "Engine/LocalPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameModes/GameModeSimpleMultiGame_Base.h"
#include "HUDs/HUDSimpleMultiGame_Base.h"
#include "PlayerControllers/PlayerControllerSMG_Base.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASimpleMultiGameCharacter::ASimpleMultiGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Add Tag
	Tags.Add(FName(TEXT("Player")));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASimpleMultiGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASimpleMultiGameCharacter, CurrentHealth);
	DOREPLIFETIME(ASimpleMultiGameCharacter, AMMOCount);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASimpleMultiGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetRightCameraPosition();

	OnTakeAnyDamage.AddDynamic(this, &ASimpleMultiGameCharacter::OnTakeDamageHealth);

	CurrentGameMode = Cast<AGameModeSimpleMultiGame_Base>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ASimpleMultiGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FTimerHandle TimerToCreateWidget;
	GetWorldTimerManager().SetTimer(TimerToCreateWidget, this, &ASimpleMultiGameCharacter::CreateOwnUI, 1.f, false);
}

void ASimpleMultiGameCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASimpleMultiGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASimpleMultiGameCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASimpleMultiGameCharacter::Look);

		// Shot
		EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Started, this, &ASimpleMultiGameCharacter::HandleShot);

		// Right Camera
		EnhancedInputComponent->BindAction(RightCameraAction, ETriggerEvent::Started, this, &ASimpleMultiGameCharacter::CameraRightPositionServer);

		// Left Camera
		EnhancedInputComponent->BindAction(LeftCameraAction, ETriggerEvent::Started, this, &ASimpleMultiGameCharacter::CameraLeftPositionServer);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASimpleMultiGameCharacter::HandleShot_Implementation()
{
	if (bIsCanShot)
	{
		if (AMMOCount > 0)
		{
			--AMMOCount;
			DrawLineTraceShot();
			PlayShotSoundClient();
			bIsCanShot = false;

			FTimerHandle TimerToCoolDown;
			GetWorldTimerManager().SetTimer(TimerToCoolDown, this, &ASimpleMultiGameCharacter::ChangeIsCanShot, ShotInRate, false);
		}
		else 
			PlayNoAmmoSoundClient();
	}
}

void ASimpleMultiGameCharacter::PlayShotSoundClient_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
}

void ASimpleMultiGameCharacter::PlayNoAmmoSoundClient_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoAMMOSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
}

void ASimpleMultiGameCharacter::DrawLineTraceShot_Implementation()
{
	FHitResult HitResult;
	auto TraceLength = GetFollowCamera()->GetComponentLocation() + (GetFollowCamera()->GetForwardVector() * LineTraceDistance);

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetFollowCamera()->GetComponentLocation(), TraceLength, ETraceTypeQuery::TraceTypeQuery1,
		false, IngnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);
	
	if (HitResult.GetActor() != nullptr && HitResult.GetActor()->ActorHasTag(FName(TEXT("Player"))))
	{
		ActorTakeDamageFromShot_Server(HitResult.GetActor());
	}
}

void ASimpleMultiGameCharacter::ActorTakeDamageFromShot_Server_Implementation(AActor* HitActor)
{
	UGameplayStatics::ApplyDamage(HitActor, CharacterDamage, GetInstigatorController(), this, UDamageType::StaticClass());
}

void ASimpleMultiGameCharacter::ChangeIsCanShot()
{ bIsCanShot = true; }

void ASimpleMultiGameCharacter::CameraRightPositionServer_Implementation()
{
	SetRightCameraPosition();
}

void ASimpleMultiGameCharacter::CameraLeftPositionServer_Implementation()
{
	SetLeftCameraPosition();
}

void ASimpleMultiGameCharacter::SetRightCameraPosition_Implementation()
{
	FollowCamera->SetRelativeLocation(RightPositionCamera);
}

void ASimpleMultiGameCharacter::SetLeftCameraPosition_Implementation()
{
	FollowCamera->SetRelativeLocation(LeftPositionCamera);
}

void ASimpleMultiGameCharacter::OnTakeDamageHealth(AActor* damageActor, float damage, const UDamageType* damageType, AController* instigateBy, AActor* damageCauser)
{
	CurrentHealth -= damage;

	if (CurrentHealth <= 0.f)
	{
		DeathAndEnabledRagdoll();

		DeleteOwnUI();

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(CurrentGameMode, &AGameModeSimpleMultiGame_Base::RespawnCharacter, GetInstigatorController());

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.f, false);
	}
}

void ASimpleMultiGameCharacter::DeathAndEnabledRagdoll_Implementation()
{
	GetCapsuleComponent()->DestroyComponent();
	GetMesh()->SetCollisionProfileName(FName(TEXT("BlockAll")));
	GetMesh()->SetSimulatePhysics(true);
	bIsCanShot = false;
}

void ASimpleMultiGameCharacter::DeleteOwnUI_Implementation()
{
	auto CurrentPlayerController = Cast<APlayerControllerSMG_Base>(GetController());
	auto CurrentHUD = Cast<AHUDSimpleMultiGame_Base>(CurrentPlayerController->GetHUD());

	CurrentHUD->DeleteUIDuringTheGame();
}

void ASimpleMultiGameCharacter::CreateOwnUI_Implementation()
{
	auto CurrentPlayerController = Cast<APlayerControllerSMG_Base>(GetController());
	auto CurrentHUD = Cast<AHUDSimpleMultiGame_Base>(CurrentPlayerController->GetHUD());

	CurrentHUD->DrawUIDuringTheGame();
}

void ASimpleMultiGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASimpleMultiGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
