
// #include "surf3d.h"
#include "goom_tools.h"
#include "goom_config.h"
#include "goom_plugin_info.h"
#include "glb_ogre3d_visualfx.h"
#include "ColorPEAffector.h"
#include "VelocityPEAffector.h"
#include "GoomPEAffector.h"
#include "spectrumshared.h"

#include <boost/atomic.hpp>

#include <glib.h>

#include <string.h>
#include <iostream>
#include <fstream>

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

	double speedTotal, speedAvg, accelTotal, accelAvg, volumeTotal, volumeAvg, speedTotalN, speedAvgN, accelTotalN, accelAvgN, volumeTotalN, volumeAvgN;
	long int sampleCount, n;
	int colorSwitch;
	std::deque<double> speeds, accels, volumes;
	Pixel * pixelBuffer1, * pixelBuffer2; // for doublebuffering
	bool buffer1Active;
	glb_ogre::ParticleEmitterAffector * fountainColorAffector, * fountainVelAffector, * fountainGoomAffector;

	void updateLights(SoundInfo soundInfo, Ogre::Light * light) {
		Ogre::Vector3 lightPos(light->getPosition());
		lightPos += Ogre::Vector3(((soundInfo.speedvar/(speedAvg+0.00001))-1)*10, 0, 0);
		if (soundInfo.goomPower*10 > 1 && soundInfo.timeSinceLastGoom < 5)
			lightPos.x = 0;
		light->setPosition(lightPos);
	}

	/**
	* Inits ogre's thread structures.
	*/
	void setupOgreThread(ogre_thread_data & data) {
		data.fxdata.sample_browser.goStartRendering(&data.fxdata.sample);
		
		// get shared memory stored by spectrum
	    useSharedMem();
	    // Signals spectrum
	    setInit(1);
				
		// Initialize affectors
		fountainColorAffector = new glb_ogre::ColorPEAffector();
		fountainVelAffector = new glb_ogre::VelocityPEAffector();
		fountainGoomAffector = new glb_ogre::GoomPEAffector();
		
		data.fxdata.sample.psFireworksEmBox->setMinParticleVelocity(150);

		// Initialize avg aux variables
		speedTotal = 0, speedAvg = 0.00001, speedTotalN = 0, speedAvgN = 0.00001; 
		accelTotal = 0, accelAvg = 0.00001, accelTotalN = 0, accelAvgN = 0.00001;
		volumeTotal = 0, volumeAvg = 0.00001, volumeTotalN = 0, volumeAvgN = 0.00001;
		sampleCount = 0, n = 200, colorSwitch = 0;

		// notify ogre was initialized
		g_mutex_lock (data.data_mutex);
		data.initialized = true;
		g_cond_signal (data.data_cond);
		g_mutex_unlock (data.data_mutex);
	}

	/**
	* Calculates averages for every update cycle.
	*/
	void calculateAvgs(SoundInfo & soundInfo) {
		// Averages (not used by the Particle Emitter Affectors now)
		sampleCount++;
		speedTotal += soundInfo.speedvar;
		speedAvg = speedTotal / sampleCount;
		accelTotal += soundInfo.accelvar;
		accelAvg = accelTotal / sampleCount;
		volumeTotal += soundInfo.volume;
		volumeAvg = volumeTotal / sampleCount;

		// Next values are averages within a fixed sized window of size n
		speedTotalN += soundInfo.speedvar;
		accelTotalN += soundInfo.accelvar;
		volumeTotalN += soundInfo.volume;

		speeds.push_back(soundInfo.speedvar);
		accels.push_back(soundInfo.accelvar);
		volumes.push_back(soundInfo.volume);
		if (speeds.size() > n) { // window reached its max size
			speedTotalN -= *speeds.begin();
			speeds.pop_front();
			accelTotalN -= *accels.begin();
			accels.pop_front();
			volumeTotalN -= *volumes.begin();
			volumes.pop_front();
		}
		soundInfo.speedAvg = speedAvgN = speedTotalN / speeds.size();
		soundInfo.accelAvg = accelAvgN = accelTotalN / accels.size();
		soundInfo.volumeAvg = volumeAvgN = volumeTotalN / volumes.size();
	}

	/**
	* Unloads resources requested at startup.
	*/
	void stopOgreThread(ogre_thread_data & data) {
		data.fxdata.sample_browser.goStopRendering();

		delete fountainColorAffector;
		fountainColorAffector = NULL;
		delete fountainVelAffector;
		fountainVelAffector = NULL;
		delete fountainGoomAffector;
		fountainGoomAffector = NULL;
	}

	static gpointer
	service_thread_func (gpointer user_data)
	{
		ogre_thread_data & data = *((ogre_thread_data *) user_data);
		GlbOgre3DVisualFXData & r_data = data.fxdata;
		
		setupOgreThread(data);
		
		std::ofstream logFile;
		logFile.open("logCSV.txt");
		
		// render loop
		int updateCycle = 2, cycleCount = 0;
		while (!data.exit.load(boost::memory_order_relaxed)) {
			
			if (!(cycleCount--)) {
				
				// Handle double buffering
				if (pixelBuffer1 != NULL && pixelBuffer2 != NULL) { // both buffers are initialized
					Ogre::RenderTexture & rTexture = *(r_data.sample.getRttTexture());
	
					Ogre::PixelBox pixelBox(rTexture.getWidth(), rTexture.getHeight(), 1, 
											Ogre::PixelFormat::PF_BYTE_BGRA, buffer1Active? pixelBuffer1 : pixelBuffer2);

					rTexture.copyContentsToMemory(pixelBox, Ogre::RenderTarget::FrameBuffer::FB_AUTO);

					buffer1Active ^= true; // swap active buffer
				}

				cycleCount = updateCycle;

				// update ogre data from plugin info (take care about sync)
				// -- not locking yet until we test it
				// ogre_thread_data::scoped_lock(data.data_mutex);
				SoundInfo & soundInfo = data.info.sound;
				Message m;
				readMessage(&m);

				calculateAvgs(data.info.sound);

				/*printf ("%f, %d, %f, %f, %f \n", 
					soundInfo.speedvar, soundInfo.timeSinceLastGoom, soundInfo.goomPower, soundInfo.accelvar, soundInfo.volume);*/
					
				logFile << soundInfo.speedvar << "," 
						<< soundInfo.accelvar << ","
						<< speedAvg << ","
						<< accelAvg << ","
						<< speedAvgN << ","
						<< accelAvgN << '\n';

				data.fxdata.sample.cameraHRotationSpeed = 2 + soundInfo.speedvar*15 + soundInfo.accelvar*60;

			// Call update functions for every particle emitter affector
				for (int i = 0; i < r_data.sample.emitters_affectors.size(); i++) {
					Ogre::ParticleEmitter * pEmitter = r_data.sample.emitters_affectors[i].first;
					glb_ogre::ParticleEmitterAffector_vector & affectors = r_data.sample.emitters_affectors[i].second;
					for_each(affectors.begin(), affectors.end(), 
						boost::bind(&glb_ogre::ParticleEmitterAffector::_updateEmitter, 
							boost::bind(&glb_ogre::ParticleEmitterAffector_ptr::get, _1), pEmitter, &soundInfo, &m));
					//for (int j = 0; j < affectors.size(); j++)
					//	affectors[j]->_updateEmitter(pEmitter, &soundInfo, &m);
				}

				updateLights(soundInfo, r_data.sample.light);
			}

			r_data.sample_browser.goRenderOneFrame();
		}

		logFile.close();
		
		stopOgreThread(data);

		return NULL;
	}

}

void GlbOgre3DVisualFX::v_init(PluginInfo *info) 
{
	data = new GlbOgre3DVisualFXData;
	GlbOgre3DVisualFXData & r_data = *data;
	
	g_ogre_thread_data.reset(new ogre_thread_data(*info, r_data));
	ogre_thread_data & data = *g_ogre_thread_data;

	pixelBuffer1 = NULL; pixelBuffer2 = NULL;

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

	if (pixelBuffer1 == NULL)
		pixelBuffer1 = dest;
	else if (dest != pixelBuffer1 && pixelBuffer2 == NULL)
		pixelBuffer2 = dest;
}

}