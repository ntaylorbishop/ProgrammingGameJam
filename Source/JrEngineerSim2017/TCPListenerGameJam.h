// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "Runtime/Networking/Public/Common/TcpListener.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Runtime/Core/Public/Math/Vector.h"
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
	void CheckForIncomingConnections();
	void CheckForMessages();
	void UpdateHost(float DeltaTime);
	bool OnConnectionAccepted(FSocket* inSocket, const FIPv4Endpoint& ipAddr) const;

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void HasNewPosition(FVector& position, bool& hasNewPosition, int& tileEnum);

	//CLIENT
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ConnectToHost(const FString& hostAddr);
	void UpdateClient(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void SendPlaceTile(const FVector& pinTilePos, int tileEnum);


	
	
	//UTILS
	TSharedRef<FInternetAddr> GetLocalIP();
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);
	FString DecodeCurrentMessage() const;



	eConnectType m_connectionType = CONNECT_NUM_CONNECTS;

	//HOST
	FSocket* m_listenSocket = nullptr;
	mutable FSocket* m_clientSocket = nullptr;
	//FTcpListener listener;
	uint8* m_msgBuffer = nullptr;
	uint32 m_msgBufferBytesRead = 0;

	bool m_hasNewPosition = false;
	FVector m_mousePositionSent;
	int m_tileEnum;

	//CLIENT
	FSocket* m_serverSocket = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
	bool m_hasConnection = false;

protected:
	virtual void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};