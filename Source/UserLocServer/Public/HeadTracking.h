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
class USERLOCSERVER_API UHeadTracking : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    UHeadTracking();

protected:

public:
    // Called when the game starts or when spawned
    bool StartHeadTracking();

    // Called every frame
    void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Sets new property and UDP receiver component. Necessary to use the custom UDPReceiver component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
        UUDPReceiver* UDPReceiverComponent;

    bool GetFaceCoordinates(FVector&);

    // Use smoothing or not when tracking head.
    UPROPERTY(EditAnywhere, Category = "Head Tracking (Server)")
        bool UseSmoothing;                          // Include smoothing of movement boolean.
    UPROPERTY(EditAnywhere, Category = "Head Tracking (Server)|Smoothing")
        int16 SmoothingBufferSize;                  // Set smoothing buffer size, higher equals smoother movement.

    // Z-axis modifiers.
    UPROPERTY(EditAnywhere, Category = "Head Tracking (Server)")
        bool ZAxis;

    // Camera sender modifier.
    UPROPERTY(EditAnywhere, Category = "Head Tracking (Server)")
        FVector CameraCentering;

private:
    // X and Y coordinate lists for average calculation.
    float X;
    float Y;
    float Z;
   
    TArray<float> XList;
    TArray<float> YList;
    TArray<float> ZList;

    // Private functions.
    float CalculateAverage(const TArray<float>& Values);
};