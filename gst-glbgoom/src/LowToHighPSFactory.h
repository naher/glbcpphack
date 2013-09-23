#ifndef __LowToHighPSFactory__
#define __LowToHighPSFactory__

#include "particlesystemfactory.h"

namespace glb_ogre {

class LowToHighPSFactory :
	public ParticleSystemFactory
{
public:
	LowToHighPSFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands);
	~LowToHighPSFactory(void);

	Ogre::ParticleSystem * createInstance(const Ogre::String &name, int band);
};

}

#endif