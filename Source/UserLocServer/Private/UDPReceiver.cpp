// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPReceiver.h"

UUDPReceiver::UUDPReceiver()
{
    // Sets it to be able to tick with world and actor.
    PrimaryComponentTick.bCanEverTick = true;
}

/*
* Called every frame, as an extention of Tick for an Actor.
*/ 
void UUDPReceiver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Continuous operation of UDP receiver
    if (Socket)
    {
        FString SerializedData = "";
        ReceiveUDPData(SerializedData);
    }
}

/*
* Starts UDP Receiving.
* Takes in socketName, IP and Port.
*/
bool UUDPReceiver::StartUDPReceiver(const FString& socketName, const FString& TheIP, const int32 ThePort)
{
    FIPv4Address Addr;
    FIPv4Address::Parse(TheIP, Addr);   // Parses into format 0.0.0.0:0000

    FIPv4Endpoint Endpoint(Addr, ThePort);

    // BUFFER SIZE
    int32 BufferSize = 2 * 1024 * 1024;

    // Build Socket
    Socket = FUdpSocketBuilder(*socketName)
        .AsNonBlocking()
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(BufferSize);

    bool bIsValidSocket = Socket != nullptr;    // Checks if socket is valid.
    UE_LOG(LogTemp, Display, TEXT("UDP: Socket creation %s"), bIsValidSocket ? TEXT("succeeded") : TEXT("failed"));

    return bIsValidSocket;
}

/*
* Function to collect data.
* Expects FString reference and sets the pointer based on UDP data.
*/
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