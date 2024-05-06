// Fill out your copyright notice in the Description page of Project Settings.

#include "MovableCamera.h"

#include "GameFramework/Pawn.h"
#include <chrono>
#include <fstream>      // Used for writing to file.
#include <string>
#include <iomanip>      // for std::put_time
#include <iostream>
#include "CoreMinimal.h"
#include "EngineUtils.h"

/*
    Constructor of Movable Camera.
    Initializes important values.
*/
AMovableCamera::AMovableCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    HeadTrackingComponent = CreateDefaultSubobject<UHeadTracking>(TEXT("HeadTrackingComponent"));
    HeadTrackingComponent->OnFaceMoved.BindUObject(this, &AMovableCamera::UpdatePosition);
    HeadTrackingComponent->UDPReceiverComponent->NoUDPDataReceived.BindUObject(this, &AMovableCamera::OutOfBounds);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
    
    // Set up standard values for Camera Tweaking.
    IncludeRotation = true;
    IncludeMovement = true;
    FOVEnabled = true;
    FOVSensitivity = 0.03f;
    XMovementSensitivity = 0.5f;
    YMovementSensitivity = 0.7f;
    PitchSensitivity = 0.6f;
    YawSensitivity = 0.9f;

    // Setting values for X and Y translation
    // Focal length of the camera
    FocalLength = 635.0;    // Get Focal Length from camera specs
    WidthUE = 600.0f;       // Measure within Unreal Editor 
    HeightUE = 400.0f;      // Measure within Unreal Editor 
    MaxZ = 300.0f;          // Max depth the user is away from the camera. Recommend using 300 cm by default

    // SCALAR = MAX_WIDTH_UE / FRAME_WIDTH_OPENCV
    // Change to the correct scale of values 
    // Note that it may be to much movement. Take 80% of it to take into account the wall
    CX = 320.0f;    // Retrive from camera-center.py
    CY = 240.0f;    // Retrive from camera-center.py

    // Set the scalars used in camera movement.
    Scalar_X = (WidthUE / 480.0f);
    Scalar_Y = (HeightUE / 480.0f);
}


/*
    BeginPlay is called when the game starts, as long as the actor is present in a level.
*/
void AMovableCamera::BeginPlay()
{
	Super::BeginPlay();
    HeadTrackingComponent->StartHeadTracking();
    // Bind the delegate to the UpdatePosition function

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


/*
    Functionality which is run every frame.
*/ 
void AMovableCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
    float new_x = (2 * CX * x_opencv / FocalLength) * Scalar_X * XMovementSensitivity;

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
    float new_y = (2 * CY * y_opencv / FocalLength) * Scalar_Y * YMovementSensitivity;

    // Use half of the set height as a maximum + padding based on the wall
    if (abs(new_y) > (HeightUE / 2 - 20))
        return HeightUE / 2 - 20;
    return new_y;
};


/*
    Function that calulates the yaw of the camera.
    Takes the current yaw of the camera, change in x position, and the estimated distance between the screen and the user (Z). 
    The distance has a minimal impact, due to logmaritic scale. This is because of the value range of Zs

    Returns the new yaw
*/
float AMovableCamera::rotation_yaw(float current_yaw, float x_change, float z_change) {
    float depthScale = 1 / (1 + z_change / MaxZ); 

    float dyaw = YawSensitivity * depthScale; 
    float targetYaw = dyaw * x_change;

    float smoothedYaw = current_yaw + (targetYaw - current_yaw) * 0.1;

    return smoothedYaw; 
};


/*
    Function that calulates the pitch of the camera.
    Takes the current pitch of the camera, change in y position, and the estimated distance between the screen and the user (Z).
    The distance has a minimal impact, due to logmaritic scale. This is because of the value range of Z  

    Returns the new pitch
*/
float AMovableCamera::rotation_pitch(float current_pitch, float y_change, float z_change) {
    float depthScale = 1 / (1 + z_change / MaxZ);

    float dpitch = PitchSensitivity * depthScale;
    float targetPitch = dpitch * y_change;

    return current_pitch + (targetPitch - current_pitch) * 0.1;
};


/*
    Function to get current time and format it.
    Only used in latency testing.
    May impact performance when enabled.
*/ 
std::string GetCurrentTimeFormatted() {
    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert time_t to tm (broken down time)
    std::tm* now_tm = std::localtime(&now_time_t);

    // Get fractional seconds
    auto fractional_seconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000000;

    // Format the time as a string
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm); // Format date and time

    // Append milliseconds
    std::string result(buffer);
    result += "." + std::to_string(fractional_seconds);

    return result;
}


/*
    Update the position of the movable camera, called each delegate execution.
    Parameter newLocation is a FVector comprised of updated X, Y, Z coordinates.
*/ 
void AMovableCamera::UpdatePosition(FVector newLocation)
{
    InBounds();

    // Retriving the last known position and rotation of camera component 
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;

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
        // Sets new position in the world, relative to parent.
        CameraComponent->SetRelativeLocation(LastKnownPosition); 
        //UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), X, Y, Z);
    }

    // Calulate the origninal Z postion retrived from OpenCV server
    float z_opencv = newLocation.Z + HeadTrackingComponent->CameraCentering.Z;
    
    // Option to remove rotation aspect of camera movement in UE.
    if (IncludeRotation)
    {
        // New position of the camera after handling as FRotator, the standard format of rotation.
        float pitch = rotation_pitch(LastKnownRotation.Pitch,newLocation.Y, z_opencv);
        float yaw = rotation_yaw(LastKnownRotation.Yaw, newLocation.X, z_opencv);

        //UE_LOG(LogTemp, Warning, TEXT("Pitch(Y): %f, Roll(X): %f, Z: %f"), pitch, yaw, z_opencv);
      
        LastKnownRotation = StartDirection + FRotator(pitch, yaw, 0);
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

    // Latency testing. Writes timestamp to file in Logs. bLogTimestamps is false by default. !!!Only fuctional with Windows!!!
    if (HeadTrackingComponent->bLatencyTesting)
    {
        // Open the file in append mode. Enter own file path for latency testing.
        std::ofstream file("C:\\Users\\sande\\GitHub\\Unreal-facetracking-client\\Logs\\latency_log.txt", std::ios_base::app);
        // Check if the file is open
        if (file.is_open()) {
            // Write the latency string to the file
            file << HeadTrackingComponent->SendIndex << "," << GetCurrentTimeFormatted() << std::endl;
            // Close the file
            file.close();
        }
        else {
            // Print an error message if the file cannot be opened
            UE_LOG(LogTemp, Error, TEXT("Unable to open file"));
        }
    }
}


/*
    Sets the camera settings based on preset index, from data table.
    Parameter FCameraPreset is a struct made specifically for movable camera presets.
*/
void AMovableCamera::ChangeCameraSettings(FCameraPreset Preset)
{
    // Set the camera properties based on the preset
    IncludeRotation = Preset.IncRot;
    IncludeMovement = Preset.IncMov;
    FOVEnabled = Preset.IncFov;

    XMovementSensitivity = Preset.XMoveSen;
    YMovementSensitivity = Preset.YMoveSen;
    ZMovementSensitivity = Preset.ZMoveSen;

    YawSensitivity = Preset.XRotSen;
    PitchSensitivity = Preset.YRotSen;

    FOVSensitivity = Preset.FOVSen;
}


/*
    Function to set new camera center, using keybinds.
*/
void AMovableCamera::CenterCamera(FVector NewCenter)
{
    StartLocation = NewCenter;
}


/*
    Function to set level specific settings.
    As of version 1.0.0, only start location and direction is set.
    Specifically used for Reset camera functionality.
*/
void AMovableCamera::SetLevelSpecificSettings(FLevelSpecificSettings LevelSetting)
{
    StartLocation = LevelSetting.StartLoc;
    StartDirection = LevelSetting.StartDir;
}


/*
    Function to load presets from data table.
*/
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


/*
    Function for OPENING out of bounds message, which pops up if user is out of view of the camera.
*/
void AMovableCamera::OutOfBounds() {
    // If the face is not in view, enter this if statement.
    if (OutOfBoundsEnabled && !bOutOfBoundsShowing)
    {
        OnFaceLost.Broadcast();
        bOutOfBoundsShowing = true;
    }
}


/*
    Function for CLOSING out of bounds message, which pops up if user is out of view of the camera.
*/
void AMovableCamera::InBounds() {
    // If the face is in view, enter this if statement.
    if (OutOfBoundsEnabled && bOutOfBoundsShowing)
    {
        OnFaceFound.Broadcast();
        bOutOfBoundsShowing = false;
    }
}