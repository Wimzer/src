// ======================================================================
//
// PlanetaryMiningJobRequest.h
//
// ======================================================================

#ifndef INCLUDED_PlanetaryMiningJobRequest_H
#define INCLUDED_PlanetaryMiningJobRequest_H

#include "sharedFoundation/NetworkId.h"
#include "sharedNetworkMessages/GameNetworkMessage.h"

class PlanetaryMiningJobRequest : public GameNetworkMessage
{
public:
	PlanetaryMiningJobRequest(NetworkId const &characterId, NetworkId const &callbackTarget, uint32 stationId, std::string const &galaxyId, int jobSequence, bool reserve);
	explicit PlanetaryMiningJobRequest(Archive::ReadIterator &source);
	~PlanetaryMiningJobRequest();

	NetworkId const &getCharacterId() const;
	NetworkId const &getCallbackTarget() const;
	uint32 getStationId() const;
	std::string const &getGalaxyId() const;
	int getJobSequence() const;
	bool getReserve() const;

private:
	Archive::AutoVariable<NetworkId> m_characterId;
	Archive::AutoVariable<NetworkId> m_callbackTarget;
	Archive::AutoVariable<uint32> m_stationId;
	Archive::AutoVariable<std::string> m_galaxyId;
	Archive::AutoVariable<int> m_jobSequence;
	Archive::AutoVariable<bool> m_reserve;

private:
	PlanetaryMiningJobRequest(PlanetaryMiningJobRequest const &);
	PlanetaryMiningJobRequest &operator=(PlanetaryMiningJobRequest const &);
};

inline NetworkId const &PlanetaryMiningJobRequest::getCharacterId() const
{
	return m_characterId.get();
}

inline NetworkId const &PlanetaryMiningJobRequest::getCallbackTarget() const
{
	return m_callbackTarget.get();
}

inline uint32 PlanetaryMiningJobRequest::getStationId() const
{
	return m_stationId.get();
}

inline std::string const &PlanetaryMiningJobRequest::getGalaxyId() const
{
	return m_galaxyId.get();
}

inline int PlanetaryMiningJobRequest::getJobSequence() const
{
	return m_jobSequence.get();
}

inline bool PlanetaryMiningJobRequest::getReserve() const
{
	return m_reserve.get();
}

#endif
