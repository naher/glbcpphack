
#include "surf3d.h"
#include "goom_tools.h"
#include "goom_config.h"
#include "goom_plugin_info.h"
#include "glb_circles_visualfx.h"

#include <string.h>

namespace glb_goom 
{

void GlbCirclesVisualFX::v_init(PluginInfo *info) 
{
	data = new GlbCirclesVisualFXData;
	GlbCirclesVisualFXData r_data = *data;
}

void GlbCirclesVisualFX::v_free()
{
	if (data)
		delete data;
	data = NULL;
}

void GlbCirclesVisualFX::v_apply(Pixel *src, Pixel *dest, PluginInfo *goomInfo)
{
	SoundInfo const& soundInfo(goomInfo->sound);
	GlbCirclesVisualFXData & r_data = *data;

	// dark color for a long time
	int const basecolor = 0x1416181a;
	int const color = (int) ( ( basecolor - ((soundInfo.timeSinceLastGoom > 100) ? 0 : soundInfo.timeSinceLastGoom)/100.0 * basecolor)
					      + (basecolor - (1-soundInfo.speedvar) * basecolor) ) % basecolor;
	// more lines for big gooms
	int circleCount = soundInfo.goomPower * (goomInfo->screen.width / 2.0)         /* power of the last Goom [0..1] */
				    + soundInfo.accelvar * (goomInfo->screen.width / 2.0) ;

	int const& w = goomInfo->screen.width;
	int const& h = goomInfo->screen.height;

	while (circleCount-- > 0) {

		int const x = goom_irand (goomInfo->gRandom, goomInfo->screen.width);
		int const y = goom_irand (goomInfo->gRandom, goomInfo->screen.height);
		int const r = goom_irand (goomInfo->gRandom, 15);

		/*goomInfo->methods.draw_circle (
				dest
			,	x
			,	y
			,	r
			,	color
			,	w
			, 	h
			);*/

	}

}

}
