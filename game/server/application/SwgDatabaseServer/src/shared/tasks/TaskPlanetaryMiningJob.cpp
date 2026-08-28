// ======================================================================
//
// TaskPlanetaryMiningJob.cpp
//
// ======================================================================

#include "SwgDatabaseServer/FirstSwgDatabaseServer.h"
#include "SwgDatabaseServer/TaskPlanetaryMiningJob.h"

#include "serverDatabase/DatabaseProcess.h"
#include "serverDatabase/GameServerConnection.h"
#include "serverNetworkMessages/PlanetaryMiningJobResponse.h"
#include "sharedDatabaseInterface/DbSession.h"

TaskPlanetaryMiningJob::TaskPlanetaryMiningJob(PlanetaryMiningJobRequest const &request, uint32 requestingProcess) :
	DB::TaskRequest(),
	m_characterId(request.getCharacterId()),
	m_callbackTarget(request.getCallbackTarget()),
	m_stationId(request.getStationId()),
	m_galaxyId(request.getGalaxyId()),
	m_jobSequence(request.getJobSequence()),
	m_reserve(request.getReserve()),
	m_requestingProcess(requestingProcess),
	m_result(4),
	m_jobCount(-1)
{
}

TaskPlanetaryMiningJob::~TaskPlanetaryMiningJob()
{
}

bool TaskPlanetaryMiningJob::process(DB::Session *session)
{
	JobQuery query(m_galaxyId, m_stationId, m_characterId, m_jobSequence, m_reserve);
	if (!session->exec(&query))
		return false;

	m_result = query.result.getValue();
	m_jobCount = query.jobCount.getValue();
	query.done();
	return true;
}

void TaskPlanetaryMiningJob::onComplete()
{
	GameServerConnection * const connection = DatabaseProcess::getInstance().getConnectionByProcess(m_requestingProcess);
	if (!connection)
	{
		DEBUG_REPORT_LOG(true, ("Discarded PlanetaryMiningJobResponse because GameServer %lu disconnected.", m_requestingProcess));
		return;
	}

	PlanetaryMiningJobResponse const response(m_characterId, m_callbackTarget, m_jobSequence, m_reserve, m_result, m_jobCount);
	connection->send(response, true);
}

TaskPlanetaryMiningJob::JobQuery::JobQuery(std::string const &newGalaxyId, uint32 newStationId, NetworkId const &newCharacterId, int newJobSequence, bool newReserve) :
	galaxyId(newGalaxyId),
	stationId(static_cast<long>(newStationId)),
	characterId(newCharacterId),
	jobSequence(newJobSequence),
	result(),
	jobCount(),
	reserve(newReserve)
{
}

void TaskPlanetaryMiningJob::JobQuery::getSQL(std::string &sql)
{
	std::string const procedure = reserve ? "reserve_job" : "release_job";
	sql = std::string("begin ") + DatabaseProcess::getInstance().getSchemaQualifier() + "planetary_mining_job_api." + procedure + "(:galaxy_id, :station_id, :character_id, :job_sequence, :result, :job_count); end;";
}

bool TaskPlanetaryMiningJob::JobQuery::bindParameters()
{
	if (!bindParameter(galaxyId)) return false;
	if (!bindParameter(stationId)) return false;
	if (!bindParameter(characterId)) return false;
	if (!bindParameter(jobSequence)) return false;
	if (!bindParameter(result)) return false;
	if (!bindParameter(jobCount)) return false;
	return true;
}

bool TaskPlanetaryMiningJob::JobQuery::bindColumns()
{
	return true;
}

DB::Query::QueryMode TaskPlanetaryMiningJob::JobQuery::getExecutionMode() const
{
	return MODE_PROCEXEC;
}
