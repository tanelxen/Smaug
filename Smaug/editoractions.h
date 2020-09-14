#pragma once
#include "actionmanager.h"

class CVertDragAction : public CBaseAction
{
public:
	virtual void Select(selectionInfo_t selectInfo)
	{
		//CBaseAction::Select(selectInfo);
		m_originalPos = m_selectInfo.vertex->origin;
	}

	virtual void Preview(glm::vec3 moveDelta)
	{
		m_selectInfo.vertex->origin = m_originalPos + moveDelta;
		m_node->Update();
	}

	virtual void Act(glm::vec3 moveDelta)
	{
		m_selectInfo.vertex->origin = m_originalPos + moveDelta;
		m_node->Update();
		m_finalMoveDelta = moveDelta;
	}

	virtual void Undo()
	{
		m_selectInfo.vertex->origin -= m_finalMoveDelta;
		m_node->Update();
	}

	virtual void Redo()
	{
		m_selectInfo.vertex->origin += m_finalMoveDelta;
		m_node->Update();
	}

	virtual void Cancel()
	{
		m_selectInfo.vertex->origin = m_originalPos;
		m_node->Update();
	}
private:
	glm::vec3 m_originalPos;
	glm::vec3 m_finalMoveDelta;

	// Inherited via CBaseAction
	virtual int GetSelectionType() override;
};


class CWallExtrudeAction : public CBaseAction
{
	// Inherited via CBaseAction
	virtual void Preview(glm::vec3 moveDelta);
	virtual void Act(glm::vec3 moveDelta);
	virtual void Cancel();
	virtual void Undo();
	virtual void Redo();
};

class CSideDragAction : public CBaseAction
{
public:

	virtual void Preview(glm::vec3 moveDelta)
	{
	}

	virtual void Act(glm::vec3 moveDelta)
	{
		m_selectInfo.side->vertex1->origin += moveDelta;
		m_selectInfo.side->vertex2->origin += moveDelta;
		m_node->Update();
	}

	virtual void Undo()
	{
	}

	virtual void Redo()
	{
	}

	virtual void Cancel()
	{
	}
};
