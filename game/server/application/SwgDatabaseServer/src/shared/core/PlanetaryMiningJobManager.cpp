// ======================================================================
//
// PlanetaryMiningJobManager.cpp
//
// ======================================================================

#include "SwgDatabaseServer/FirstSwgDatabaseServer.h"
#include "SwgDatabaseServer/PlanetaryMiningJobManager.h"

#include "SwgDatabaseServer/TaskPlanetaryMiningJob.h"
#include "serverDatabase/ConfigServerDatabase.h"
#include "serverDatabase/DatabaseProcess.h"
#include "serverDatabase/GameServerConnection.h"
#include "serverNetworkMessages/PlanetaryMiningJobRequest.h"
#include "sharedDatabaseInterface/DbTaskQueue.h"
#include "sharedFoundation/CrcConstexpr.hpp"
#include "sharedFoundation/ExitChain.h"
#include "sharedNetworkMessages/GameNetworkMessage.h"

PlanetaryMiningJobManager *PlanetaryMiningJobManager::ms_instance = nullptr;

void PlanetaryMiningJobManager::install()
{
	DEBUG_FATAL(ms_instance, ("PlanetaryMiningJobManager installed twice."));
	ms_instance = new PlanetaryMiningJobManager;
	ExitChain::add(&remove, "PlanetaryMiningJobManager::remove");
}

void PlanetaryMiningJobManager::remove()
{
	DEBUG_FATAL(!ms_instance, ("PlanetaryMiningJobManager was not installed."));
	delete ms_instance;
	ms_instance = nullptr;
}

PlanetaryMiningJobManager::PlanetaryMiningJobManager() :
	MessageDispatch::Receiver(),
	m_taskQueue(new DB::TaskQueue(1, DatabaseProcess::getInstance().getDBServer(), 2))
{
	connectToMessage("PlanetaryMiningJobRequest");
}

PlanetaryMiningJobManager::~PlanetaryMiningJobManager()
{
	DEBUG_FATAL(m_taskQueue, ("Call shutdown before deleting PlanetaryMiningJobManager."));
}

void PlanetaryMiningJobManager::update()
{
	m_taskQueue->update(ConfigServerDatabase::getDefaultQueueUpdateTimeLimit());
}

void PlanetaryMiningJobManager::shutdown()
{
	NOT_NULL(m_taskQueue);
	m_taskQueue->cancel();
	delete m_taskQueue;
	m_taskQueue = nullptr;
}

void PlanetaryMiningJobManager::receiveMessage(MessageDispatch::Emitter const &source, MessageDispatch::MessageBase const &message)
{
	if (message.getType() != constcrc("PlanetaryMiningJobRequest"))
		return;

	GameServerConnection const * const connection = dynamic_cast<GameServerConnection const *>(&source);
	if (!connection)
	{
		DEBUG_WARNING(true, ("PlanetaryMiningJobRequest did not come from a GameServerConnection."));
		return;
	}

	Archive::ReadIterator iterator = static_cast<GameNetworkMessage const &>(message).getByteStream().begin();
	PlanetaryMiningJobRequest const request(iterator);
	if (!request.getCharacterId().isValid() || !request.getCallbackTarget().isValid() || request.getStationId() == 0 || request.getGalaxyId().empty() || request.getGalaxyId().size() > 64 || request.getJobSequence() < 1)
	{
		DEBUG_WARNING(true, ("Rejected invalid PlanetaryMiningJobRequest for character %s.", request.getCharacterId().getValueString().c_str()));
		return;
	}

	m_taskQueue->asyncRequest(new TaskPlanetaryMiningJob(request, connection->getProcessId()));
}
