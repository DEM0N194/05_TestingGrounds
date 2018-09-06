// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Mannequin.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Weapons/Gun.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

// Sets default values
AMannequin::AMannequin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FP_Camera"));
	FPCameraComponent->SetupAttachment(GetCapsuleComponent());
	FPCameraComponent->RelativeLocation = FVector(0.44f, 1.75f, 69.f); // Position the camera

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Arms"));
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->SetupAttachment(FPCameraComponent);
	FPMesh->bCastDynamicShadow = false;
	FPMesh->CastShadow = false;
	FPMesh->RelativeLocation = FVector(3.3f, -5.0f, -161.0f);
	FPMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
}

void AMannequin::PullTrigger()
{
	Gun->OnFire();
}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	//Spawn and attach gun actor to Skeleton, doing it here because the skeleton is not yet created in the constructor
	if (GunBlueprint == nullptr)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: Gun_BP in BP_Character.uasset is set to NONE!"));
	}
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	Gun->AttachToComponent(FPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("GripPoint"));
	Gun->SetAnimInstance(GetMesh()->GetAnimInstance());

	if (InputComponent != nullptr)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::PullTrigger);
	}
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FPCameraComponent->SetRelativeRotation(FRotator(GetControlRotation().Pitch, 0.0f, 0.0f));
}

// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

