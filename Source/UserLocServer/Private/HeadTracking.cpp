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

void AHeadTracking::BeginPlay()
{
    Super::BeginPlay();
    FString socketName = "localhost";   // Change if not localhost
    FString TheIP = "127.0.0.1";        // Change this to your server's IP address
    int32 ThePort = 5052;               // Change this to your server's port

    // Example: Directly call a method on UDPReceiverComponent
    if (UDPReceiverComponent)
    {
        // Assuming StartUDPReceiver is a method within UUDPReceiver
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

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
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

void AHeadTracking::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateHeadPosition();
}

void AHeadTracking::UpdateHeadPosition()
{
    FString Data = "";

    if (UDPReceiverComponent->ReceiveUDPData(Data))
    {
        // Assuming the data format is "X,Y"
        Data = Data.RightChop(1);
        Data = Data.LeftChop(1);

        TArray<FString> Points;
        Data.ParseIntoArray(Points, TEXT(","), true);

        if (Points.Num() >= 2)
        {
            float X = (FCString::Atof(*Points[0]) - 320) / 100.0f;
            float Y = (FCString::Atof(*Points[1]) - 240) / 100.0f;

            XList.Add(X);
            YList.Add(Y);

            if (XList.Num() > 50) XList.RemoveAt(0);
            if (YList.Num() > 50) YList.RemoveAt(0);

            float XAverage = CalculateAverage(XList);
            float YAverage = CalculateAverage(YList);

            FVector NewPosition = FVector(-26.75f - XAverage, 2.9f - YAverage, CameraComponent->GetComponentLocation().Z);
            //FRotator NewRotation = FRotator(18.76f - YAverage * 10, XAverage * 10, 0.0f);

            CameraComponent->SetRelativeLocation(NewPosition);
            //CameraComponent->SetRelativeRotation(NewRotation);
        }
    }
}

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