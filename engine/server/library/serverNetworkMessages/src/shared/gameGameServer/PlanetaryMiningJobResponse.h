// ======================================================================
//
// PlanetaryMiningJobResponse.h
//
// ======================================================================

#ifndef INCLUDED_PlanetaryMiningJobResponse_H
#define INCLUDED_PlanetaryMiningJobResponse_H

#include "sharedFoundation/NetworkId.h"
#include "sharedNetworkMessages/GameNetworkMessage.h"

class PlanetaryMiningJobResponse : public GameNetworkMessage
{
public:
	PlanetaryMiningJobResponse(NetworkId const &characterId, NetworkId const &callbackTarget, int jobSequence, bool reserve, int result, int jobCount);
	explicit PlanetaryMiningJobResponse(Archive::ReadIterator &source);
	~PlanetaryMiningJobResponse();

	NetworkId const &getCharacterId() const;
	NetworkId const &getCallbackTarget() const;
	int getJobSequence() const;
	bool getReserve() const;
	int getResult() const;
	int getJobCount() const;

private:
	Archive::AutoVariable<NetworkId> m_characterId;
	Archive::AutoVariable<NetworkId> m_callbackTarget;
	Archive::AutoVariable<int> m_jobSequence;
	Archive::AutoVariable<bool> m_reserve;
	Archive::AutoVariable<int> m_result;
	Archive::AutoVariable<int> m_jobCount;

private:
	PlanetaryMiningJobResponse(PlanetaryMiningJobResponse const &);
	PlanetaryMiningJobResponse &operator=(PlanetaryMiningJobResponse const &);
};

inline NetworkId const &PlanetaryMiningJobResponse::getCharacterId() const
{
	return m_characterId.get();
}

inline NetworkId const &PlanetaryMiningJobResponse::getCallbackTarget() const
{
	return m_callbackTarget.get();
}

inline int PlanetaryMiningJobResponse::getJobSequence() const
{
	return m_jobSequence.get();
}

inline bool PlanetaryMiningJobResponse::getReserve() const
{
	return m_reserve.get();
}

inline int PlanetaryMiningJobResponse::getResult() const
{
	return m_result.get();
}

inline int PlanetaryMiningJobResponse::getJobCount() const
{
	return m_jobCount.get();
}

#endif
