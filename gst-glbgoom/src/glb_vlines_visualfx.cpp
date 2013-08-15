
#include "surf3d.h"
#include "goom_tools.h"
#include "goom_config.h"
#include "goom_plugin_info.h"
#include "glb_vlines_visualfx.h"

namespace glb_goom 
{

	namespace {
		static const int colsize = 15;
		static const int colval[] = {
		  0x1416181a,
		  0x1419181a,
		  0x141f181a,
		  0x1426181a,
		  0x142a181a,
		  0x142f181a,
		  0x1436181a,
		  0x142f1819,
		  0x14261615,
		  0x13201411,
		  0x111a100a,
		  0x0c180508,
		  0x08100304,
		  0x00050101,
		  0x0
		};
	}

void GlbVLinesVisualFX::v_init(PluginInfo *info) 
{
	data = new GlbVLinesVisualFXData;
	GlbVLinesVisualFXData r_data = *data;

	r_data.last_x1 = -1;
	r_data.last_y1 = -1;
	r_data.last_x2 = -1;
	r_data.last_y2 = -1;
}

void GlbVLinesVisualFX::v_free()
{
	if (data)
		delete data;
	data = NULL;
}

void GlbVLinesVisualFX::v_apply(Pixel *src, Pixel *dest, PluginInfo *goomInfo)
{
	SoundInfo const& soundInfo(goomInfo->sound);
	GlbVLinesVisualFXData & r_data = *data;

	// dark color for a long time
	int const basecolor = 0x1416181a;
	int const color = (int) ( ( basecolor - ((soundInfo.timeSinceLastGoom > 100) ? 0 : soundInfo.timeSinceLastGoom)/100.0 * basecolor)
					      + (basecolor - (1-soundInfo.speedvar) * basecolor) ) % basecolor;

	// more lines for big gooms
	int lineCount = soundInfo.goomPower * (goomInfo->screen.width / 2.0)         /* power of the last Goom [0..1] */
				  + soundInfo.accelvar * (goomInfo->screen.width / 2.0) ;

	int const& w = goomInfo->screen.width;
	int const& h = goomInfo->screen.height;

	while (lineCount-- > 0) {
		int const x = goom_irand (goomInfo->gRandom, goomInfo->screen.width);
		int const& maxY = goomInfo->screen.height -1;

		goomInfo->methods.draw_line (
				dest
			,	x
			,	0
			,	x
			,	maxY
			,	color
			,	w
			, 	h
			);
	}

}

}
