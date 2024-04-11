// Fill out your copyright notice in the Description page of Project Settings.

#include "MovableCamera.h"

// Sets default values
AMovableCamera::AMovableCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    HeadTrackingComponent = CreateDefaultSubobject<UHeadTracking>(TEXT("HeadTrackingComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;

    StartLocation = FVector(0.0f, 0.0f, 0.0f);
    StartDirection = FRotator(0.0f, 0.0f, 0.0f);

    IncludeRotation = true;
    IncludeMovement = true;

    FOVEnabled = true;
    FOVSensitivity = 3.0f;

    XMovementSensitivity = 1.0f;
    YMovementSensitivity = 1.0f;
    ZMovementSensitivity = 1.0f;

    XRotationSensitivity = 0.1f;
    YRotationSensitivity = 0.1f;
    ZRotationSensitivity = 0.0f;


    // Setting values for X and Y translation
    // Focal length of the camera
    FocalLength = 635.0;    // Get Focal Length from camera specs
    WidthUE = 600.0f;       // Measure within Unreal Editor 
    HeightUE = 400.0f;      // Measure within Unreal Editor 


    // SCALAR = MAX_WIDTH_UE / FRAME_WIDTH_OPENCV
    // Change to the correct scale of values 
    // Note that it may be to much movement. Take 80% of it to take into account the wall
    Scalar_X = (WidthUE / 480.0f) * 0.80f;
    Scalar_Y = (HeightUE / 480.0f) * 0.80f;
    CX = 320.0f;    // Retrive from camera-center.py
    CY = 240.0f;    // Retrive from camera-center.py

    newLocation = FVector();
}

// Called when the game starts or when spawned
void AMovableCamera::BeginPlay()
{
	Super::BeginPlay();
    HeadTrackingComponent->StartHeadTracking();

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); // Retrieves the FP controller.
    // If FP controller is found, set new actor location and rotation.
    if (PlayerController)
    {
        APawn* CurrentPawn = PlayerController->GetPawn();

        // Check if the current pawn is not null and is of type AHeadTracking
        AMovableCamera* CameraPawn = Cast<AMovableCamera>(CurrentPawn);
        if (CameraPawn != nullptr)
        {
            // If it's already a HeadTrackingPawn, just move it to the standard location and rotation
            CameraPawn->SetActorLocation(StartLocation);
            CameraPawn->SetActorRotation(StartDirection);
        }
    }
}

// Called every frame
void AMovableCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdatePosition();
}


float AMovableCamera::FOV(float x, float z) {
    // Calculating variables
    float l = x * Scalar_X;
    float r = (CX * 2 - l) * Scalar_X;
    float distance = (sqrt(l * l + z * z) * sqrt(r * r + z * z));

    // Returning the result of the sigmoid calculation 
    return (acos((-l * r + z * z) / distance))*30;
}

void AMovableCamera::UpdatePosition()
{
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;

    HeadTrackingComponent->UpdateHeadPosition(newLocation);

    // New position of the camera after handling as FVector, the standard format of coordinates.
    if (IncludeMovement)
    {
        X = TranslateX(newLocation.X);
        Y = TranslateY(newLocation.Y);
        Z = newLocation.Z * ZMovementSensitivity;
        
        // Translating the cordinates relative to the cameras axis 
        LastKnownPosition = StartLocation + FVector(Z, -X, -Y);
        CameraComponent->SetRelativeLocation(LastKnownPosition); // Sets new position in the world.
        UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), X, Y, Z);
    }
    
    // Option to remove rotation aspect of camera movement in UE.
    if (IncludeRotation)
    {
        // New position of the camera after handling as FRotator, the standard format of rotation.
        LastKnownRotation = StartDirection + FRotator(newLocation.Y * YRotationSensitivity, newLocation.X * XRotationSensitivity, newLocation.Z * ZRotationSensitivity);
        CameraComponent->SetWorldRotation(LastKnownRotation); // Sets new rotation relative to parent.
    }

    // Option to include or remove fov.
    if (HeadTrackingComponent->ZAxis && FOVEnabled)
    {   
        float new_fov = this->FOV(newLocation.X + HeadTrackingComponent->CameraCentering.X, newLocation.Z + HeadTrackingComponent->CameraCentering.Z);
        CameraComponent->SetFieldOfView(new_fov);
        UE_LOG(LogTemp, Warning, TEXT("FOV: %f"), new_fov);
    }
}


float  AMovableCamera::TranslateX(float x_opencv) {
    // Calculate the x translation 
    float res = (((2 * CX * x_opencv - 2 * CX) / FocalLength) * Scalar_X);

    // Use half of the set width as a maximum + padding based on the wall
    if (abs(res) > (WidthUE / 2 - 20))
        return WidthUE / 2 - 20; 
    return res; 
};


float  AMovableCamera::TranslateY(float y_opencv) {
    // Calculate the y translation 
    float res = (((2 * CY * y_opencv - 2 * CY) / FocalLength) * Scalar_Y);

    // Use half of the set height as a maximum + padding based on the wall
    if (abs(res) > (WidthUE / 2 - 20))
        return WidthUE / 2 - 20;
    return res;
};