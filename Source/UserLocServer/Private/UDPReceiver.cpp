// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPReceiver.h"

// Sets default values
UUDPReceiver::UUDPReceiver()
{
    // Set this component to be initialized when the game starts, and to tick every frame. 
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UUDPReceiver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Continuous operation of UDP receiver
    if (Socket)
    {
        FString SerializedData = "";
        ReceiveUDPData(SerializedData);
        if (!SerializedData.IsEmpty())
        {
            UE_LOG(LogTemp, Display, TEXT("UDP: Received: %s"), *SerializedData);
        }
    }
}


bool UUDPReceiver::StartUDPReceiver(const FString& socketName, const FString& TheIP, const int32 ThePort)
{
    FIPv4Address Addr;
    FIPv4Address::Parse(TheIP, Addr);

    FIPv4Endpoint Endpoint(Addr, ThePort);

    //BUFFER SIZE
    int32 BufferSize = 2 * 1024 * 1024;

    //Create Socket
    Socket = FUdpSocketBuilder(*socketName)
        .AsNonBlocking()
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(BufferSize);

    bool bIsValidSocket = Socket != nullptr;

    UE_LOG(LogTemp, Display, TEXT("UDP: Socket creation %s"), bIsValidSocket ? TEXT("succeeded") : TEXT("failed"));

    return bIsValidSocket;
}

bool UUDPReceiver::ReceiveUDPData(FString& OutReceivedData)
{
    if (!Socket)
    {
        UE_LOG(LogTemp, Error, TEXT("UDP: Failed to create listener socket!"));
        return false;
    }

    TArray<uint8> ReceivedData;
    uint32 Size;
    while (Socket->HasPendingData(Size))
    {
        ReceivedData.Init(0, FMath::Min(Size, 65507u));

        int32 Read = 0;
        Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

        if (Read > 0)
        {
            OutReceivedData = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
            return true;
        }
    }

    return false;
}