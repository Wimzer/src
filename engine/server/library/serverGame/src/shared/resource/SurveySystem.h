// ======================================================================
//
// SurveySystem.h
// copyright (c) 2001 Sony Online Entertainment
//
// ======================================================================

#ifndef INCLUDED_SurveySystem_H
#define INCLUDED_SurveySystem_H

// ======================================================================

#include "serverGame/GameServer.h"
#include "serverGame/NonCriticalTaskQueue.h"
#include "sharedFoundation/NetworkId.h"
#include "sharedMath/Vector.h"
#include "Singleton/Singleton2.h"

#include <vector>

// ======================================================================

/** 
 * Singleton to manage surveys.
 */
class SurveySystem : public Singleton2<SurveySystem>
{
  public:
	void requestResourceListForSurvey  (const NetworkId &playerId, const NetworkId &surveyTool, const std::string &parentResourceClassName) const;
	void requestResourceListForSurvey  (const NetworkId &playerId, const NetworkId &surveyTool, const std::string &parentResourceClassName, const std::string &planetName) const;
	void requestSurvey                 (const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const Vector &location, int surveyRange, int numPoints) const;
	void requestSurvey                 (const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) const;
	bool collectSurveyValues           (const std::string &planetName, const NetworkId &resourceTypeId, const Vector &location, int surveyRange, int numPoints, std::vector<float> &values) const;
	void requestPmdSurvey              (const NetworkId &playerId, const NetworkId &callbackTarget, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints) const;
	
  public:
	SurveySystem                       ();
	~SurveySystem                      ();
	
  private:
	class TaskGetResourceList : public NonCriticalTaskQueue::TaskRequest
	{
	  public:
		TaskGetResourceList            (const NetworkId &playerId, const NetworkId &surveyTool, const std::string &parentResourceClassName, const std::string &planetName);
		virtual ~TaskGetResourceList   ();
		virtual bool run               ();

	  private:
		NetworkId                      m_playerId;
		NetworkId                      m_surveyTool; 		
		std::string *                  m_parentResourceClassName;
		std::string *                  m_planetName;

	  private:
		TaskGetResourceList            (const TaskGetResourceList&);
		TaskGetResourceList & operator=(const TaskGetResourceList&);
	};

	class TaskSurvey : public NonCriticalTaskQueue::TaskRequest
	{
	  public:
		TaskSurvey                     (const NetworkId &playerId, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints);
		virtual ~TaskSurvey            ();
		virtual bool run               ();

	  private:
		const NetworkId                m_playerId;
		const std::string *            m_parentResourceClassName;
		const std::string *            m_resourceTypeName;
		const std::string *            m_planetName;
		Vector                         m_location;
		int                            m_surveyRange;
		int                            m_numPoints;

	  private:
		TaskSurvey                     (const TaskSurvey&);
		TaskSurvey & operator=         (const TaskSurvey&);
	};

	class TaskPmdSurvey : public NonCriticalTaskQueue::TaskRequest
	{
	  public:
		TaskPmdSurvey                  (const NetworkId &playerId, const NetworkId &callbackTarget, const std::string &parentResourceClassName, const std::string &resourceTypeName, const std::string &planetName, const Vector &location, int surveyRange, int numPoints);
		virtual ~TaskPmdSurvey         ();
		virtual bool run                ();

	  private:
		const NetworkId                 m_playerId;
		const NetworkId                 m_callbackTarget;
		const std::string *             m_parentResourceClassName;
		const std::string *             m_resourceTypeName;
		const std::string *             m_planetName;
		Vector                          m_location;
		int                             m_surveyRange;
		int                             m_numPoints;

	  private:
		TaskPmdSurvey                  (const TaskPmdSurvey&);
		TaskPmdSurvey & operator=      (const TaskPmdSurvey&);
	};
};

// ======================================================================

#endif
