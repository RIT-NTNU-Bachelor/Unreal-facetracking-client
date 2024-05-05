// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "UDPReceiver.generated.h"

DECLARE_DELEGATE_OneParam(FOnUDPDataReceivedSignature, FString)

UCLASS(ClassGroup = (UUDPReceiver), meta = (BlueprintSpawnableComponent))
class USERLOCSERVER_API UUDPReceiver : public UActorComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	UUDPReceiver();

public:
	void BeginDestroy() override;

	FSocket* Socket;
	// Method to initialize receiving of data.
	bool StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);
	// Handling of UDP data.
	void ReceiveUDPData();

	// Extention of Tick function for an actor, will run in tandem.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnUDPDataReceivedSignature UDPDataReceived;
private:
	uint32 Size;
};