// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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

    // Custom function to calculate the skewed frustum projection matrix
    FMatrix GetProjectionMatrix(const FVector& pa,
        const FVector& pb,
        const FVector& pc,
        const FVector& pe,
        float near_clip,
        float far_clip);

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

    // Projection settings
    UPROPERTY(EditAnywhere, Category = "Camera Tweaking|Projection")
        bool ProjectionEnabled;

private:
    FVector newLocation;
    
    // Scene capture component for the camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        USceneCaptureComponent2D* SceneCaptureComponent;

    // Function to calculate FOV
    float FOV(float z);

    float X;
    float Y;
    float Z;

    void UpdatePosition();
};
