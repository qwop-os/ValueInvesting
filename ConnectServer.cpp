#include "pch.h"
#include "ConnectServer.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // 链接 Winsock 库

ConnectServer::ConnectServer()
{

}

json ConnectServer::Connect(std::string strCode, std::string strDate)
{
	std::string serverIp = "114.55.110.120";
	int port = atoi("8888");
	std::string stockCode = strCode;
	std::string reportDate = strDate;
	// 初始化 Winsock
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

// 向服务端发送 JSON 数据（上传）
void ConnectServer::SendJsonToServer(const json& data)
{
	// 仅当有 balance_sheet 节点且 data_available 为 true 时才上传
	if (!data.contains("balance_sheet") ||
		!data["balance_sheet"].value("data_available", false))
	{
		//AfxMessageBox(_T("该 JSON 不包含有效财报数据，未上传"));
		return;
	}

	std::string uploadStr = "UPLOAD:" + data.dump();

	CString serverIP = "114.55.110.120";  
	int port = 8888;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		//AfxMessageBox(_T("网络初始化失败"));
		return;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
		//AfxMessageBox(_T("创建套接字失败"));
		return;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, CT2A(serverIP), &serverAddr.sin_addr);

	if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(sock);
		WSACleanup();
		//AfxMessageBox(_T("连接服务器失败"));
		return;
	}

	// 发送上传数据
	int sent = send(sock, uploadStr.c_str(), uploadStr.size(), 0);
	if (sent == SOCKET_ERROR) {
		closesocket(sock);
		WSACleanup();
		//AfxMessageBox(_T("数据发送失败"));
		return;
	}

	// 接收服务端响应
	char buffer[512] = { 0 };
	int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
	//if (bytes > 0) {
	//	std::string response(buffer, bytes);
	//	CString msg;
	//	msg.Format(_T("服务器响应: %s"), CA2W(response.c_str(), CP_UTF8));
	//	AfxMessageBox(msg);
	//}

	closesocket(sock);
	WSACleanup();
}