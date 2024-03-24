// HeadTrackingActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include <iostream>
#include "EngineUtils.h"
#include "UDPReceiver.h"

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
    UUDPReceiver* UDPReceiverComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head Tracking")
    UCameraComponent* CameraComponent;
    // Standard spawn location for the AHeadTracking pawn
    FVector SpawnLocation = FVector(-100.0f, 0.0f, 150.0f); // Default to origin

    // Standard spawn rotation for the AHeadTracking pawn
    FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f); // Default to no rotation

private:
    TArray<float> XList;
    TArray<float> YList;

    void UpdateHeadPosition();
    float CalculateAverage(const TArray<float>& Values);
};