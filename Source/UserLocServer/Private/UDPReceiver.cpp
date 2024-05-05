// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPReceiver.h"

/*
    Constuctor of UDP Receiver.
    Initializes important values.
*/
UUDPReceiver::UUDPReceiver()
{
    // Sets it to be able to tick with world and actor.
    PrimaryComponentTick.bCanEverTick = true;
    // Allows any parent to destroy the instance.
    bAllowAnyoneToDestroyMe = true;
}

/*
    Activates when the game is stopped. Resets the component.
*/
void UUDPReceiver::BeginDestroy()
{
    Super::BeginDestroy();

    // Ensures the socket is properly closed
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
}

/*
    Called every frame, as an extention of Tick for an Actor.
*/ 
void UUDPReceiver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!Socket) 
    {
        UE_LOG(LogTemp, Error, TEXT("UDP: Failed to create listener socket!"));
        return;
    }
    else {
        // Continuous operation of UDP receiver
        while (Socket->HasPendingData(Size))
        {
            ReceiveUDPData();
        }
        
        if (!Socket->HasPendingData(Size) && OutOfBoundsTick > 100)
        {
            // Activates out of bounds message.
            NoUDPDataReceived.Execute();
            OutOfBoundsTick = 0;
        }
        OutOfBoundsTick++;
    }
}

/*
    Starts UDP Receiving.
    Takes in socketName, IP and Port.
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
    Function to collect data.
    Expects FString reference and sets the pointer based on UDP data.
*/
void UUDPReceiver::ReceiveUDPData()
{
    TArray<uint8> ReceivedData;
    ReceivedData.Init(0, FMath::Min(Size, 65507u));
    int32 Read = 0;
    Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
    if (Read > 0)
    {
        UDPDataReceived.Execute(FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData()))));
    }
}