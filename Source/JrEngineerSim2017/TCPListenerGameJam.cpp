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

	TSharedRef<FInternetAddr> bindAddr = GetLocalIP();

	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(bindAddr->ToString(false), IP4Nums)) {
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "Packet received!");
		return;
	}

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), 11000);
	m_tcpListener = new FTcpListener(Endpoint);
	m_tcpListener->Init();

	FString outAddr = bindAddr->ToString(false);
	outHostAddr = outAddr;
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::UpdateHost(float DeltaTime) {

	m_tcpListener->Run();

	//uint8 recvData[PACKET_MTU];
	//int32 bytesRead = 0;
	//m_listenerSocket->Recv(recvData, PACKET_MTU, bytesRead);
	//
	//if (bytesRead > 0) {
	//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Green, "Packet received!");
	//}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLIENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::ConnectToHost(const FString& hostAddr) {


}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::UpdateClient(float DeltaTime) {

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
FString ATCPListenerGameJam::StringFromBinaryArray(TArray<uint8>& BinaryArray) {
	BinaryArray.Add(0);
	return BytesToString(BinaryArray.GetData(), BinaryArray.Num());
}
