#include "3dview.h"

#include <bigg.hpp>
#include <bx/string.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp> 

/*
 * Snagged from Bigg's examples
 *
 * Simple rotating cubes example, based off bgfx's example-01-cubes.
 * Does not mimic the original example completely because we're in a
 * different coordinate system.
 *
 * Copyright 2011-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();
	}
	static bgfx::VertexLayout ms_layout;
};
bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};
static const uint16_t s_cubeTriList[] = { 2, 1, 0, 2, 3, 1, 5, 6, 4, 7, 6, 5, 4, 2, 0, 6, 2, 4, 3, 5, 1, 3, 7, 5, 1, 4, 0, 1, 5, 4, 6, 3, 2, 7, 3, 6 };


void C3DView::Init(bgfx::ViewId viewId, int width, int height, uint32_t clearColor)
{
	CBaseView::Init(viewId, width, height, clearColor);

	PosColorVertex::init();

	char vsName[32];
	char fsName[32];

	const char* shaderPath = "???";

	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Noop:
	case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
	case bgfx::RendererType::Gnm:                                       break;
	case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
	case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
	case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
	case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
	case bgfx::RendererType::Count:                                     break;
	}

	bx::strCopy(vsName, BX_COUNTOF(vsName), shaderPath);
	bx::strCat(vsName, BX_COUNTOF(vsName), "vs_cubes.bin");

	bx::strCopy(fsName, BX_COUNTOF(fsName), shaderPath);
	bx::strCat(fsName, BX_COUNTOF(fsName), "fs_cubes.bin");

	m_hShaderProgram = bigg::loadProgram(vsName, fsName);
	m_hVertexBuf = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);
	m_hIndexBuf = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

	m_flTime = 0.0f;
}

void C3DView::Update(float dt)
{
	CBaseView::Update(dt);

	m_flTime += dt;

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(m_width) / m_height, 0.1f, 100.0f);
	bgfx::setViewTransform(m_viewId, &view[0][0], &proj[0][0]);

	for (uint32_t yy = 0; yy < 11; ++yy)
	{
		for (uint32_t xx = 0; xx < 11; ++xx)
		{
			glm::mat4 mtx = glm::identity<glm::mat4>();
			mtx = glm::translate(mtx, glm::vec3(15.0f - float(xx) * 3.0f, -15.0f + float(yy) * 3.0f, 0.0f));
			mtx *= glm::yawPitchRoll(m_flTime + xx * 0.21f, m_flTime + yy * 0.37f, 0.0f);
			bgfx::setTransform(&mtx[0][0]);
			bgfx::setVertexBuffer(0, m_hVertexBuf);
			bgfx::setIndexBuffer(m_hIndexBuf);
			bgfx::setState(BGFX_STATE_DEFAULT);
			bgfx::submit(m_viewId, m_hShaderProgram);
		}
	}
}


