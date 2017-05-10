// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "Runtime/Networking/Public/Common/TcpListener.h"
#include "TCPListenerGameJam.generated.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ENUMS CONSTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
enum eConnectType {
	CONNECT_HOST = 0,
	CONNECT_CLIENT,
	CONNECT_NUM_CONNECTS
};

const unsigned int PACKET_MTU = 1024;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TCPLISTENER CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
UCLASS()
class JRENGINEERSIM2017_API ATCPListenerGameJam : public AActor
{
	GENERATED_BODY()
	
public:	
	//STRUCTORS
	ATCPListenerGameJam();

	//UPDATE
	virtual void Tick(float DeltaTime) override;


	//HOST
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void BeginHosting(FString& outHostAddr);
	void UpdateHost(float DeltaTime);

	//CLIENT
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ConnectToHost(const FString& hostAddr);
	void UpdateClient(float DeltaTime);


	//MESSAGE PASSING
	void ListenToMessage();
	
	
	//UTILS
	TSharedRef<FInternetAddr> GetLocalIP();
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);



	eConnectType m_connectionType = CONNECT_NUM_CONNECTS;
	FTcpListener* m_tcpListener = nullptr;

	FSocket* m_connectionSocket;
	FSocket* m_listenerSocket;


	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

protected:
	virtual void BeginPlay() override;	
};