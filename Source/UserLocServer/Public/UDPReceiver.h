// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "UDPReceiver.generated.h"

UCLASS(ClassGroup = (UUDPReceiver), meta = (BlueprintSpawnableComponent))
class USERLOCSERVER_API UUDPReceiver : public UActorComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	UUDPReceiver();

public:
	FSocket* Socket;

	bool StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);

	bool ReceiveUDPData(FString& OutReceivedData);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};