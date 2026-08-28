// ======================================================================
//
// PlanetaryMiningJobManager.h
//
// ======================================================================

#ifndef INCLUDED_PlanetaryMiningJobManager_H
#define INCLUDED_PlanetaryMiningJobManager_H

#include "sharedMessageDispatch/Receiver.h"

namespace DB
{
	class TaskQueue;
}

class PlanetaryMiningJobManager : public MessageDispatch::Receiver
{
public:
	static void install();
	static PlanetaryMiningJobManager &getInstance();

	void update();
	void shutdown();
	void receiveMessage(MessageDispatch::Emitter const &source, MessageDispatch::MessageBase const &message);

private:
	PlanetaryMiningJobManager();
	~PlanetaryMiningJobManager();
	static void remove();

private:
	static PlanetaryMiningJobManager *ms_instance;
	DB::TaskQueue *m_taskQueue;

private:
	PlanetaryMiningJobManager(PlanetaryMiningJobManager const &);
	PlanetaryMiningJobManager &operator=(PlanetaryMiningJobManager const &);
};

inline PlanetaryMiningJobManager &PlanetaryMiningJobManager::getInstance()
{
	DEBUG_FATAL(!ms_instance, ("PlanetaryMiningJobManager was not installed."));
	return *ms_instance;
}

#endif
