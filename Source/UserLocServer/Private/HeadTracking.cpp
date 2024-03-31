// HeadTrackingActor.cpp

#include "HeadTracking.h"

AHeadTracking::AHeadTracking()
{
    // Set this actor to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    StartLocation = FVector(0.0f, 0.0f, 0.0f);
    StartDirection = FRotator(0.0f, 0.0f, 0.0f);

    IncludeRotation = true;
    UseSmoothing = true;
    ZAxis = true;

    FOVEnabled = true;
    FOVSensitivity = 3.0f;

    SmoothingBufferSize = 5;

    XMovementSensitivity = 1.0f;
    YMovementSensitivity = 1.0f;
    ZMovementSensitivity = 1.0f;

    XRotationSensitivity = 0.1f;
    YRotationSensitivity = 0.1f;
    ZRotationSensitivity = 0.0f;

    // Create and attach the UDPReceiver component
    UDPReceiverComponent = CreateDefaultSubobject<UUDPReceiver>(TEXT("UDPReceiverComponent"));

    // Create and setup the camera component as a subcomponent.
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    RootComponent = CameraComponent;
}

/*
* Runs when the game plays.
*/
void AHeadTracking::BeginPlay()
{
    Super::BeginPlay();

    FString socketName = "localhost";   // Not necessary to change.
    FString TheIP = "127.0.0.1";        // Change this to your server's IP address.
    int32 ThePort = 5052;               // Change this to your server's port.

    // If the UDPReceiverComponent is initialized, do this..
    if (UDPReceiverComponent)
    {
        // StartUDPReceiver is a method within UUDPReceiver
        bool bStarted = UDPReceiverComponent->StartUDPReceiver(socketName, TheIP, ThePort);
        if (bStarted)
        {
            UE_LOG(LogTemp, Log, TEXT("UDP Receiver started successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to start UDP Receiver."));
        }
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); // Retrieves the FP controller.
    // If FP controller is found, set new actor location and rotation.
    if (PlayerController) 
    {
        APawn* CurrentPawn = PlayerController->GetPawn();

        // Check if the current pawn is not null and is of type AHeadTracking
        AHeadTracking* HeadTrackingPawn = Cast<AHeadTracking>(CurrentPawn);
        if (HeadTrackingPawn != nullptr)
        {
            // If it's already a HeadTrackingPawn, just move it to the standard location and rotation
            HeadTrackingPawn->SetActorLocation(StartLocation);
            HeadTrackingPawn->SetActorRotation(StartDirection);
        }
    }
}

/*
* Every tick in the game, this function will be run.
* It updated the head position using the method: UpdateHeadPosition.
*/
void AHeadTracking::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateHeadPosition();
}

/*
* Updates head position based on the UDP component data.
*/
void AHeadTracking::UpdateHeadPosition()
{
    FString Data = "";
    FVector LastKnownPosition = StartLocation;
    FRotator LastKnownRotation = StartDirection;
    float FOVmax = 120;
    float FOVmin = 85;

    if (UDPReceiverComponent->ReceiveUDPData(Data))
    {
        if (Data.IsEmpty())
        {
            return;
        }

        // Assuming the data format is: '(X,Y)'. There is a b first, but not read in this context.
        Data = Data.RightChop(1);   // Removes '(
        Data = Data.LeftChop(1);    // Removes )'

        TArray<FString> Points;
        Data.ParseIntoArray(Points, TEXT(","), true);
        
        if (Points.Num() >= 2) // 2 or more points: x, y, may also include z.
        {
            X = (FCString::Atof(*Points[0]) - 320.0f); // Parses X into float from FCString.
            Y = (FCString::Atof(*Points[1]) - 280.0f); // Parses Y into float from FCString.
            if (ZAxis) Z = (FCString::Atof(*Points[2]) - 60.0f); // Parses Z into float from FCString.

            // Adds the data to a list to find average of X and Y. Smooths the movement.
            if (UseSmoothing)
            {
                XList.Add(X);
                YList.Add(Y);
                if (ZAxis) ZList.Add(Z);

                if (XList.Num() > SmoothingBufferSize) XList.RemoveAt(0);
                if (YList.Num() > SmoothingBufferSize) YList.RemoveAt(0);
                if (ZAxis && ZList.Num() > SmoothingBufferSize) ZList.RemoveAt(0);

                X = CalculateAverage(XList);
                Y = CalculateAverage(YList);
                if (ZAxis) Z = CalculateAverage(ZList);
            }
            
            // New position of the camera after handling as FVector, the standard format of coordinates.
            LastKnownPosition = StartLocation + FVector((- X * XMovementSensitivity), (Z * ZMovementSensitivity), (-Y * YMovementSensitivity));
            CameraComponent->SetRelativeLocation(LastKnownPosition); // Sets new position in the world.
            
            // Option to remove rotation aspect of camera movement in UE.
            if (IncludeRotation)
            {
                // New position of the camera after handling as FRotator, the standard format of rotation.
                LastKnownRotation = StartDirection + FRotator(Y * YRotationSensitivity, X * XRotationSensitivity, 0.0f * ZRotationSensitivity);
                CameraComponent->SetWorldRotation(LastKnownRotation); // Sets new rotation relative to parent.
            }
            // Option to include or remove fov.
            if (ZAxis && FOVEnabled)
            {
                float ZFov = (Z / FOVSensitivity) + 90.0f;

                if (ZFov < FOVmin)
                {
                    ZFov = FOVmin;
                }
                else if (ZFov > FOVmax)
                {
                    ZFov = FOVmax;
                }
                CameraComponent->SetFieldOfView(ZFov);
            }
        } 
    }
}

/*
* Simple average calculation.
*/
float AHeadTracking::CalculateAverage(const TArray<float>& Values)
{
    if (Values.Num() == 0)
    {
        return 0.0f;
    }

    float Sum = 0.0f;
    for (float Val : Values)
    {
        Sum += Val;
    }
    return Sum / Values.Num();
}