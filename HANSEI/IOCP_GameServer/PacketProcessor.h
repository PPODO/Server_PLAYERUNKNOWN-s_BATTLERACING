#pragma once
#include "DataBase.h"
#include <functional>
#include <utility>
#include <vector>
#include <random>
#include <chrono>
#include <queue>
#include <tuple>
#include <mutex>
#include <list>
#include <map>

const float ItemRespawnTime = 5.f;

typedef std::function<void(SOCKETINFO*, struct GAMEPACKET*&)> Processor;

class PacketProcessor {
	enum { ETP_LASTTIME, ETP_DELAY, ETP_SESSIONID, ETP_PACKET };
	enum { ESI_UNIQUEKEY, ESI_PLAYERINFORMATION };
private:
	std::mutex m_Lock;
	DataBase* m_DataBase;
	class IOCP* m_Server;

private:
	std::vector<Processor> m_Processor;
	std::map<int, std::tuple<int, std::vector<struct GAMEPACKET>>> m_Sessions;

private:
	bool m_bIsStop;
	std::thread m_TimerThread;
	std::random_device m_RandomDevice;
	std::mt19937_64 m_RandomAlgorithm;
	std::list<std::tuple<std::chrono::system_clock::time_point, std::chrono::duration<float>, int, struct PACKET*>> m_TimerList;

public:
	PacketProcessor(class IOCP* Server);
	~PacketProcessor();

private:
	void JoinGame(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void UpdatePlayerInformation(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void DisconnectPlayer(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void StartGame(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void AddNewSpawnTimer(SOCKETINFO* Info, struct GAMEPACKET*& Packet);

private:
	void BroadCast(PACKET* Packet, const std::vector<struct GAMEPACKET>& PlayerList);
	void BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, struct GAMEPACKET*& Packet);

public:
	Processor& operator[](const EPACKETMESSAGETYPE& MessageType) {
		return m_Processor[static_cast<size_t>(MessageType)];
	}

};