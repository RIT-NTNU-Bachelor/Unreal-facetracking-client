// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "UDPReceiver.h"
#include "HeadTracking.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include <iostream>
#include "Engine/DataTable.h"

#include "MovableCamera.generated.h"

// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFaceDetectionSignature);

USTRUCT(BlueprintType)
struct FCameraPreset : public FTableRowBase
{
    GENERATED_BODY()

public:
    // Including rotation or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncRot;
    // Include movement or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncMov;

    // XYZ movement sensitivity modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float XMoveSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float YMoveSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ZMoveSen;

    // XYZ rotation sensitivity modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float XRotSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float YRotSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ZRotSen;

    // FOV enabler and sensitivity modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncFov;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float FOVSen;
};

USTRUCT(BlueprintType)
struct FLevelSpecificSettings : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FVector StartLoc;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FRotator StartDir;
};

UCLASS()
class USERLOCSERVER_API AMovableCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovableCamera();

    // Defines necessary components for the MovableCamera.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head Tracking (Server)")
        UHeadTracking* HeadTrackingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movable Camera")
		UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Preset array in UE.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        TArray<FCameraPreset> CameraPresets;

    // Preset reference to Data table
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        UDataTable* PresetDataTable;

    // Reference to level settings data table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        UDataTable* LevelSpecificSettings;

    UFUNCTION(BlueprintCallable) void ChangeCameraSettings(FCameraPreset Preset);
    UFUNCTION(BlueprintCallable) void CenterCamera(FVector NewCenter);
    UFUNCTION(BlueprintCallable) void SetLevelSpecificSettings(FLevelSpecificSettings LevelSetting);

   // Start location and direction modifiers in UE, this has to be changed for the actual camera placement.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Transform")
        FVector StartLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Transform")
        FRotator StartDirection;

    // Movement modifiers, allows for enabling/disabling movement, and change of sensitivity for each axis.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        bool IncludeMovement;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Movement")
        float XMovementSensitivity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Movement")
        float YMovementSensitivity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Movement")
        float ZMovementSensitivity;

    // Rotation modifiers, allows for enabling/disabling rotation, and change of sensitivity for each axis.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        bool IncludeRotation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Rotation")
        float YawSensitivity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|Rotation")
        float PitchSensitivity;

    // FOV modifiers, allows for enabling/disabling fov, and change the sensitivity.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|FOV")
        bool FOVEnabled;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking|FOV")
        float FOVSensitivity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        bool OutOfBoundsEnabled;

    // Delegates for event upon face loss and detection.
    UPROPERTY(BlueprintAssignable)
        FFaceDetectionSignature OnFaceLost;
    UPROPERTY(BlueprintAssignable)
        FFaceDetectionSignature OnFaceFound;

    // Function to load preset from existing data table.
    void LoadPresetsFromDataTable();

private:
    //FVector newLocation;

    // Function to calculate FOV
    float FOV(float z);

    // Coordinate variables.
    float X;
    float Y;
    float Z;

    // Focal length of the camera
    float FocalLength;
    float Scalar_X;
    float Scalar_Y;
    float WidthUE; 
    float HeightUE;
    float MaxZ; 

    // Center position of OpenCV Frame
    float CX;
    float CY;

    float BlurCounter; 
    bool bHasDebugMessage; 

    // Function for telling the user that they are out of view 
    bool OutOfBounds(bool has_coords); 

    // Function for updating the user postion 
    void UpdatePosition(FVector NewLocation);

    // Funcitons for calulating the change for x and y axis
    float TranslateX(float x_opencv);
    float TranslateY(float y_opencv);

    //Function for rotation 
    float rotation_yaw(float current_yaw, float x_change, float z_change);
    float rotation_pitch(float current_roll, float y_change, float z_change);
};
