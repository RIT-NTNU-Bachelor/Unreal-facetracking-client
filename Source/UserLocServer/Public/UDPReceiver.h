// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UDPReceiver.generated.h"

// Delegate which is called whenever UDP data is received.
DECLARE_DELEGATE_OneParam(FOnUDPDataReceivedSignature, FString)
// Delegate which is called when no UDP data is received.
DECLARE_DELEGATE(FNoUDPDataReceivedSignature)

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
	bool StartUDPReceiver(const FString& SocketName, const FString& TheIP, const int32 ThePort);
	// Handling of UDP data.
	void ReceiveUDPData();

	// Extention of Tick function for an actor, will run in tandem.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Set OOB sensitivity, default 100 ticks.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Tweaking")
		int32 OutOfBoundsSensitivity = 100;                

	// Delegate declarations used for udp data receival or if none is incoming.
	FOnUDPDataReceivedSignature UDPDataReceived;
	FNoUDPDataReceivedSignature NoUDPDataReceived;
private:
	// Private variables
	uint8 OutOfBoundsTick = 0;
	uint32 Size;
	bool bHasPendingData;
};