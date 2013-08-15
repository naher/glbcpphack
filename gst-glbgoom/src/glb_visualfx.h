
#ifndef _GLB_VISUAL_FX_H
#define _GLB_VISUAL_FX_H

#include "goom_visual_fx.h"

namespace glb_goom {

	template <class concrete_type, class data_type>
	class GlbVisualFX 
	{
	public:
		typedef data_type DataType;

		void init(PluginInfo *info) { static_cast<concrete_type*>(this)->v_init(info); }
		void free() { static_cast<concrete_type*>(this)->v_free(); }
		void apply(Pixel *src, Pixel *dest, PluginInfo *info) { static_cast<concrete_type*>(this)->v_apply(src, dest, info); }

	protected:
		PluginParameters *params;
		DataType *data;
	};


}

#endif
