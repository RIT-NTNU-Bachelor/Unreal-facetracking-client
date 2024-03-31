// HeadTrackingActor.cpp

#include "HeadTracking.h"

UHeadTracking::UHeadTracking()
{
    // Sets it to be able to tick with world and actor.
    PrimaryComponentTick.bCanEverTick = true;
    // Allows any parent to destroy the instance.
    bAllowAnyoneToDestroyMe = true;

    // Create and attach the UDPReceiver component
    UDPReceiverComponent = CreateDefaultSubobject<UUDPReceiver>(TEXT("UDPReceiverComponent"));
}

/*
* Runs when the game plays.
*/
bool UHeadTracking::StartHeadTracking()
{
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
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to start UDP Receiver."));
        }
    }
    return false;
}

/*
* Every tick in the game, this function will be run.
* It updated the head position using the method: UpdateHeadPosition.
*/
void UHeadTracking::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    FVector newLocation;
    UpdateHeadPosition(newLocation);
}

/*
* Updates head position based on the UDP component data.
*/
bool UHeadTracking::UpdateHeadPosition(FVector& newLocation)
{
    FString Data = "";

    if (UDPReceiverComponent->ReceiveUDPData(Data))
    {
        if (Data.IsEmpty())
        {
            return false;
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

                newLocation = FVector(X, Y, Z);
            }
            return true;
        } 
    }
    return false;
}

/*
* Simple average calculation.
*/
float UHeadTracking::CalculateAverage(const TArray<float>& Values)
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