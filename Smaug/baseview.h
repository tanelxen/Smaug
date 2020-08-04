#pragma once
#include "ibaseview.h"
#include "viewlist.h"

class CBaseView : public IBaseView
{
public:
	virtual void Init(bgfx::ViewId viewId, int width, int height, uint32_t clearColor);

	virtual void Update(float dt);

	virtual ImTextureID GetImTextureID();

	bgfx::ViewId m_viewId;
	bgfx::FrameBufferHandle m_framebuffer;

	int m_width;
	int m_height;
	uint32_t m_clearColor;
};
