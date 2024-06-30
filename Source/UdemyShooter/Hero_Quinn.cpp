// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero_Quinn.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// Sets default values
AHero_Quinn::AHero_Quinn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AHero_Quinn::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> components;
	GetRootComponent()->GetChildrenComponents(true, components);

	for (int i = 0; i < components.Num(); ++i) {
		auto t = components[i];
		if (components[i]->GetFName() == _pistolMeshName) _pistolMesh = Cast<USkeletalMeshComponent>(components[i]);
		// UE_LOG(LogTemp, Log, TEXT("Component name is %s"), *components[i]->GetFName().ToString());
	}

}

// Called every frame
void AHero_Quinn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHero_Quinn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	SetupEnhancedInputActions(PlayerInputComponent);
}

void AHero_Quinn::SetupEnhancedInputActions(UInputComponent* PlayerInputComponent) {
	auto inputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	inputComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &AHero_Quinn::Move);
	inputComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &AHero_Quinn::Look);
	inputComponent->BindAction(InputIsADS, ETriggerEvent::Started, this, &AHero_Quinn::ADSStarted);
	inputComponent->BindAction(InputIsADS, ETriggerEvent::Completed, this, &AHero_Quinn::ADSCompleted);
	inputComponent->BindAction(InputShoot, ETriggerEvent::Started, this, &AHero_Quinn::Shoot);

	auto pc = Cast<APlayerController>(GetController());
	auto inputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

	inputSystem->ClearAllMappings();
	inputSystem->AddMappingContext(InputMapping, 0);
}

void AHero_Quinn::Move(const FInputActionValue& value) {
	if (Controller != nullptr) {
		auto moveValue = value.Get<FVector2D>();
		auto controlRotation = Controller->GetControlRotation();

		if (abs(moveValue.X) > 1e-6f) AddMovementInput(controlRotation.RotateVector(FVector::RightVector), moveValue.X);
		if (abs(moveValue.Y) > 1e-6f) AddMovementInput(controlRotation.RotateVector(FVector::ForwardVector), moveValue.Y);
	}
}

void AHero_Quinn::Look(const FInputActionValue& value) {
	auto lookValue = value.Get<FVector2D>();
	if (abs(lookValue.X) > 1e-6f) AddControllerYawInput(lookValue.X);
	if (abs(lookValue.Y) > 1e-6f) AddControllerPitchInput(lookValue.Y);
}

// Runs on server and gets called by the client.
void AHero_Quinn::Server_SetIsADS_Implementation(bool value) {
	_isADSCpp = value;
}

void AHero_Quinn::ADSStarted(const FInputActionValue& value) {
	auto aim = this->GetBaseAimRotation();

	// Client-specific functionality
	if (GetOwner()->HasAuthority()) {
		// Set on server and replicate to all clients
		_isADSCpp = true;
		UE_LOG(LogTemp, Log, TEXT("Server: pitch %f, yaw %f, roll %f"), aim.Pitch, aim.Yaw, aim.Roll);
	}
	else {
		// Set locally on client
		_isADSCpp = true;
		this->Server_SetIsADS(true);
		UE_LOG(LogTemp, Log, TEXT("Client: pitch %f, yaw %f, roll %f"), aim.Pitch, aim.Yaw, aim.Roll);
	}
}

void AHero_Quinn::ADSCompleted(const FInputActionValue& value) {
	auto aim = this->GetBaseAimRotation();

	// Client-specific functionality
	if (GetOwner()->HasAuthority()) {
		// Set on server and replicate to all clients
		_isADSCpp = false;
	}
	else {
		// Set locally on client
		_isADSCpp = false;
		// Set on server via RPC
		this->Server_SetIsADS(false);
	}
}

void AHero_Quinn::Shoot(const FInputActionValue& value) {
	if (_pistolMesh == nullptr) {
		UE_LOG(LogCore, Error, TEXT("Pistol mesh wasn't found and is null"));
		return;
	}

	UGameplayStatics::SpawnEmitterAttached(
		_pistolFlash, 
		_pistolMesh, 
		_pistolMuzzleBoneName, 
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget
	);

	UGameplayStatics::PlaySound2D(this->GetWorld(), _weaponSound);

	DrawShootLine(5.0f);
}

void AHero_Quinn::DrawShootLine(float drawTime) {
	auto pistolSocketTransform = _pistolMesh->GetSocketTransform(_pistolMuzzleBoneName);
	auto start = pistolSocketTransform.GetLocation();
	auto end = start + pistolSocketTransform.GetRotation().GetForwardVector() * 10000;
	auto traceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	UE_LOG(LogTemp, Log, TEXT("Start: %f,%f,%f, end: %f,%f%f"), start.X, start.Y, start.Z, end.X, end.Y, end.Z);

	FHitResult hit;
	const TArray<AActor*> actorsToIgnore;
	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), start, end, traceChannel, false, actorsToIgnore, EDrawDebugTrace::ForDuration,
		hit, true, FColor::Red, FColor::Green, drawTime
	);
}

void AHero_Quinn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHero_Quinn, _isADSCpp);
}

void AHero_Quinn::OnRep_IsADSCpp() {
	UE_LOG(LogTemp, Log, TEXT("OnRep_IsADSCpp"));
	auto aim = this->GetBaseAimRotation();
	UE_LOG(LogTemp, Log, TEXT("Rep: pitch %f, yaw %f, roll %f"), aim.Pitch, aim.Yaw, aim.Roll);
}

bool AHero_Quinn::GetIsAdsCpp() {
	return _isADSCpp;
}
