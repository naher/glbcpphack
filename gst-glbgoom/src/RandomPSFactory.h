#ifndef __RandomPSFactory__
#define __RandomPSFactory__

#include "ParticleSystemFactory.h"

namespace glb_ogre {

class RandomPSFactory :
	public ParticleSystemFactory
{
public:
	RandomPSFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands, int width, int prof) :
	  ParticleSystemFactory(sceneManager, templateName, totalBands), width(width), prof(prof) 
	  {	srand((unsigned)time(0)); }
	~RandomPSFactory(void);

	Ogre::ParticleSystem * createInstance(const Ogre::String &name, int band);

private:
	int width, prof;
};

}

#endif