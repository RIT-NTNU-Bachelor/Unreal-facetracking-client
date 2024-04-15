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
    
    // Set up standard values for Camera Tweaking.
    IncludeRotation = true;
    IncludeMovement = true;
    FOVEnabled = true;
    FOVSensitivity = 0.03f;
    XMovementSensitivity = 0.5f;
    YMovementSensitivity = 0.7f;

    // Setting values for X and Y translation
    // Focal length of the camera
    FocalLength = 635.0;    // Get Focal Length from camera specs
    WidthUE = 600.0f;       // Measure within Unreal Editor 
    HeightUE = 400.0f;      // Measure within Unreal Editor 

    // SCALAR = MAX_WIDTH_UE / FRAME_WIDTH_OPENCV
    // Change to the correct scale of values 
    // Note that it may be to much movement. Take 80% of it to take into account the wall
    CX = 320.0f;    // Retrive from camera-center.py
    CY = 240.0f;    // Retrive from camera-center.py

    // Set the scalars used in camera movement.
    Scalar_X = (WidthUE / 480.0f) * XMovementSensitivity;
    Scalar_Y = (HeightUE / 480.0f) * YMovementSensitivity;

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
    float z0 = 70.0f;
    UE_LOG(LogTemp, Warning, TEXT("FOV Z CORD: %f"), z);

    // Returning the result of the sigmoid calculation 
    float result = L / (1 + exp((FOVSensitivity * (z - z0)))) + C;
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


/*
    Function that calulates the roll of the camera.
    Takes the change in x position, and the estimated distance between the screen and the user (Z). 
    The distance has a minimal impact, due to logmaritic scale. This is because of the value range of Zs

    Returns the new roll

*/
float AMovableCamera::rotation_roll(float x_change, float z_change) {
    float c = 10;
    float z_impact =  z_change / 1900;
    float droll = 0.09 - z_impact;

    return droll * (x_change - WidthUE / 2) + c;
};


/*
    Function that calulates the pitch of the camera.
    Takes the change in y position, and the estimated distance between the screen and the user (Z).
    The distance has a minimal impact, due to logmaritic scale. This is because of the value range of Z  

    Returns the new pitch

*/
float AMovableCamera::rotation_pitch(float y_change, float z_change) {
    float c = 10;
    float z_impact = z_change / 1900;
    float dpitch = 0.06 - z_impact;

    return dpitch * (y_change - HeightUE / 2) + c;
};

// Update the position of the movable camera, called each tick.
void AMovableCamera::UpdatePosition()
{
    // Retriving the last known position and rotation of camera component 
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;

    // Gets the face coordinates from the headtracking component.
    HeadTrackingComponent->GetFaceCoordinates(newLocation);

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

    // Calulate the origninal Z postion retrived from OpenCV server
    float z_opencv = newLocation.Z + HeadTrackingComponent->CameraCentering.Z;
    
    // Option to remove rotation aspect of camera movement in UE.
    if (IncludeRotation)
    {
        // New position of the camera after handling as FRotator, the standard format of rotation.
        float pitch = rotation_pitch(newLocation.Y, z_opencv);
        float roll = rotation_roll(newLocation.X, z_opencv);

        UE_LOG(LogTemp, Warning, TEXT("Pitch(Y): %f, Roll(X): %f, Z: %f"), pitch, roll, z_opencv);


        LastKnownRotation = StartDirection + FRotator(pitch, roll, 0);
        // Sets new rotation relative to the world.
        CameraComponent->SetWorldRotation(LastKnownRotation); 
    }

    // Option to include or remove fov.
    if (HeadTrackingComponent->ZAxis && FOVEnabled)
    {
        

        // Calulating the new fov value and changing it 
        float new_fov = this->FOV(z_opencv);
        CameraComponent->SetFieldOfView(new_fov);
    }
}


// Sets the camera settings based on preset index, from data table.
void AMovableCamera::ChangeCameraSettings(int32 PresetIndex)
{
    if (CameraPresets.IsValidIndex(PresetIndex))
    {
        FCameraPreset Preset = CameraPresets[PresetIndex];

        // Set the camera properties based on the preset
        IncludeRotation = Preset.IncRot;
        IncludeMovement = Preset.IncMov;
        FOVEnabled = Preset.IncFov;

        XMovementSensitivity = Preset.XMoveSen;
        YMovementSensitivity = Preset.YMoveSen;
        ZMovementSensitivity = Preset.ZMoveSen;

        XRotationSensitivity = Preset.XRotSen;
        YRotationSensitivity = Preset.YRotSen;
        ZRotationSensitivity = Preset.ZRotSen;

        FOVSensitivity = Preset.FOVSen;
    }
}


// Loads presets from data table.
void AMovableCamera::LoadPresetsFromDataTable()
{
    // If preset data table exist, do this..
    if (PresetDataTable)
    {
        static const FString ContextString(TEXT("Camera Preset Data Table"));

        // Iterate over each row in the data table
        for (auto& RowName : PresetDataTable->GetRowNames())
        {
            // Retrieve each row as an FCameraPreset struct
            FCameraPreset* Preset = PresetDataTable->FindRow<FCameraPreset>(RowName, ContextString, true);

            if (Preset)
            {
                // Add the struct to CameraPresets array, simplifies process of setting presets.
                CameraPresets.Add(*Preset);
            }
        }
    }
}