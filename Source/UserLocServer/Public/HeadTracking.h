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

    // Modifiers in UE.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool IncludeRotation;                       // Include rotation estimation boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool UseSmoothing;                          // Include smoothing of movement boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        bool ZAxis;                                 // Include Z axis usage boolean.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        int16 SmoothingBufferSize;                  // Set smoothing buffer size, higher equals smoother movement.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float MultiplierMovement;                   // Amount of movement multiplier.
    UPROPERTY(EditAnywhere, Category = "Tweaking")
        float MultiplierRotation;                   // Rotation multiplier.
    
    // Standard spawn location & rotation, for the AHeadTracking pawn
    FVector SpawnLocation = FVector(0.0f, 0.0f, 150.0f); // Default to origin
    FRotator SpawnRotation = FRotator(0.0f, -90.0f, 0.0f); // Default to no rotation

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