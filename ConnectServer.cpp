#include "pch.h"
#include "ConnectServer.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // ¡¥Ω” Winsock ø‚

ConnectServer::ConnectServer()
{

}

json ConnectServer::Connect(std::string strCode, std::string strDate)
{
	std::string serverIp = "114.55.110.120";
	int port = atoi("8888");
	std::string stockCode = strCode;
	std::string reportDate = strDate;
	// ≥ı ºªØ Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
		std::cerr << "inet_pton failed: invalid IP address" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	std::string request = stockCode + "," + reportDate;
	if (send(sock, request.c_str(), request.size(), 0) == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	char buffer[65536] = { 0 };
	std::string jsonStr;
	int bytes;
	while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
		jsonStr.append(buffer, bytes);
	}
	json j = nullptr;
	try {
		j = json::parse(jsonStr);
	}
	catch (const json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << "\nReceived:\n" << jsonStr << std::endl;
	}
	closesocket(sock);
	WSACleanup();
	return j;
}