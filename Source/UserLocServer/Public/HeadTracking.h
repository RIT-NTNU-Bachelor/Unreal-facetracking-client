// HeadTrackingActor.h

#pragma once
#include "UDPReceiver.h"
#include "Engine/DataTable.h"
#include "HeadTracking.generated.h"

// Delegate which is called whenever the face coordinate has changed.
DECLARE_DELEGATE_OneParam(FOnFaceMoved, FVector)

/*
    Struct used for Head Tracking presets.
    Each struct is considered a row in a Data Table using this as base.
*/
USTRUCT(BlueprintType)
struct FHeadTrackingPresets : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool SmoothingBool;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 SmoothingAmount;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool ZBool;
};

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

    // Sets new property and UDP receiver component. Necessary to use the custom UDPReceiver component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
        UUDPReceiver* UDPReceiverComponent;

    // Function to extract face coordinates from UDP data packets.
    void ExtractFaceCoordinateData(FString Data);

    // Use smoothing or not when tracking head.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head Tracking (Server)")
        bool UseSmoothing;                          // Include smoothing of movement boolean.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head Tracking (Server)|Smoothing")
        int32 SmoothingBufferSize;                  // Set smoothing buffer size, higher equals smoother movement.

    // Z-axis modifiers.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head Tracking (Server)")
        bool ZAxis;

    UFUNCTION(BlueprintCallable) void ChangeHeadTrackingPreset(FHeadTrackingPresets Preset);

    // Camera sender modifier.
    UPROPERTY(EditAnywhere, Category = "Head Tracking (Server)")
        FVector CameraCentering;

    // Delegate signature
    FOnFaceMoved OnFaceMoved;

    // Boolean for testing latency, if true it expects another value from python server: send index (0..N).
    bool bLatencyTesting = true;
    float SendIndex;
private:
    float X;
    float Y;
    float Z;
    
    // X and Y coordinate lists for average calculation.
    TArray<float> XList;
    TArray<float> YList;
    TArray<float> ZList;

    // Private functions.
    float CalculateAverage(const TArray<float>& Values);
};