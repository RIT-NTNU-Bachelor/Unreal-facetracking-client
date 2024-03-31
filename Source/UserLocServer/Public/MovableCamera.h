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
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head Tracking")
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
    UPROPERTY(EditAnywhere, Category = "Tweaking|Transform")
        FVector StartLocation;
    UPROPERTY(EditAnywhere, Category = "Tweaking|Transform")
        FRotator StartDirection;

    // Rotation modifier in UE.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool IncludeRotation;                       // Include rotation estimation boolean.

    // Use smoothing or not when tracking head.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Smoothing")
        bool UseSmoothing;                          // Include smoothing of movement boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Smoothing")
        int16 SmoothingBufferSize;                  // Set smoothing buffer size, higher equals smoother movement.

    // Z-axis modifiers.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool ZAxis;                                 // Include Z axis usage boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Movement")
        float ZMovementSensitivity;

    // Movement modifiers XY.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Movement")
        float XMovementSensitivity;
    UPROPERTY(EditAnywhere, Category = "Tweaking|Movement")
        float YMovementSensitivity;

    // Rotation modifiers.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Rotation")
        float XRotationSensitivity;                   // Rotation multiplier for X direction.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Rotation")
        float YRotationSensitivity;                   // Rotation multiplier for Y direction.
    UPROPERTY(EditAnywhere, Category = "Tweaking|Rotation")
        float ZRotationSensitivity;                   // Not in use.

    // FOV modifiers.
    UPROPERTY(EditAnywhere, Category = "Tweaking|FOV")
        bool FOVEnabled;
    UPROPERTY(EditAnywhere, Category = "Tweaking|FOV")
        float FOVSensitivity;

private:
    void UpdatePosition();
};
