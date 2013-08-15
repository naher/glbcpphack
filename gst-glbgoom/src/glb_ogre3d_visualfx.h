
#ifndef _GLB_OGRE3D_VISUAL_FX_H
#define _GLB_OGRE3D_VISUAL_FX_H

#include "glb_visualfx.h"

#include "ogre3d/ParticleFX.h"
#include "ogre3d/SampleBrowser.h"

namespace glb_goom 
{
	struct GlbOgre3DVisualFXData 
	{
		GlbOgre3DVisualFXData() {}

		Sample_ParticleFX sample;
		OgreBites::SampleBrowser sample_browser;
	};

	class GlbOgre3DVisualFX : public GlbVisualFX<GlbOgre3DVisualFX, GlbOgre3DVisualFXData> 
	{
	public:
		GlbOgre3DVisualFX(PluginInfo *info) { init(info); }
		~GlbOgre3DVisualFX() { free(); }

		void v_init(PluginInfo *info);
		void v_free();
		void v_apply(Pixel *src, Pixel *dest, PluginInfo *info);
	};
}

#endif
