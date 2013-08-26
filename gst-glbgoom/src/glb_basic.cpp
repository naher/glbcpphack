
#include "surf3d.h"
#include "goom_tools.h"
#include "goom_config.h"
#include "goom_plugin_info.h"
#include "glb_basic.h"


#include "glb_vlines_visualfx.h"
#include "glb_circles_visualfx.h"
#include "glb_ogre3d_visualfx.h"
#include "glb_visualfx.h"

#include <memory>

namespace glb_goom {
namespace glb_basic_fx {

std::unique_ptr<GlbVLinesVisualFX> gfx_vlines; 
std::unique_ptr<GlbCirclesVisualFX> gfx_circles; 
std::unique_ptr<GlbOgre3DVisualFX> gfx_ogre3d; 

static void glb_basic_fx_apply (VisualFX * _this, Pixel * src, Pixel * dest, PluginInfo * goomInfo) 
{
	//static GlbVLinesVisualFX & r_gfx_vlines = *gfx_vlines;
	//static GlbCirclesVisualFX & r_gfx_circles = *gfx_circles;
	static GlbOgre3DVisualFX & r_gfx_ogre3d = *gfx_ogre3d;

	SoundInfo const& soundInfo(goomInfo->sound);

	int const updateP = goom_irand(goomInfo->gRandom, 10);
	bool const shouldUpdate = updateP < soundInfo.speedvar*100;

	//if (!shouldUpdate) {
	//	//memcpy (dest, src, goomInfo->screen.size * sizeof (Pixel));
	//	return;
	//}

	//r_gfx_vlines.apply(src,  dest, goomInfo);
	//r_gfx_circles.apply(src,  dest, goomInfo);
	r_gfx_ogre3d.apply(src,  dest, goomInfo);
}

static void glb_basic_fx_init (VisualFX * _this, PluginInfo * info) 
{
	//gfx_vlines.reset(new GlbVLinesVisualFX(info));
	//gfx_circles.reset(new GlbCirclesVisualFX(info));
	gfx_ogre3d.reset(new GlbOgre3DVisualFX(info));
}

static void glb_basic_fx_free (VisualFX * _this) 
{
	//gfx_vlines.reset();
	//gfx_circles.reset();
	gfx_ogre3d.reset();
}

} // namespace glb_basic_fx
} // namespace glb_goom 

// ------------------------------------

extern "C" {

VisualFX glb_basic_fx_create (void)
{
  VisualFX fx;

  fx.init = glb_goom::glb_basic_fx::glb_basic_fx_init;
  fx.apply = glb_goom::glb_basic_fx::glb_basic_fx_apply;
  fx.free = glb_goom::glb_basic_fx::glb_basic_fx_free;
  fx.fx_data = NULL; 
  fx.params = NULL;

  return fx;
}

}
