// Fill out your copyright notice in the Description page of Project Settings.

#include "Math/Matrix.h"
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

    newLocation = FVector();
    ProjectionEnabled = true;
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
    float result = L / (1 + exp(( k * (z - z0)))) + C;
    return result;
}


void AMovableCamera::UpdatePosition()
{
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;

    HeadTrackingComponent->UpdateHeadPosition(newLocation);

    // New position of the camera after handling as FVector, the standard format of coordinates.
    if (IncludeMovement)
    {
        X = newLocation.X * XMovementSensitivity;
        Y = newLocation.Y * YMovementSensitivity;
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

    // Set the custom projection matrix initially
    if (ProjectionEnabled);
}

// CalculateCustomProjectionMatrix
FMatrix AMovableCamera::GetProjectionMatrix(
    const FVector& pa,
    const FVector& pb,
    const FVector& pc,
    const FVector& pe,
    float near_clip,
    float far_clip) {

    // Compute the screen's orthonormal basis vectors
    FVector vr = (pb - pa).GetSafeNormal();
    FVector vu = (pc - pa).GetSafeNormal();
    FVector vn = FVector::CrossProduct(vr, vu).GetSafeNormal();

    // Compute vectors from pe to each of the screen's corners
    FVector va = pa - pe;
    FVector vb = pb - pe;
    FVector vc = pc - pe;

    // Compute the distance from the eye to screen plane
    float d = -FVector::DotProduct(va, vn);

    // Compute the extents of the perpendicular projection
    float l = FVector::DotProduct(vr, va) * near_clip / d;
    float r = FVector::DotProduct(vr, vb) * near_clip / d;
    float b = FVector::DotProduct(vu, va) * near_clip / d;
    float t = FVector::DotProduct(vu, vc) * near_clip / d;

    // Construct the projection matrix
    FMatrix projectionMatrix = FMatrix();
    projectionMatrix.M[0][0] = 2.0f * near_clip / (r - l);
    projectionMatrix.M[1][1] = 2.0f * near_clip / (t - b);
    projectionMatrix.M[2][2] = -(far_clip + near_clip) / (far_clip - near_clip);
    projectionMatrix.M[2][3] = -1.0f;
    projectionMatrix.M[3][2] = -(2.0f * far_clip * near_clip) / (far_clip - near_clip);
    projectionMatrix.M[0][2] = (r + l) / (r - l);
    projectionMatrix.M[1][2] = (t + b) / (t - b);
    projectionMatrix.M[3][3] = 0.0f;

    // Construct the rotation matrix from the basis vectors
    FMatrix rotationMatrix = FMatrix();
    rotationMatrix.M[0][0] = vr.X; rotationMatrix.M[1][0] = vr.Y; rotationMatrix.M[2][0] = vr.Z;
    rotationMatrix.M[0][1] = vu.X; rotationMatrix.M[1][1] = vu.Y; rotationMatrix.M[2][1] = vu.Z;
    rotationMatrix.M[0][2] = vn.X; rotationMatrix.M[1][2] = vn.Y; rotationMatrix.M[2][2] = vn.Z;

    // Combine the matrices
    FMatrix finalMatrix = rotationMatrix * projectionMatrix;

    return finalMatrix;
}

