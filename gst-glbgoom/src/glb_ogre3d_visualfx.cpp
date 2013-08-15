
// #include "surf3d.h"
#include "goom_tools.h"
#include "goom_config.h"
#include "goom_plugin_info.h"
#include "glb_ogre3d_visualfx.h"

#include <boost/atomic.hpp>

#include <glib.h>

#include <string.h>

namespace glb_goom 
{

namespace {
	
	struct ogre_thread_data 
	{
		PluginInfo & info;
		GlbOgre3DVisualFXData & fxdata;
		GThread *gthread;

		GMutex * data_mutex;
		GCond * data_cond;
		bool initialized;

		boost::atomic<bool> exit;

		ogre_thread_data(PluginInfo & info, GlbOgre3DVisualFXData & fxdata) 
			: info(info), fxdata(fxdata), gthread(0), exit(false), initialized(false) 
		{
			data_mutex = g_mutex_new();
			data_cond = g_cond_new();
		}

		~ogre_thread_data() {
			g_mutex_free(data_mutex);
			g_cond_free(data_cond);
		}

		struct scoped_lock 
		{
			GMutex * data_mutex;
			scoped_lock(GMutex * _data_mutex) : data_mutex(_data_mutex) 
				{ g_mutex_lock (data_mutex); }
			~scoped_lock() 
				{ g_mutex_unlock (data_mutex); }
		};
	};

	std::auto_ptr<ogre_thread_data> g_ogre_thread_data;

	static gpointer
	service_thread_func (gpointer user_data)
	{
		ogre_thread_data & data = *((ogre_thread_data *) user_data);

		GlbOgre3DVisualFXData & r_data = data.fxdata;
		r_data.sample_browser.goStartRendering(&r_data.sample);

		// notify ogre was initialized
		g_mutex_lock (data.data_mutex);
		data.initialized = true;
		g_cond_signal (data.data_cond);
		g_mutex_unlock (data.data_mutex);

		int updateCycle = 10, cycleCount = 0;

		printf ("soundInfo.speedvar, soundInfo.timeSinceLastGoom, soundInfo.goomPower, soundInfo.accelvar, soundInfo.volume, \n");

		// render loop
		while (!data.exit.load(boost::memory_order_relaxed)) {
			
			if (!(cycleCount--)) {
				cycleCount = updateCycle;

				// update ogre data from plugin info (take care about sync)
				// -- not locking yet until we test it
				// ogre_thread_data::scoped_lock(data.data_mutex);
				SoundInfo & soundInfo = data.info.sound;

				// dark color for a long time
				int const basecolor = 0x1416181a;
				int const color = (int) ( ( basecolor - ((soundInfo.timeSinceLastGoom > 100) ? 0 : soundInfo.timeSinceLastGoom)/100.0 * basecolor)
									  + (basecolor - (soundInfo.speedvar /* 100.0*/) * basecolor) ) % basecolor;
				// more lines for big gooms
				int const w = 640;
				int const h = 480;

				int circleCount = soundInfo.goomPower * (w / 2.0)         /* power of the last Goom [0..1] */
								+ soundInfo.accelvar * (w / 2.0) ;

				printf ("%f, %d, %f, %f, %f \n", 
					soundInfo.speedvar, soundInfo.timeSinceLastGoom, soundInfo.goomPower, soundInfo.accelvar, soundInfo.volume);
				/*
				printf ("soundInfo.speedvar:  %f\n", soundInfo.speedvar);
				printf ("soundInfo.timeSinceLastGoom:  %d\n", soundInfo.timeSinceLastGoom);
				printf ("soundInfo.goomPower:  %f\n", soundInfo.goomPower);
				printf ("soundInfo.accelvar:  %f\n", soundInfo.accelvar);
				*/

				// --
				Ogre::Real fwVmin1 = 0, fwVmax1 = 0 ;
				//fwVmax1 = fwVmin1 = fwVmin1 * (double)(1 + soundInfo.speedvar + ((soundInfo.timeSinceLastGoom > 100) ? 0 : soundInfo.timeSinceLastGoom)/100.0);

				Ogre::Real fwER1 = 0;
				//fwER1 = 
				//fwER1 = 80 + soundInfo.goomPower * (fwER1) + soundInfo.accelvar * (fwER1) ;				
				
				//printf ("psFireworksEmBox->setEmissionRate:  %f\n", fwER);
				//printf ("psFireworksEmBox->setParticleVelocity:  %f\n", fwVmin);
				
				data.fxdata.sample.psFireworksEmBox->setEmissionRate(fwER1);
				/*
				data.fxdata.sample.psFireworksEmBox->setEmissionRate(fwER);
				data.fxdata.sample.psFireworksEmBox->setParticleVelocity(fwVmin);
				*/

				// --
				Ogre::Real fwVmin2 = 0, fwVmax2 = 0 ;
				//Ogre::Real fwVmax2 = fwVmin2 = fwVmin2 * (double)(1 + soundInfo.speedvar + ((soundInfo.timeSinceLastGoom > 100) ? 0 : soundInfo.timeSinceLastGoom)/100.0);

				Ogre::Real fwER2 = 0;
				fwER2 = 50 + soundInfo.accelvar * 600;
				//fwER2 = 10 + soundInfo.goomPower * (fwER2) + soundInfo.accelvar * (fwER2) ;

				/*
				printf ("psPurpleFountain1EmPoint->setEmissionRate:  %f\n", fwER);
				printf ("psPurpleFountain1EmPoint->setParticleVelocity:  %f - %f\n", fwVmin, fwVmax);
				*/
				data.fxdata.sample.psPurpleFountain1EmPoint->setEmissionRate(fwER2);
				data.fxdata.sample.psPurpleFountain2EmPoint->setEmissionRate(fwER2);

				/*
				data.fxdata.sample.psPurpleFountain1EmPoint->setEmissionRate(fwER);
				data.fxdata.sample.psPurpleFountain2EmPoint->setEmissionRate(fwER);
				data.fxdata.sample.psPurpleFountain1EmPoint->setParticleVelocity(fwVmin, fwVmax);
				data.fxdata.sample.psPurpleFountain2EmPoint->setParticleVelocity(fwVmin, fwVmax);
				*/
			}

			r_data.sample_browser.goRenderOneFrame();
		}
		r_data.sample_browser.goStopRendering();
		return NULL;
	}
}

void GlbOgre3DVisualFX::v_init(PluginInfo *info) 
{
	data = new GlbOgre3DVisualFXData;
	GlbOgre3DVisualFXData & r_data = *data;

	g_ogre_thread_data.reset(new ogre_thread_data(*info, r_data));
	ogre_thread_data & data = *g_ogre_thread_data;

	// creating ogre thread
	g_ogre_thread_data->gthread = g_thread_create (service_thread_func,
		g_ogre_thread_data.get(), true, NULL);

	// wait until ogre was initialized
	g_mutex_lock (data.data_mutex);
	while (!data.initialized) {
		g_cond_wait (data.data_cond, data.data_mutex);
	}
	g_mutex_unlock (data.data_mutex);
}

void GlbOgre3DVisualFX::v_free()
{
	GlbOgre3DVisualFXData & r_data = *data;

	// notify ogre thread that it should die
	g_ogre_thread_data->exit.store(true, boost::memory_order_relaxed);

	// wait for ogre shut down
	g_thread_join(g_ogre_thread_data->gthread);

	if (data)
		delete data;
	data = NULL;
}

void GlbOgre3DVisualFX::v_apply(Pixel *src, Pixel *dest, PluginInfo *goomInfo)
{
	SoundInfo const& soundInfo(goomInfo->sound);
	GlbOgre3DVisualFXData & r_data = *data;

	// update ogre data from plugin info (take care about sync)
	// -- nothing todo by now since ogre takes the actual 
}

}
