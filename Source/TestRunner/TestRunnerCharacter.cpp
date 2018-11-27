// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TestRunnerCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "NewFloor.h"

FVector floorPosition;
FVector floorDirection = FVector(1,0,0);
int frameCount = 0;

//////////////////////////////////////////////////////////////////////////
// ATestRunnerCharacter

ATestRunnerCharacter::ATestRunnerCharacter()
{
	floorDirection = FVector(1, 0, 0);
	floorPosition = FVector(2000, 0, -200);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->MaxWalkSpeed = 800.0F;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 54.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	//SpringArm->AttachToComponent(RootComponent);
	SpringArm->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpringArm->TargetArmLength = 525.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	//Camera->AttachToComponent(SpringArm, USpringArmComponent::SocketName);
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	direction = 0;
	canTurnRight = true;


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATestRunnerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATestRunnerCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATestRunnerCharacter::OnStopJump);

	//PlayerInputComponent->BindAxis("MoveForward", this, &ATestRunnerCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ATestRunnerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestRunnerCharacter::ChangeDirection);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATestRunnerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATestRunnerCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATestRunnerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATestRunnerCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATestRunnerCharacter::OnResetVR);
}


void ATestRunnerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATestRunnerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	OnStartJump();
}

void ATestRunnerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	OnStopJump();
}

void ATestRunnerCharacter::OnStartJump()
{
	if (GetCharacterMovement()->IsMovingOnGround() == true)
	{
		resetMyDoOnce();
	}

	if ((GetCharacterMovement()->IsMovingOnGround() == true) && (jumpCounter < 1))
	{
		bPressedJump = true;
	}
	else if ((GetCharacterMovement()->IsMovingOnGround() == false) && (jumpCounter < 1))
	{
		myDoOnce();
	}
}

void ATestRunnerCharacter::myDoOnce()
{
	if (bDo == true)
	{
		if (jumpCounter == 0)
		{
			LaunchCharacter(FVector(0, 0, 1) * 400, false, false);
		}
		else
		{
			LaunchCharacter(FVector(0, 0, 1) * 600, false, false);
		}

		++jumpCounter;
		return;
	}
	else
	{
		return;
	}
}

void ATestRunnerCharacter::resetMyDoOnce()
{
	bDo = true;
	jumpCounter = 0;
	return;
}

void ATestRunnerCharacter::OnStopJump()
{
	bPressedJump = false;
}//*/

void ATestRunnerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATestRunnerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATestRunnerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATestRunnerCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATestRunnerCharacter::ChangeDirection(float Value)
{
	if (Value != 0 && canTurnRight) {
		canTurnRight = false;
		direction += Value;
		if (direction > 3)
			direction = 0;
		else if (direction < 0)
			direction = 3;
	}
	else if (Value == 0)
		canTurnRight = true;


}

void ATestRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (direction == 0)
	{
		MoveForward(1);
	}
	else if (direction == 2)
	{
		MoveForward(-1);
	}
	else if (direction == 1)
	{
		MoveRight(1);
	}
	else if (direction == 3)
	{
		MoveRight(-1);
	}

	ANewFloor *myFloor;

	UWorld* const World = GetWorld();
	if (World && frameCount++ > 15) {
		frameCount = 0;
		FVector SpawnLocation = floorPosition;// GetActorLocation() + FVector(0, newCount * 100, -5);
		floorPosition += floorDirection * 400;
		updateFloorDirection(floorDirection);
		const FRotator FloorRotation = FRotator(0, 0, 0);
		myFloor = World->SpawnActor<ANewFloor>(SpawnLocation, FloorRotation);
	}

}

void ATestRunnerCharacter::updateFloorDirection(FVector& direction) {

	float zChange = 0.0F;//used to randomly change the height of the floor piece

	int randNum = rand() % 10;
	if (randNum > 7)
		zChange = 0.1;
	else if (randNum > 4)
		zChange = -0.1;

	floorDirection = FVector(1, 0, zChange);

	
}