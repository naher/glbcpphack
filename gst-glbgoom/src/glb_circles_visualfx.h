
#ifndef _GLB_CIRCLES_VISUAL_FX_H
#define _GLB_CIRCLES_VISUAL_FX_H

#include "glb_visualfx.h"

namespace glb_goom 
{

	struct GlbCirclesVisualFXData 
	{
		GlbCirclesVisualFXData() {}
	};

	class GlbCirclesVisualFX : public GlbVisualFX<GlbCirclesVisualFX, GlbCirclesVisualFXData> 
	{
	public:
		GlbCirclesVisualFX(PluginInfo *info) { init(info); }
		~GlbCirclesVisualFX() { free(); }

		void v_init(PluginInfo *info);
		void v_free();
		void v_apply(Pixel *src, Pixel *dest, PluginInfo *info);
	};
}

#endif
