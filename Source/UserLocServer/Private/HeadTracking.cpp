// HeadTrackingActor.cpp

#include "HeadTracking.h"

AHeadTracking::AHeadTracking()
{
    // Set this actor to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

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
            HeadTrackingPawn->SetActorLocation(SpawnLocation);
            HeadTrackingPawn->SetActorRotation(SpawnRotation);
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

    if (UDPReceiverComponent->ReceiveUDPData(Data))
    {
        // Assuming the data format is: '(X,Y)'. There is a b first, but not read in this context.
        Data = Data.RightChop(2);   // Removes '(
        Data = Data.LeftChop(2);    // Removes )'

        TArray<FString> Points;
        Data.ParseIntoArray(Points, TEXT(","), true);

        if (Points.Num() >= 2) // 2 or more points: x, y, may also include z.
        {
            float X = (FCString::Atof(*Points[0])); // Parses X into float from FCString.
            float Y = (FCString::Atof(*Points[1])); // Parses Y into float from FCString.

            // Adds the data to a list to find average of X and Y. Smooths the movement.
            XList.Add(X);
            YList.Add(Y);
            if (XList.Num() > bufferSize) XList.RemoveAt(0);
            if (YList.Num() > bufferSize) YList.RemoveAt(0);
            float XAverage = CalculateAverage(XList);
            float YAverage = CalculateAverage(YList);
            
            // New position of the camera after handling as FVector, the standard format of coordinates.
            FVector NewPosition = FVector(X * 5.0f, CameraComponent->GetComponentLocation().Z, Y * 5.0f);
            // New position of the camera after handling as FRotator, the standard format of rotation.
            FRotator NewRotation = FRotator(YAverage, XAverage, 0.0f);

            CameraComponent->SetRelativeLocation(NewPosition); // Sets new position.
            CameraComponent->SetRelativeRotation(NewRotation); // Sets new rotation.
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