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

    XMovementSensitivity = 2.0f;
    YMovementSensitivity = 2.0f;
    ZMovementSensitivity = 1.0f;

    XRotationSensitivity = 0.1f;
    YRotationSensitivity = 0.1f;
    ZRotationSensitivity = 0.0f;

    // Setting configuration for calculating the camera center 
    focal_length = 635.0f;
    center_x_camera = 345.0f;
    center_y_camera = 250.0f;

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


float AMovableCamera::FOV(float z) {
    // Constants that can be tweaked 
    float L = 30.0f;
    float C = 70.0f;
    float k = 0.03f;
    float z0 = 70.0f;
    UE_LOG(LogTemp, Warning, TEXT("FOV Z CORD: %f"), z);

    // Returning the result of the sigmoid calculation 
    float result = L / (1 + exp((k * (z - z0)))) + C;
    return result;
}

float AMovableCamera::translate_x(float x, float z) {
    return ((x - center_x_camera) * z / focal_length);
}

float AMovableCamera::translate_y(float y, float z) {
    return ((y - center_y_camera) * z / focal_length);
}


void AMovableCamera::UpdatePosition()
{
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;

    HeadTrackingComponent->UpdateHeadPosition(newLocation);

    // New position of the camera after handling as FVector, the standard format of coordinates.
    if (IncludeMovement)
    {
        X = translate_x(newLocation.X, newLocation.Z) * XMovementSensitivity;
        Y = translate_y(newLocation.Y, newLocation.Z) * YMovementSensitivity;
        Z = newLocation.Z * ZMovementSensitivity;

        LastKnownPosition = StartLocation + FVector(Z, X, Y);
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
        float new_fov = this->FOV(newLocation.Z + HeadTrackingComponent->CameraCentering.Z);
        CameraComponent->SetFieldOfView(new_fov);
        UE_LOG(LogTemp, Warning, TEXT("FOV: %f"), new_fov);
    }
}

