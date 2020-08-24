#include "worldrenderer.h"
#include "worldeditor.h"

#include <bgfx/bgfx.h>
#include <glm/gtc/matrix_transform.hpp>


bgfx::VertexLayout CreateVertexLayout()
{
	bgfx::VertexLayout layout;
	layout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.skip(sizeof(nodeVertex_t) - sizeof(glm::vec3)) // This value depends on nodeSide_t's layout. Make sure it's correct!
		.end();
	return layout;
}

static bgfx::VertexLayout g_vertexLayout;

static const uint16_t g_quadTriIndexList[] = { 0, 1, 2, 2, 3, 0 };
static bgfx::IndexBufferHandle g_quadTriIndexBuffer;
static const uint16_t g_triTriIndexList[] = { 0, 1, 2 };
static bgfx::IndexBufferHandle g_triTriIndexBuffer;

static const uint16_t g_wallTriIndexList[] = { 0, 2, 3, 3, 1, 0 };


void CWorldRenderer::Init()
{
	g_vertexLayout = CreateVertexLayout();
	g_quadTriIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(g_quadTriIndexList, sizeof(g_quadTriIndexList)));
	g_triTriIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(g_triTriIndexList, sizeof(g_triTriIndexList)));
}

void CWorldRenderer::Draw2D(bgfx::ViewId viewId, Shader shader)
{
	bgfx::ProgramHandle shaderProgram = ShaderManager::GetShaderProgram(shader);
	CWorldEditor& world = GetWorldEditor();
	for (int i = 0; i < world.m_nodes.size(); i++)
	{
		world.m_nodes[i]->m_renderData.Draw2D();
		bgfx::submit(viewId, shaderProgram);
	}
}

void CWorldRenderer::Draw3D(bgfx::ViewId viewId, Shader shader)
{
	bgfx::ProgramHandle shaderProgram = ShaderManager::GetShaderProgram(shader);
	CWorldEditor& world = GetWorldEditor();
	for (int i = 0; i < world.m_nodes.size(); i++)
	{
		world.m_nodes[i]->m_renderData.Draw3D();
		bgfx::submit(viewId, shaderProgram);
	}
}

void CNodeRenderData::Setup(CNode* node)
{
	m_parentNode = node;

	m_vertexBuf2D = bgfx::createDynamicVertexBuffer(bgfx::makeRef(node->m_vertexes, sizeof(nodeVertex_t) * node->m_sideCount), g_vertexLayout);
	
	switch (node->m_nodeType)
	{
	case NodeType::QUAD:
		m_indexBuf2D = g_quadTriIndexBuffer;
		break;
	case NodeType::TRI:
		m_indexBuf2D = g_triTriIndexBuffer;
	default:
		break;
	}

	const bgfx::Memory* vertBuf;
	const bgfx::Memory* indexBuf;

	GenerateWallBufs(vertBuf, indexBuf);

	m_vertexBuf3D = bgfx::createDynamicVertexBuffer(vertBuf, g_vertexLayout, BGFX_BUFFER_ALLOW_RESIZE);
	m_indexBuf3D = bgfx::createDynamicIndexBuffer(indexBuf, BGFX_BUFFER_ALLOW_RESIZE);

}

void CNodeRenderData::UpdateVertexBuf()
{
	bgfx::update(m_vertexBuf2D, 0, bgfx::makeRef(m_parentNode->m_vertexes, sizeof(nodeVertex_t) * m_parentNode->m_sideCount));


	const bgfx::Memory* vertBuf;
	const bgfx::Memory* indexBuf;

	GenerateWallBufs(vertBuf, indexBuf);

	bgfx::update(m_vertexBuf3D, 0, vertBuf);
	bgfx::update(m_indexBuf3D, 0, indexBuf);
}

void CNodeRenderData::GenerateWallBufs(const bgfx::Memory*& vertBuf, const bgfx::Memory*& indexBuf)
{

	// Count up our walls
	size_t wallCount = 0;
	for (int i = 0; i < m_parentNode->m_sideCount; i++)
		wallCount += m_parentNode->m_sides[i].walls.size();

	// Allocate our buffers
	// bgfx cleans these up for us
	vertBuf = bgfx::alloc(wallCount * sizeof(nodeWall_t));
	indexBuf = bgfx::alloc(wallCount * sizeof(g_wallTriIndexList)); // Each wall is a quad

	size_t vertPos = 0;
	size_t indexPos = 0;
	int currentWallPos = 0;
	// Copy in our data
	for (int i = 0; i < m_parentNode->m_sideCount; i++)
	{
		nodeSide_t* side = &m_parentNode->m_sides[i];

		// Fill vert data
		memcpy(vertBuf->data + vertPos, side->walls.data(), side->walls.size() * sizeof(nodeWall_t));
		vertPos += side->walls.size() * sizeof(nodeWall_t);

		// Fill the index data
		for (int j = 0; j < side->walls.size(); j++)
		{
			memcpy(indexBuf->data + indexPos, g_wallTriIndexList, sizeof(g_wallTriIndexList));
			
			// Increment the indexes to our current wall
			for (int k = 0; k < 6; k++)
				reinterpret_cast<uint16_t&>(indexBuf->data[indexPos + k * sizeof(uint16_t)]) += currentWallPos * 4;

			currentWallPos++;
			indexPos += sizeof(g_wallTriIndexList);
		}

	}

}

void CNodeRenderData::Draw2D()
{
	Draw2D(m_parentNode->m_origin);
}

void CNodeRenderData::Draw2D(glm::vec3 origin)
{
	glm::mat4 mtx = glm::identity<glm::mat4>();
	mtx = glm::translate(mtx, origin);

	bgfx::setTransform(&mtx[0][0]);
	bgfx::setVertexBuffer(0, m_vertexBuf2D);
	bgfx::setIndexBuffer(m_indexBuf2D);
	bgfx::setState(BGFX_STATE_DEFAULT);
	// This does not bgfx::submit!!
}

void CNodeRenderData::Draw3D()
{
	Draw3D(m_parentNode->m_origin);
}

void CNodeRenderData::Draw3D(glm::vec3 origin)
{
	glm::mat4 mtx = glm::identity<glm::mat4>();
	mtx = glm::translate(mtx, origin);

	bgfx::setTransform(&mtx[0][0]);
	bgfx::setVertexBuffer(0, m_vertexBuf3D);
	bgfx::setIndexBuffer(m_indexBuf3D);
	bgfx::setState(BGFX_STATE_DEFAULT);
	// This does not bgfx::submit!!
}

void CNodeRenderData::Shutdown()
{
}


CWorldRenderer& GetWorldRenderer()
{
	static CWorldRenderer worldRenderer;
	return worldRenderer;
}
