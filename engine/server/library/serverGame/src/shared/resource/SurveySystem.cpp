// ======================================================================
//
// SurveySystem.cpp
// copyright (c) 2001 Sony Online Entertainment
//
// ======================================================================

#include "serverGame/FirstServerGame.h"
#include "serverGame/SurveySystem.h"

#include "UnicodeUtils.h"
#include "serverGame/PlanetObject.h"
#include "serverGame/ResourcePoolObject.h"
#include "serverGame/ResourceTypeObject.h"
#include "serverGame/ServerResourceClassObject.h"
#include "serverGame/ServerUniverse.h"
#include "serverGame/ServerWorld.h"
#include "serverScript/GameScriptObject.h"
#include "serverScript/ScriptParameters.h"
#include "sharedFoundation/ExitChain.h"
#include "swgSharedNetworkMessages/ResourceListForSurveyMessage.h"
#include "swgSharedNetworkMessages/SurveyMessage.h"

// ======================================================================

SurveySystem::SurveySystem() :
		Singleton2<SurveySystem>()
{
	ExitChain::add(&remove, "SurveySystem::remove");
}

// ----------------------------------------------------------------------

SurveySystem::~SurveySystem()
{
}

// ----------------------------------------------------------------------

void SurveySystem::requestResourceListForSurvey(const NetworkId &playerId, const NetworkId &surveyTool, const std::string &parentResourceClassName) const
{
	requestResourceListForSurvey(playerId, surveyTool, parentResourceClassName, ServerWorld::getSceneId());
}

// ----------------------------------------------------------------------

void SurveySystem::requestResourceListForSurvey(const NetworkId &playerId, const NetworkId &surveyTool, const std::string &parentResourceClassName, const std::string &planetName) const
{
	NonCriticalTaskQueue::getInstance().addTask(new TaskGetResourceList(playerId, surveyTool, parentResourceClassName, planetName));
}

// ----------------------------------------------------------------------

void SurveySystem::requestSurvey(const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const Vector &location, int surveyRange, int numPoints) const
{
	requestSurvey(playerId, parentResourceClassName, resourceTypeName, ServerWorld::getSceneId(), location, surveyRange, numPoints);
}

// ----------------------------------------------------------------------

void SurveySystem::requestSurvey(const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) const
{
	NonCriticalTaskQueue::getInstance().addTask(new TaskSurvey(playerId, parentResourceClassName, resourceTypeName, planetName, location, surveyRange, numPoints));
}

// ----------------------------------------------------------------------

bool SurveySystem::collectSurveyValues(const std::string &planetName, const NetworkId &resourceTypeId, const Vector &location, int surveyRange, int numPoints, std::vector<float> &values) const
{
	values.clear();
	if ((numPoints < 2) || (numPoints > 64) || (surveyRange < 1) || (surveyRange > 4096) || (numPoints > surveyRange + 1))
		return false;
	ResourceTypeObject const * const typeObj = ServerUniverse::getInstance().getResourceTypeById(resourceTypeId);
	PlanetObject const * const planet = ServerUniverse::getInstance().getPlanetByName(planetName);
	ResourcePoolObject const * const pool = typeObj && planet ? typeObj->getPoolForPlanet(*planet) : nullptr;
	if (!pool)
		return false;

	int const distBetweenPoints = surveyRange / (numPoints - 1); // -1 is so that we get points at both ends
	int const radius = surveyRange / 2;
	values.reserve(numPoints * numPoints);
	for (int xIndex = 0; xIndex < numPoints; ++xIndex)
		for (int zIndex = 0; zIndex < numPoints; ++zIndex)
		{
			float const x = location.x - radius + (xIndex * distBetweenPoints);
			float const z = location.z - radius + (zIndex * distBetweenPoints);
			values.push_back(pool->getEfficiencyAtLocation(x, z));
		}

	return true;
}

// ======================================================================

SurveySystem::TaskGetResourceList::TaskGetResourceList(const NetworkId &playerId, const NetworkId& surveyTool, const std::string &parentResourceClassName, const std::string &planetName) :
	m_playerId(playerId),
	m_surveyTool(surveyTool),
	m_parentResourceClassName(new std::string(parentResourceClassName)),
	m_planetName(new std::string(planetName))
{
}

// ----------------------------------------------------------------------

void SurveySystem::requestPmdSurvey(const NetworkId &playerId, const NetworkId &callbackTarget, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) const
{
	NonCriticalTaskQueue::getInstance().addTask(new TaskPmdSurvey(playerId, callbackTarget, parentResourceClassName, resourceTypeName, planetName, location, surveyRange, numPoints));
}

// ----------------------------------------------------------------------

SurveySystem::TaskGetResourceList::~TaskGetResourceList()
{
	delete m_parentResourceClassName;
	m_parentResourceClassName = 0;
	delete m_planetName;
	m_planetName = 0;
}

// ----------------------------------------------------------------------

bool SurveySystem::TaskGetResourceList::run()
{
	Client *client = GameServer::getInstance().getClient(m_playerId);
	const ResourceClassObject *masterClass = ServerUniverse::getInstance().getResourceClassByName(*m_parentResourceClassName);
	PlanetObject const *planet = ServerUniverse::getInstance().getPlanetByName(*m_planetName);
	if (client && masterClass && planet)
	{
		std::vector<ResourceTypeObject const *> results;
		std::vector<ResourceListForSurveyMessage::DataItem> sendableResults;
		planet->getAvailableResourceList(results,*masterClass);
		for (std::vector<ResourceTypeObject const *>::const_iterator i=results.begin(); i!=results.end(); ++i)
		{
			ResourceTypeObject const * const rto = *i;
			NOT_NULL(rto);
			if (rto)
			{
				ResourceListForSurveyMessage::DataItem d;
				d.resourceName    = rto->getResourceName();
				d.resourceId      = rto->getNetworkId();
				d.parentClassName = rto->getParentClass().getResourceClassName();
				
				sendableResults.push_back(d);
			}
		}
		ResourceListForSurveyMessage msg(sendableResults, masterClass->getResourceClassName(), m_surveyTool);
		client->send(msg,true);
	}
	else
	{
		// TODO:  request isn't valid -- what do we do?
	}

	return true;
}

// ======================================================================

SurveySystem::TaskSurvey::TaskSurvey(const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) :
	m_playerId                (playerId),
	m_parentResourceClassName (new std::string(parentResourceClassName)),
	m_resourceTypeName        (new std::string(resourceTypeName)),
	m_planetName              (new std::string(planetName)),
		m_location                (location),
		m_surveyRange             (surveyRange),
		m_numPoints               (numPoints)
{
	DEBUG_WARNING((m_numPoints/2)*2 == m_numPoints,("The number of points on a side should be an odd number when taking a survey, to guarantee that the point the player is standing on is included in the survey.\n"));
}

// ----------------------------------------------------------------------

SurveySystem::TaskSurvey::~TaskSurvey()
{
	delete m_parentResourceClassName; //lint !e605 // deleting const pointer
	delete m_resourceTypeName;        //lint !e605 // deleting const pointer
	m_parentResourceClassName = 0;
	m_resourceTypeName = 0;
	delete m_planetName;
	m_planetName = 0;
}

// ----------------------------------------------------------------------

bool SurveySystem::TaskSurvey::run()
{
	if ((m_numPoints < 2) || (m_numPoints > 64) || (m_surveyRange < 1) || (m_surveyRange > 4096) || (m_numPoints > m_surveyRange + 1))
		return true;

	Client const *              client            = GameServer::getInstance().getClient(m_playerId);
	ResourceTypeObject const *  typeObj           = ServerUniverse::getInstance().getResourceTypeByName(*m_resourceTypeName);
	ResourceClassObject const * parentClass       = ServerUniverse::getInstance().getResourceClassByName(*m_parentResourceClassName);
	PlanetObject const *        planet            = ServerUniverse::getInstance().getPlanetByName(*m_planetName);
	ResourcePoolObject const *  pool              = typeObj && planet ? typeObj->getPoolForPlanet(*planet) : nullptr;
	int                         distBetweenPoints = m_surveyRange / (m_numPoints - 1); // -1 is so that we get points at both ends
	int                         radius            = m_surveyRange / 2;
	
	if (client && typeObj && parentClass && pool && (typeObj->isDerivedFrom(*parentClass)))
	{
		std::vector<float> surveyValues;
		if (!SurveySystem::getInstance().collectSurveyValues(*m_planetName, typeObj->getNetworkId(), m_location, m_surveyRange, m_numPoints, surveyValues))
			return true;

		std::vector<SurveyMessage::DataItem> surveyData;
		SurveyMessage::DataItem item;

		//vectors to store the data that goes to the script trigger
		std::vector<float>   xVals;
		std::vector<float>   zVals;
		std::vector<float>   efficiencyVals;
		
		size_t surveyValueIndex = 0;
		for (int xIndex = 0; xIndex < m_numPoints; ++xIndex)
			for (int zIndex = 0; zIndex < m_numPoints; ++zIndex)
			{
				item.m_location.x = m_location.x - radius + (xIndex * distBetweenPoints);
				item.m_location.z = m_location.z - radius + (zIndex * distBetweenPoints);
				item.m_efficiency = surveyValues[surveyValueIndex++];
				surveyData.push_back(item);
				DEBUG_REPORT_LOG(true,("Adding data item (%f,%f,%f) -- %f\n",item.m_location.x, item.m_location.y, item.m_location.z, item.m_efficiency));
				xVals.push_back(item.m_location.x);
				zVals.push_back(item.m_location.z);
				efficiencyVals.push_back(item.m_efficiency);
			}

		//send the client the response data
		SurveyMessage msg(surveyData);
		client->send(msg,true);

		//send the data to script trigger
		ScriptParams params;
		params.addParam(xVals);
		params.addParam(zVals);
		params.addParam(efficiencyVals);
		ServerObject* so = ServerWorld::findObjectByNetworkId(m_playerId);
		if (so)
			IGNORE_RETURN(so->getScriptObject()->trigAllScripts(Scripting::TRIG_SURVEY_DATA_RECEIVED, params));
	}
	else
	{
		// TODO: Survey is not valid -- what to do?
	}
	return true;
}

// ======================================================================

SurveySystem::TaskPmdSurvey::TaskPmdSurvey(const NetworkId &playerId, const NetworkId &callbackTarget, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) :
	m_playerId                (playerId),
	m_callbackTarget          (callbackTarget),
	m_parentResourceClassName (new std::string(parentResourceClassName)),
	m_resourceTypeName        (new std::string(resourceTypeName)),
	m_planetName              (new std::string(planetName)),
	m_location                (location),
	m_surveyRange             (surveyRange),
	m_numPoints               (numPoints)
{
}

// ----------------------------------------------------------------------

SurveySystem::TaskPmdSurvey::~TaskPmdSurvey()
{
	delete m_parentResourceClassName;
	delete m_resourceTypeName;
	delete m_planetName;
	m_parentResourceClassName = 0;
	m_resourceTypeName = 0;
	m_planetName = 0;
}

// ----------------------------------------------------------------------

bool SurveySystem::TaskPmdSurvey::run()
{
	ServerObject * const callbackTarget = ServerWorld::findObjectByNetworkId(m_callbackTarget);
	Client const * const client = GameServer::getInstance().getClient(m_playerId);
	std::vector<float> xVals;
	std::vector<float> zVals;
	std::vector<float> efficiencyVals;
	ResourceTypeObject const * const typeObj = ServerUniverse::getInstance().getResourceTypeByName(*m_resourceTypeName);
	ResourceClassObject const * const parentClass = ServerUniverse::getInstance().getResourceClassByName(*m_parentResourceClassName);
	PlanetObject const * const planet = ServerUniverse::getInstance().getPlanetByName(*m_planetName);

	if (client && callbackTarget && typeObj && parentClass && planet && typeObj->isDerivedFrom(*parentClass))
	{
		std::vector<ResourceTypeObject const *> availableResources;
		planet->getAvailableResourceList(availableResources, *parentClass);
		bool resourceIsAvailable = false;
		for (std::vector<ResourceTypeObject const *>::const_iterator i = availableResources.begin(); i != availableResources.end(); ++i)
		{
			if (*i == typeObj)
			{
				resourceIsAvailable = true;
				break;
			}
		}

		if (resourceIsAvailable && SurveySystem::getInstance().collectSurveyValues(*m_planetName, typeObj->getNetworkId(), m_location, m_surveyRange, m_numPoints, efficiencyVals))
		{
			int const distBetweenPoints = m_surveyRange / (m_numPoints - 1);
			int const radius = m_surveyRange / 2;
			for (int xIndex = 0; xIndex < m_numPoints; ++xIndex)
				for (int zIndex = 0; zIndex < m_numPoints; ++zIndex)
				{
					float const x = m_location.x - radius + (xIndex * distBetweenPoints);
					float const z = m_location.z - radius + (zIndex * distBetweenPoints);
					xVals.push_back(x);
					zVals.push_back(z);
				}

		}
	}

	if (callbackTarget)
	{
		ScriptParams params;
		params.addParam(xVals);
		params.addParam(zVals);
		params.addParam(efficiencyVals);
		IGNORE_RETURN(callbackTarget->getScriptObject()->trigAllScripts(Scripting::TRIG_SURVEY_DATA_RECEIVED, params));
	}

	return true;
}

// ======================================================================
