// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Net/UnrealNetwork.h"


#include "SimpleMultiGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AGameModeSimpleMultiGame_Base;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASimpleMultiGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/* Shot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShotAction;

	/* Right Camera Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightCameraAction;

	/* Left Camera Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftCameraAction;

	/* Shot sounds */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USoundBase* ShotSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USoundBase* NoAMMOSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USoundAttenuation* SoundAttenuation;

	/* == Shot sounds == */

	/* Current Game Mode */
	AGameModeSimpleMultiGame_Base* CurrentGameMode = nullptr;

public:
	ASimpleMultiGameCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
	float LineTraceDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
	float CharacterDamage = 0.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
	float CurrentHealth = 0.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
	int32 AMMOCount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
	float ShotInRate = 0.f;

	TArray<AActor*> IngnoreActors;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);		

protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Shot */
	
	UFUNCTION(Server, Unreliable)
	void HandleShot();
	void HandleShot_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayShotSoundClient();
	void PlayShotSoundClient_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayNoAmmoSoundClient();
	void PlayNoAmmoSoundClient_Implementation();

	UFUNCTION(Client, Unreliable)
	void DrawLineTraceShot();
	void DrawLineTraceShot_Implementation();

	UFUNCTION(Server, Reliable)
	void ActorTakeDamageFromShot_Server(AActor* HitActor);
	void ActorTakeDamageFromShot_Server_Implementation(AActor* HitActor);
	/* Shot */

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected: // Protected variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Positions")
	FVector RightPositionCamera = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Positions")
	FVector LeftPositionCamera = FVector::ZeroVector;

private: // Private variables

	bool bIsCanShot = true;

private: // Private functions

	/* Character shot rate */

	void ChangeIsCanShot();

	/* == Character shot rate == */

	/* == Change Location Follow Camera == */

	/* Server calls */
	UFUNCTION(Server, Unreliable)
	void CameraRightPositionServer();
	void CameraRightPositionServer_Implementation();

	UFUNCTION(Server, Unreliable)
	void CameraLeftPositionServer();
	void CameraLeftPositionServer_Implementation();

	/* Multicast calls */
	UFUNCTION(NetMulticast, Unreliable)
	void SetRightCameraPosition();
	void SetRightCameraPosition_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void SetLeftCameraPosition();
	void SetLeftCameraPosition_Implementation();

	/* == Change Location Follow Camera == */



	/* Take Damage and enabled ragdoll */
	UFUNCTION()
	void OnTakeDamageHealth(AActor* damageActor, float damage, const UDamageType* damageType,
		AController* instigateBy, AActor* damageCauser);

	UFUNCTION(NetMulticast, Unreliable)
	void DeathAndEnabledRagdoll();
	void DeathAndEnabledRagdoll_Implementation();

	/* ==  Take Damage and enabled ragdoll == */

	/* Delete and Create UI During the game only on client */
	UFUNCTION(Client, Unreliable)
	void DeleteOwnUI();
	void DeleteOwnUI_Implementation();

	UFUNCTION(Client, Unreliable)
	void CreateOwnUI();
	void CreateOwnUI_Implementation();

	/* Delete and Create UI During the game only on client */
};

