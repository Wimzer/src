// ======================================================================
//
// PlanetaryMiningJobResponse.cpp
//
// ======================================================================

#include "serverNetworkMessages/FirstServerNetworkMessages.h"
#include "serverNetworkMessages/PlanetaryMiningJobResponse.h"

PlanetaryMiningJobResponse::PlanetaryMiningJobResponse(NetworkId const &characterId, NetworkId const &callbackTarget, int jobSequence, bool reserve, int result, int jobCount) :
	GameNetworkMessage("PlanetaryMiningJobResponse"),
	m_characterId(characterId),
	m_callbackTarget(callbackTarget),
	m_jobSequence(jobSequence),
	m_reserve(reserve),
	m_result(result),
	m_jobCount(jobCount)
{
	addVariable(m_characterId);
	addVariable(m_callbackTarget);
	addVariable(m_jobSequence);
	addVariable(m_reserve);
	addVariable(m_result);
	addVariable(m_jobCount);
}

PlanetaryMiningJobResponse::PlanetaryMiningJobResponse(Archive::ReadIterator &source) :
	GameNetworkMessage("PlanetaryMiningJobResponse"),
	m_characterId(),
	m_callbackTarget(),
	m_jobSequence(),
	m_reserve(),
	m_result(),
	m_jobCount()
{
	addVariable(m_characterId);
	addVariable(m_callbackTarget);
	addVariable(m_jobSequence);
	addVariable(m_reserve);
	addVariable(m_result);
	addVariable(m_jobCount);
	unpack(source);
}

PlanetaryMiningJobResponse::~PlanetaryMiningJobResponse()
{
}
