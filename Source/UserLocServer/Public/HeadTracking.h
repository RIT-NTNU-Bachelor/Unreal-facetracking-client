// HeadTrackingActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "UDPReceiver.h"
#include <iostream>

#include "HeadTracking.generated.h"

UCLASS()
class USERLOCSERVER_API AHeadTracking : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AHeadTracking();

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Called every frame
    void Tick(float DeltaTime) override;

    // Sets new property and UDP receiver component. Necessary to use the custom UDPReceiver component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
    UUDPReceiver* UDPReceiverComponent;

    // Sets new property and Camera component. Necessary to use the custom head tracking C++ class.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Head Tracking")
    UCameraComponent* CameraComponent;

    // Preset modifier in UE.
    // Todo

    // Start location modifiers in UE.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        FVector StartLocation;
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        FRotator StartDirection;

    // Rotation modifier in UE.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool IncludeRotation;                       // Include rotation estimation boolean.

    // Use smoothing or not when tracking head.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool UseSmoothing;                          // Include smoothing of movement boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        int16 SmoothingBufferSize;                  // Set smoothing buffer size, higher equals smoother movement.

    // Z-axis modifiers.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool ZAxis;                                 // Include Z axis usage boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float ZMovementSensitivity;

    // Movement modifiers XY.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float XMovementSensitivity;                   
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float YMovementSensitivity;

    // Rotation modifiers.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float XRotationSensitivity;                   // Rotation multiplier for X direction.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float YRotationSensitivity;                   // Rotation multiplier for Y direction.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float ZRotationSensitivity;                   // Not in use.
    
private:
    // X and Y coordinate lists for average calculation.
    float X;
    float Y;
    float Z;
    
    TArray<float> XList;
    TArray<float> YList;
    TArray<float> ZList;

    // Private functions.
    void UpdateHeadPosition();
    float CalculateAverage(const TArray<float>& Values);
};