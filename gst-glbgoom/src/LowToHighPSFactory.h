#ifndef __LowToHighPSFactory__
#define __LowToHighPSFactory__

#include "particlesystemfactory.h"

namespace glb_ogre {

/**
* This factory creates particle systems and places them according
* to the band they are being affected by.
*/
class LowToHighPSFactory :
	public ParticleSystemFactory
{
public:
	LowToHighPSFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands);
	virtual ~LowToHighPSFactory(void);

	virtual Ogre::ParticleSystem * createInstance(const Ogre::String &name, int band);
};

}

#endif