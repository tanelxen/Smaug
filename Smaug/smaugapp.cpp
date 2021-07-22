#include "smaugapp.h"
#include "worldrenderer.h"
#include "shadermanager.h"
#include "worldsave.h"
#include "interfaceimpl.h"

void smaugKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
		io.KeysDown[key] = action != GLFW_RELEASE;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	Input().SetInput({key, false}, action);
}

void smaugMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown))
		io.MouseDown[button] = action != GLFW_RELEASE;

	Input().SetInput({ button, true }, action);
}

void CSmaugApp::initialize(int _argc, char** _argv)
{
	ShaderManager().Init();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigDockingWithShift = true;

	GetWorldRenderer().Init();


	SupplyWorldInterface(new CWorldInterface);
	defaultWorld();


	m_uiView.Init(ViewID::MAIN_VIEW, 1024, 1024, 0x404040FF);

	m_mouseLocked = false;
	mFpsLock = 120;

	// Bigg's gonna redirect our input to imgui, and we can't have that
	// Let's take it over ourselves
	glfwSetKeyCallback(mWindow, smaugKeyCallback);
	glfwSetMouseButtonCallback(mWindow, smaugMouseButtonCallback);
}

int CSmaugApp::shutdown()
{
	ShaderManager().Shutdown();
	return 0;
}

void CSmaugApp::onReset()
{
	uint32_t width = getWidth();
	uint32_t height = getHeight();

	m_uiView.m_width = width;
	m_uiView.m_height = height;

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
}

void CSmaugApp::SetMouseLock(bool state)
{
	m_mouseLocked = state;
	if (state)
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void CSmaugApp::ToggleMouseLock()
{
	SetMouseLock(!m_mouseLocked);
}


void CSmaugApp::update(float dt)
{
	m_uiView.Update(dt,0,0);
	m_uiView.Draw(dt);
	Input().EndFrame();
}


CSmaugApp& GetApp()
{
	static CSmaugApp app;
	return app;
}
