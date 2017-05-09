// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "TCPListenerGameJam.generated.h"



UCLASS()
class JRENGINEERSIM2017_API ATCPListenerGameJam : public AActor
{
	GENERATED_BODY()
	
public:	
	ATCPListenerGameJam();
	virtual void Tick(float DeltaTime) override;

	bool StartTCPReceiver(const FString& YourChosenSocketName,const FString& TheIP,const int32 ThePort);
	FSocket* CreateTCPConnectionListener(const FString& YourChosenSocketName,const FString& TheIP,const int32 ThePort,const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	void TCPConnectionListener();
	void TCPSocketListener();
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);


	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

protected:
	virtual void BeginPlay() override;	
};