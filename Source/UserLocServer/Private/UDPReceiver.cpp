// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPReceiver.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"

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
        
        if (!Socket->HasPendingData(Size) && OutOfBoundsTick > OutOfBoundsSensitivity)
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

    Returns boolean if it successfully started: true, false if not.
*/
bool UUDPReceiver::StartUDPReceiver(const FString& socketName, const FString& TheIP, const int32 ThePort)
{
    FIPv4Address Addr;
    // Parses into format 0.0.0.0:0000
    FIPv4Address::Parse(TheIP, Addr);
    // Creates an endpoint based on the address and port.
    FIPv4Endpoint Endpoint(Addr, ThePort);

    // BUFFER SIZE
    int32 BufferSize = 2 * 1024 * 1024;

    // Build Socket
    Socket = FUdpSocketBuilder(*socketName)
        .AsNonBlocking()
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(BufferSize);
    
    // Checks if socket is valid.
    bool bIsValidSocket = Socket != nullptr;    
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
    // Initialize the array with zeros, with a size that is the minimum between 'Size' and 65507
    ReceivedData.Init(0, FMath::Min(Size, 65507u));
    // Variable to store the number of bytes read from the UDP socket
    int32 Read = 0;
    // Receive data from the UDP socket and store it in the 'ReceivedData' array
    Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
    if (Read > 0)
    {
        // Receive data from the UDP socket and store it in the 'ReceivedData' array. Calls delegate when converted.
        UDPDataReceived.Execute(FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData()))));
    }
}