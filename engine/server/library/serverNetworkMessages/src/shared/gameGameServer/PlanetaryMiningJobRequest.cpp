// ======================================================================
//
// PlanetaryMiningJobRequest.cpp
//
// ======================================================================

#include "serverNetworkMessages/FirstServerNetworkMessages.h"
#include "serverNetworkMessages/PlanetaryMiningJobRequest.h"

PlanetaryMiningJobRequest::PlanetaryMiningJobRequest(NetworkId const &characterId, NetworkId const &callbackTarget, uint32 stationId, std::string const &galaxyId, int jobSequence, bool reserve) :
	GameNetworkMessage("PlanetaryMiningJobRequest"),
	m_characterId(characterId),
	m_callbackTarget(callbackTarget),
	m_stationId(stationId),
	m_galaxyId(galaxyId),
	m_jobSequence(jobSequence),
	m_reserve(reserve)
{
	addVariable(m_characterId);
	addVariable(m_callbackTarget);
	addVariable(m_stationId);
	addVariable(m_galaxyId);
	addVariable(m_jobSequence);
	addVariable(m_reserve);
}

PlanetaryMiningJobRequest::PlanetaryMiningJobRequest(Archive::ReadIterator &source) :
	GameNetworkMessage("PlanetaryMiningJobRequest"),
	m_characterId(),
	m_callbackTarget(),
	m_stationId(),
	m_galaxyId(),
	m_jobSequence(),
	m_reserve()
{
	addVariable(m_characterId);
	addVariable(m_callbackTarget);
	addVariable(m_stationId);
	addVariable(m_galaxyId);
	addVariable(m_jobSequence);
	addVariable(m_reserve);
	unpack(source);
}

PlanetaryMiningJobRequest::~PlanetaryMiningJobRequest()
{
}
