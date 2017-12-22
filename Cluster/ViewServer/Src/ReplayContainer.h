#pragma once


struct Replay;

class ReplayContainer : public Singleton<ReplayContainer>
{
public:
	Replay* Get(const ReplayID& rID);

	Replay* Insert(const ReplayID& rID);

protected:
private:
	typedef	std::map<ReplayID, Replay>	ReplayMap;

	ReplayMap	m_ReplayMap;
};
extern ReplayContainer& gReplays;