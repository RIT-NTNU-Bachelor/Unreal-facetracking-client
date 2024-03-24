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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head Tracking")
    UCameraComponent* CameraComponent;

    // Standard spawn location & rotation, for the AHeadTracking pawn
    FVector SpawnLocation = FVector(0.0f, 750.0f, 150.0f); // Default to origin
    FRotator SpawnRotation = FRotator(0.0f, -90.0f, 0.0f); // Default to no rotation

private:
    // X and Y coordinate lists for average calculation.
    TArray<float> XList;
    TArray<float> YList;

    const int8 bufferSize = 20; // Size of the buffer: coordinate data.

    // Private functions.
    void UpdateHeadPosition();
    float CalculateAverage(const TArray<float>& Values);
};