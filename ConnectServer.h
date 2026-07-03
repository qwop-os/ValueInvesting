#pragma once
#include "json.hpp"
#include <string.h>

using namespace std;
using json = nlohmann::json;
class ConnectServer
{
public:
	ConnectServer();
	json Connect(std::string strCode, std::string strDate);
	void SendJsonToServer(const json& data);
};