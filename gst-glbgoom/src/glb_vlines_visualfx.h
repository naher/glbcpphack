
#ifndef _GLB_VLINES_VISUAL_FX_H
#define _GLB_VLINES_VISUAL_FX_H

#include "glb_visualfx.h"

namespace glb_goom 
{

	struct GlbVLinesVisualFXData 
	{
		int last_x1; 
		int last_y1; 
		int last_x2; 
		int last_y2;

		GlbVLinesVisualFXData() : last_x1(-1), last_y1(-1), last_x2(-1), last_y2(-1) {}
	};

	class GlbVLinesVisualFX : public GlbVisualFX<GlbVLinesVisualFX, GlbVLinesVisualFXData> 
	{
	public:
		GlbVLinesVisualFX(PluginInfo *info) { init(info); }
		~GlbVLinesVisualFX() { free(); }

		void v_init(PluginInfo *info);
		void v_free();
		void v_apply(Pixel *src, Pixel *dest, PluginInfo *info);
	};
}

#endif
