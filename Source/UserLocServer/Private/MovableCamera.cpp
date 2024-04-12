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

    // TODO: change to be based on K, L and Z0 not FOV sensitivity 
    FOVSensitivity = 3.0f;


    // TODO REMOVE X AND Y SENSITVITY AND CHANGE TO PRESET FOR Scalar_X and Scalar_Y
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

    // Initilize the new location as a vector 
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

/*
    Function that calucated the new field of view.
    Uses a logistical function to set the FOV. 
    Takes the distance from the camera from the OpenCV Server, as a parameter. 

    Returns the calulted field of view
*/
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

/*
    Function that translates the camera to a new x position based.
    Using both scalar and normalizing based on the frame width and focal length of the camera. 
    Takes the x position of the user relative to the frame from OpenCV Server, as a parameter. 

    Returns the change in x position relative to the stating x positon , or the max allowed width based on the Unreal Engine scene. 
    Can be used by adding the change with the starting postion of the camera, and setting that value to the new X postion of the camera. 
*/
float  AMovableCamera::TranslateX(float x_opencv) {
    // Calculate the x translation 
    float new_x = (2 * CX * x_opencv / FocalLength) * Scalar_X;

    // Use half of the set width as a maximum + padding based on the wall
    // Movement should not be more than half of the width based on camera center
    if (abs(new_x) > (WidthUE / 2 - 20))
        return WidthUE / 2 - 20;
    return new_x;
};



/*
    Function that translates the camera to a new y position based.
    Using both scalar and normalizing based on the frame height and focal length of the camera.
    Takes the y position of the user relative to the frame from OpenCV Server, as a parameter.

    Returns the change in y position relative to the stating y positon , or the max allowed height based on the Unreal Engine scene.
    Can be used by adding the change with the starting postion of the camera, and setting that value to the new y postion of the camera.
*/
float  AMovableCamera::TranslateY(float y_opencv) {
    // Calculate the y translation 
    float new_y = (2 * CY * y_opencv / FocalLength) * Scalar_Y;

    // Use half of the set height as a maximum + padding based on the wall
    if (abs(new_y) > (HeightUE / 2 - 20))
        return HeightUE / 2 - 20;
    return new_y;
};

void AMovableCamera::UpdatePosition()
{
    // Retriving the last known position and rotation of camera component 
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;


    // TODO: why do we do this and this: CameraComponent->SetWorldRotation(LastKnownRotation);
    // Please clearify mr sander
    HeadTrackingComponent->UpdateHeadPosition(newLocation);

    // New position of the camera after handling as FVector, the standard format of coordinates.
    if (IncludeMovement)
    {   
        // Calculate the new change in x and y position 
        X = TranslateX(newLocation.X);
        Y = TranslateY(newLocation.Y);

        // The Z axis moves relative to its input from OpenCV server multiplied by its own factor
        Z = newLocation.Z * ZMovementSensitivity;
        
        // Translating the cordinates relative to the cameras axis 
        // Note that the camera has its own axis - i.e its important to note that we set the new values based on the cameras axis.  
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
        // Calulate the origninal Z postion retrived from OpenCV server
        float z_opencv = newLocation.Z + HeadTrackingComponent->CameraCentering.Z; 

        // Calulating the new fov value and changing it 
        float new_fov = this->FOV(z_opencv);
        CameraComponent->SetFieldOfView(new_fov);
    }
}