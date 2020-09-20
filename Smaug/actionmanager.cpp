#include "actionmanager.h"
#include "smaugapp.h"
#include "utils.h"
#include "cursor.h"

CActionManager& GetActionManager()
{
	static CActionManager actionManager;
	
	return actionManager;
}


void CActionManager::Act(glm::vec3 mousePos)
{
	// Move io out of here?
	ImGuiIO& io = ImGui::GetIO();


	if (actionMode == ActionMode::NONE)
	{
		// Should we pan the view?
		if (io.MouseDown[GLFW_MOUSE_BUTTON_3])
		{
			mouseStartPos = mousePos;
			actionMode = ActionMode::PAN_VIEW;
			return;
		}

		/*
		if (m_selectedAction)
		{
			selectionInfo_t selectionInfo;
			bool success = FindFlags(mousePos, selectionInfo, m_selectedAction->GetSelectionType());

			if (success)
			{
				
				GetCursor().SetSelection(selectionInfo);

				if (io.MouseDown[GLFW_MOUSE_BUTTON_1])
				{
					m_selectedAction->Select(selectionInfo);
					actionMode = ActionMode::IN_ACTION;

					mouseStartPos = mousePos;
				}
			}
			else
			{
				GetCursor().SetPosition(mousePos);
			}
		}
		*/

	}
	else if (actionMode == ActionMode::PAN_VIEW)
	{
		if (io.MouseDown[GLFW_MOUSE_BUTTON_3])
		{
		}
		else
		{
			GetApp().m_uiView.m_editView.m_cameraPos += mousePos - mouseStartPos;
			mouseStartPos = mousePos;
			actionMode = ActionMode::NONE;
		}
	}
	else if (actionMode == ActionMode::IN_ACTION)
	{
		if (io.MouseDown[GLFW_MOUSE_BUTTON_1])
		{
		}
		else
		{
			m_selectedAction->Act(mousePos - mouseStartPos);
			actionMode = ActionMode::NONE;
		}
	}
}



bool CActionManager::FindFlags(glm::vec3 mousePos, selectionInfo_t& info, int findFlags)
{
	// If this isn't 0, ACT_SELECT_NONE, we might have issues down the line
	info.selected = ACT_SELECT_NONE;
	
	// We successfully found nothing! Woo!
	if (findFlags == ACT_SELECT_NONE)
		return true;

	// Find the selected item
	for (int i = 0; i < GetWorldEditor().m_nodes.size(); i++)
	{
		CNode* node = GetWorldEditor().m_nodes[i];

		// Check if we're in the AABB
		// Note: Maybe add a threshold to this?

		if (!node->IsPointInAABB({ mousePos.x, mousePos.z }))
			continue; // Not in bounds!


		// Do we want vertex selecting?
		if (findFlags & ACT_SELECT_VERT)
		{
			// Corner check
			for (int j = 0; j < node->m_sideCount; j++)
			{
				if (IsPointNearPoint2D(node->m_vertexes[j].origin + node->m_origin, mousePos, 4))
				{
					// Got a point. Add the flag and break the loop.
					info.selected |= ACT_SELECT_VERT;
					info.vertex = &node->m_vertexes[j];
					break;
				}
			}
		}

		if (findFlags & ACT_SELECT_WALL || findFlags & ACT_SELECT_SIDE)
		{
			bool foundSomething = false;
			// Side's walls check
			for (int j = 0; j < node->m_sideCount; j++)
			{
				nodeSide_t* side = &node->m_sides[j];

				// Are we on the side?
				if (IsPointOnLine2D(side->vertex1->origin + node->m_origin, side->vertex2->origin + node->m_origin, mousePos, 2))
				{
					if (findFlags & ACT_SELECT_SIDE)
					{
						info.selected |= ACT_SELECT_SIDE;
						info.side = &node->m_sides[j];
						
						foundSomething = true;
					}


					if (findFlags & ACT_SELECT_WALL)
					{
						// Which wall are we selecting?
						for (int k = 0; k < side->walls.size(); k++)
						{
							nodeWall_t wall = side->walls[k];
							if (IsPointOnLine2D(wall.bottomPoints[0] + node->m_origin, wall.bottomPoints[1] + node->m_origin, mousePos, 2))
							{
								info.selected |= ACT_SELECT_WALL;
								info.wall = &side->walls[k];

								// If they're asking for a wall, they're going to want a side too
								info.side = &node->m_sides[j];

								foundSomething = true;
								break;
							}
						}
					}

					if (foundSomething)
						break;

				}

			}
		}

		// Did we find something?
		if (info.selected != ACT_SELECT_NONE)
		{
			// We almost always need the node anyway
			info.selected |= ACT_SELECT_NODE;
			info.node = node;

			// We got our selected item(s). Let's dip
			return true;
		}
		else if (findFlags & ACT_SELECT_NODE)
		{
			// They just want a node
			info.selected |= ACT_SELECT_NODE;
			info.node = node;

			return true;
		}
	}

	return false;
}
