// Fill out your copyright notice in the Description page of Project Settings.

#include "JrEngineerSim2017.h"
#include "TCPListenerGameJam.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/IPAddress.h"
#include "../Networking/Public/Common/TcpSocketBuilder.h"
#include "Runtime/Networking/Public/Common/TcpListener.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
ATCPListenerGameJam::ATCPListenerGameJam() {

	PrimaryActorTick.bCanEverTick = true;
}


void ATCPListenerGameJam::BeginPlay() {

	Super::BeginPlay();
}

void ATCPListenerGameJam::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	if (m_listenSocket) {
		m_listenSocket->Close();
	}
	if (m_clientSocket) {
		m_clientSocket->Close();
	}
	if (m_serverSocket) {
		m_serverSocket->Close();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);

	if (m_connectionType == CONNECT_HOST) {
		UpdateHost(DeltaTime);
	}
	else if (m_connectionType == CONNECT_CLIENT) {
		UpdateClient(DeltaTime);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HOSTING
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::BeginHosting(FString& outHostAddr) {

	m_connectionType = CONNECT_HOST;

	TSharedRef<FInternetAddr> bindAddr = GetLocalIP();

	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(bindAddr->ToString(false), IP4Nums)) {
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "Packet received!");
		return;
	}

	//Create Socket
	FIPv4Endpoint endPoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), 11000);

	m_listenSocket = FTcpSocketBuilder(TEXT("HostListenerSocket"))
		.AsReusable()
		.BoundToEndpoint(endPoint)
		.Listening(8);

	int32 newSize = 0;
	m_listenSocket->SetReceiveBufferSize(PACKET_MTU, newSize);
	m_msgBuffer = new uint8[PACKET_MTU];


	FString outAddr = bindAddr->ToString(false);
	outHostAddr = outAddr;
}

//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::CheckForIncomingConnections() {

	TSharedRef<FInternetAddr> clientAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool pendingConnection;
	if (m_listenSocket->HasPendingConnection(pendingConnection)) {

		FSocket* connectionSocket = m_listenSocket->Accept(*clientAddr, TEXT("FTcpListener client"));

		if (connectionSocket != nullptr) {

			OnConnectionAccepted(connectionSocket, FIPv4Endpoint(clientAddr));
			m_hasConnection = true;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::CheckForMessages() {

	TArray<uint8> recvData;

	uint32 size;
	if (m_clientSocket->HasPendingData(size)) {

		int32 bytesRead = 0;
		m_clientSocket->Recv(m_msgBuffer, PACKET_MTU, bytesRead);
		m_msgBufferBytesRead = size;
	}

	if (m_msgBufferBytesRead <= 0) {
		return;
	}


	FString recvMsg = DecodeCurrentMessage();
	m_hasNewPosition = true;
	m_mousePositionSent.InitFromString(recvMsg);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, recvMsg);

	m_msgBufferBytesRead = 0;
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::UpdateHost(float DeltaTime) {

	if (!m_hasConnection) {
		CheckForIncomingConnections();
	}
	else {
		CheckForMessages();
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::HasNewPosition(FVector& position, bool& hasNewPosition) {

	position = m_mousePositionSent;
	bool out = m_hasNewPosition;
	hasNewPosition = out;

	//if (m_hasNewPosition) {
	//	m_hasNewPosition = false;
	//}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLIENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::ConnectToHost(const FString& hostAddr) {

	m_connectionType = CONNECT_CLIENT;

	FString address = hostAddr;
	int32 port = 11000;
	FIPv4Address ip;
	FIPv4Address::Parse(address, ip);

	m_serverSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	bool connected = m_serverSocket->Connect(*addr);

	if (connected) {
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS! Connected to server."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "SUCCESS! Connected to server.");
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Could not connect to server."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "ERROR: Could not connect to server.");
	}

	m_msgBuffer = new uint8[PACKET_MTU];
	return;


	FString serialized = TEXT("Connect|");
	uint32 size = StringToBytes(serialized, m_msgBuffer, PACKET_MTU);
	int32 sent = 0;

	bool successful = m_serverSocket->Send(m_msgBuffer, size, sent);

	if (successful) {
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS! Message sent."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "SUCCESS! Message sent.");
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Could not send message to server."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "ERROR: Could not send message to server.");
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::UpdateClient(float DeltaTime) {

}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::SendPlaceTile(const FVector& pinTilePos) {

	FString posStr = pinTilePos.ToString();
	uint32 size = StringToBytes(posStr, m_msgBuffer, PACKET_MTU);
	int32 sent = 0;

	bool successful = m_serverSocket->Send(m_msgBuffer, size, sent);

	if (successful) {
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS! Message sent."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "SUCCESS! Sent tile position successfully.");
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ERROR: Could not send message to server."));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "ERROR: Could not send tile position to server.");
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
TSharedRef<FInternetAddr> ATCPListenerGameJam::GetLocalIP() {

	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);

	if (localIp->IsValid()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, localIp->ToString(false)); // if you want to append the port (true) or not (false).
	}

	return localIp;
}


//---------------------------------------------------------------------------------------------------------------------------
bool ATCPListenerGameJam::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]) {

	TheIP.Replace(TEXT(" "), TEXT(""));

	TArray<FString> Parts;
	TheIP.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	for (int32 i = 0; i < 4; ++i) {
		Out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}


//---------------------------------------------------------------------------------------------------------------------------
FString ATCPListenerGameJam::DecodeCurrentMessage() const {
	return BytesToString(m_msgBuffer, m_msgBufferBytesRead);
}


//---------------------------------------------------------------------------------------------------------------------------
bool ATCPListenerGameJam::OnConnectionAccepted(FSocket* inSocket, const FIPv4Endpoint& ipAddr) const {

	FString str = "RECEIVED CONNECTION from" + ipAddr.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, str);
	m_clientSocket = inSocket;

	return true;
}