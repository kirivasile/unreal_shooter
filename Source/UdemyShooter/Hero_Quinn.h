// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/StaticMeshComponent.h"
#include "Hero_Quinn.generated.h"


UCLASS()
class UDEMYSHOOTER_API AHero_Quinn : public ACharacter
{
	GENERATED_BODY()

	// Setup Enhanced Input System
	void SetupEnhancedInputActions(UInputComponent* PlayerInputComponent);

	// Handle move input
	void Move(const FInputActionValue& Value);

	// Handle look input
	void Look(const FInputActionValue& Value);

	// Handle ADS input
	void ADSStarted(const FInputActionValue& Value);
	void ADSCompleted(const FInputActionValue& Value);

	// Handle shoot input
	void Shoot(const FInputActionValue& value);

	void DrawShootLine(float drawTime);

	UFUNCTION(Server, Reliable)
	void Server_SetIsADS(bool value);

	UFUNCTION()
	void OnRep_IsADSCpp();
public:
	// Sets default values for this character's properties
	AHero_Quinn();

protected:
#pragma region Enhanced_Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	// TODO: Move to a separate struct
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class  UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* InputIsADS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* InputShoot;

	UPROPERTY(BlueprintReadOnly, Category = "Enhanced Input", ReplicatedUsing=OnRep_IsADSCpp)
	bool _isADSCpp;

#pragma endregion Enhanced_Input

#pragma region Weapon_Setup
	// TODO: Find the fix for the UPROPERTY USkeletalMeshComponent
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup")
	FName _pistolMeshName;

	USkeletalMeshComponent* _pistolMesh; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup")
	USkeletalMeshComponent* _tempMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup")
	UParticleSystem* _pistolFlash; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup")
	FName _pistolMuzzleBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup")
	USoundBase* _weaponSound;

	
#pragma endregion Weapon_Setup

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetIsAdsCpp();
};
