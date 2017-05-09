// Fill out your copyright notice in the Description page of Project Settings.

#include "JrEngineerSim2017.h"
#include "TCPListenerGameJam.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Sockets/Public/IPAddress.h"
#include "../Networking/Public/Common/TcpSocketBuilder.h"


ATCPListenerGameJam::ATCPListenerGameJam() {

	PrimaryActorTick.bCanEverTick = true;

	if (!StartTCPReceiver("RamaSocketListener", "127.0.0.1", 8890)) {
		return;
	}
}


void ATCPListenerGameJam::BeginPlay() {

	Super::BeginPlay();
}


void ATCPListenerGameJam::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);
	TCPConnectionListener();
}


//---------------------------------------------------------------------------------------------------------------------------
bool ATCPListenerGameJam::StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort) {

	ListenerSocket = CreateTCPConnectionListener(YourChosenSocketName, TheIP, ThePort);

	if (!ListenerSocket) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *TheIP, ThePort));
		return false;
	}

	return true;
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
FSocket* ATCPListenerGameJam::CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(TheIP, IP4Nums)) {
		//VShow("Invalid IP! Expecting 4 parts separated by .");
		return false;
	}

	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
	FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	return ListenSocket;
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::TCPConnectionListener() {

	if (!ListenerSocket) 
		return;

	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	if (ListenerSocket->HasPendingConnection(Pending) && Pending) {

		if (ConnectionSocket) {
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}

		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));

		if (ConnectionSocket != NULL) {

			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
			//GetWorldTimerManager().SetTimer(this, &ATCPListenerGameJam::TCPSocketListener, 0.01, true);
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
FString ATCPListenerGameJam::StringFromBinaryArray(TArray<uint8>& BinaryArray) {
	BinaryArray.Add(0);
	return BytesToString(BinaryArray.GetData(), BinaryArray.Num());
}


//---------------------------------------------------------------------------------------------------------------------------
void ATCPListenerGameJam::TCPSocketListener() {

	if (!ConnectionSocket) 
		return;

	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size)) {
		ReceivedData.Add(FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);


	}

	if (ReceivedData.Num() <= 0) {
		return;
	}

	//VShow("Total Data read!", ReceivedData.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	//VShow("As String!!!!! ~>", ReceivedUE4String);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
}