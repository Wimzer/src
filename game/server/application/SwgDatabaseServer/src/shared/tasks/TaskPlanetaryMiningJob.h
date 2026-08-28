// ======================================================================
//
// TaskPlanetaryMiningJob.h
//
// ======================================================================

#ifndef INCLUDED_TaskPlanetaryMiningJob_H
#define INCLUDED_TaskPlanetaryMiningJob_H

#include "serverNetworkMessages/PlanetaryMiningJobRequest.h"
#include "sharedDatabaseInterface/Bindable.h"
#include "sharedDatabaseInterface/BindableNetworkId.h"
#include "sharedDatabaseInterface/DbQuery.h"
#include "sharedDatabaseInterface/DbTaskRequest.h"

class TaskPlanetaryMiningJob : public DB::TaskRequest
{
public:
	TaskPlanetaryMiningJob(PlanetaryMiningJobRequest const &request, uint32 requestingProcess);
	~TaskPlanetaryMiningJob();

	bool process(DB::Session *session);
	void onComplete();

private:
	class JobQuery : public DB::Query
	{
	public:
		JobQuery(std::string const &galaxyId, uint32 stationId, NetworkId const &characterId, int jobSequence, bool reserve);

		void getSQL(std::string &sql);
		bool bindParameters();
		bool bindColumns();
		QueryMode getExecutionMode() const;

		DB::BindableString<64> galaxyId;
		DB::BindableLong stationId;
		DB::BindableNetworkId characterId;
		DB::BindableLong jobSequence;
		DB::BindableLong result;
		DB::BindableLong jobCount;
		bool reserve;

	private:
		JobQuery(JobQuery const &);
		JobQuery &operator=(JobQuery const &);
	};

private:
	NetworkId m_characterId;
	NetworkId m_callbackTarget;
	uint32 m_stationId;
	std::string m_galaxyId;
	int m_jobSequence;
	bool m_reserve;
	uint32 m_requestingProcess;
	int m_result;
	int m_jobCount;

private:
	TaskPlanetaryMiningJob(TaskPlanetaryMiningJob const &);
	TaskPlanetaryMiningJob &operator=(TaskPlanetaryMiningJob const &);
};

#endif
