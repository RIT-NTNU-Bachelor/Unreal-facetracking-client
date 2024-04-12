// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "UDPReceiver.h"
#include "HeadTracking.h"
#include "GameFramework/Pawn.h"
#include <iostream>
#include "Engine/DataTable.h"
#include "MovableCamera.generated.h"

USTRUCT(BlueprintType)
struct FCameraPreset : public FTableRowBase
{
    GENERATED_BODY()

public:
    /** including rotation or not **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncRot;
    /** include movement or not **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncMov;

    /** XYZ movement sensitivity modifiers **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float XMoveSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float YMoveSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ZMoveSen;

    /** XYZ rotation sensitivity modifiers **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float XRotSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float YRotSen;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float ZRotSen;

    /** fov enabler and sensitivity modifiers **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool IncFov;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float FOVSen;
};

UCLASS()
class USERLOCSERVER_API AMovableCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovableCamera();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head Tracking (Server)")
        UHeadTracking* HeadTrackingComponent;

	// Sets new property and Camera component. Necessary to use the custom head tracking C++ class.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movable Camera")
		UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    /** Preset array in UE. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        TArray<FCameraPreset> CameraPresets;

    /** Preset reference to Data table **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
        UDataTable* PresetDataTable;

    UFUNCTION(BlueprintCallable) void ChangeCameraSettings(int32 PresetIndex);

   // Start location modifiers in UE.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Transform")
        FVector StartLocation;
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Transform")
        FRotator StartDirection;

    // Rotation modifier in UE.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking")
        bool IncludeRotation;                       // Include rotation estimation boolean.

    // Rotation modifier in UE.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking")
        bool IncludeMovement;                       // Include rotation estimation boolean.
    
    // Movement modifiers XYZ.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Movement")
        float XMovementSensitivity;
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Movement")
        float YMovementSensitivity;
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Movement")
        float ZMovementSensitivity;

    // Rotation modifiers.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Rotation")
        float XRotationSensitivity;                   // Rotation multiplier for X direction.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Rotation")
        float YRotationSensitivity;                   // Rotation multiplier for Y direction.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Rotation")
        float ZRotationSensitivity;                   // Not in use.

    // FOV modifiers.
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|FOV")
        bool FOVEnabled;
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|FOV")
        float FOVSensitivity;

    void LoadPresetsFromDataTable();

private:
    FVector newLocation;

    // Function to calculate FOV
    float FOV(float z);

    float X;
    float Y;
    float Z;

    // Focal length of the camera
    float FocalLength;
    float Scalar_X;
    float Scalar_Y;
    float WidthUE; 
    float HeightUE;

    // Center position of OpenCV Frame
    float CX;
    float CY;


    void UpdatePosition();

    float TranslateX(float x_opencv);
    float TranslateY(float y_opencv);
};
