// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "UDPReceiver.h"
#include "HeadTracking.h"
#include "GameFramework/Pawn.h"
#include <iostream>
#include "MovableCamera.generated.h"

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

    // Preset modifier in UE.
   // Todo

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


private:
    FVector newLocation;

    // Function to calculate FOV
    float FOV(float z);

    // Values that needs to be configures for calculations
    float focal_length;
    float center_x_camera;
    float center_y_camera;

    float X;
    float Y;
    float Z;

    void UpdatePosition();

    // Functions for translating the postion of the x and y value correctly 
    float translate_x(float x);
    float translate_y(float y);
};
