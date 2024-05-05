// HeadTrackingActor.cpp

#include "HeadTracking.h"

UHeadTracking::UHeadTracking()
{
    // Sets it to be able to tick with world and actor.
    PrimaryComponentTick.bCanEverTick = true;
    // Allows any parent to destroy the instance.
    bAllowAnyoneToDestroyMe = true;

    CameraCentering = FVector(320.0f, 280.0f, 60.0f);

    // Create and attach the UDPReceiver component
    UDPReceiverComponent = CreateDefaultSubobject<UUDPReceiver>(TEXT("UDPReceiverComponent"));
    UDPReceiverComponent->UDPDataReceived.BindUObject(this, &UHeadTracking::ExtractFaceCoordinateData);
}


/*
* Runs when the game plays.
*/
bool UHeadTracking::StartHeadTracking()
{
    FString socketName = "localhost";   // Not necessary to change.
    FString TheIP = "127.0.0.1";        // Change this to your server's IP address.
    int32 ThePort = 5052;               // Change this to your server's port.

    // If the UDPReceiverComponent is initialized
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


void UHeadTracking::ChangeHeadTrackingPreset(FHeadTrackingPresets Preset)
{
    UseSmoothing = Preset.SmoothingBool;
    SmoothingBufferSize = Preset.SmoothingAmount;
    ZAxis = Preset.ZBool;
}


/*
* Updates head position based on the UDP component data.
*/
void UHeadTracking::ExtractFaceCoordinateData(FString Data)
{    
   // Data should be have retrived, but if not the tracking component does nothing 
   if (Data.IsEmpty())
   {
       return;
   }

   // Assuming the data format is: '(X,Y,Z)' 
   // Gets the data as bytes 
   Data = Data.RightChop(1).LeftChop(1);    // Removes '( )'

   // Parse the data into a list of points 
   TArray<FString> Points;
   
   Data.ParseIntoArray(Points, TEXT(","), true);
   
   if (Points.Num() >= 2) // 2 or more points: x, y, may also include z.
   {
       try
       {
           X = (FCString::Atof(*Points[0]) - CameraCentering.X); // Parses X into float from FCString.
           Y = (FCString::Atof(*Points[1]) - CameraCentering.Y); // Parses Y into float from FCString.
           Z = (Points.Num() > 2 && ZAxis) ? (FCString::Atof(*Points[2]) - CameraCentering.Z) : 0.0f;
       }
       catch (const std::exception&)
       {
           UE_LOG(LogTemp, Error, TEXT("Not correct format."));
           return;
       }

       //  Adds the data to a list to find average of X and Y. Smooths the movement.
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
           Z = ZAxis ? CalculateAverage(ZList) : Z;
       }
       OnFaceMoved.Execute(FVector(X, Y, Z));
       //UE_LOG(LogTemp, Error, TEXT("X: %f, Y: %f, Z: %f"), X, Y, Z)        
   }
}

/*
* Simple average calculation.
*/
float UHeadTracking::CalculateAverage(const TArray<float>& Values)
{
    if (Values.IsEmpty()) return 0.0f;
    float Sum = 0.0f;
    for (float Val : Values) Sum += Val;
    return Sum / Values.Num();
}