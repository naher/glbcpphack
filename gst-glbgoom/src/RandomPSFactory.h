#ifndef __RandomPSFactory__
#define __RandomPSFactory__

#include "ParticleSystemFactory.h"

namespace glb_ogre {

/**
* This factory creates a new Particle System in a random position inside rectangle of width x prof.
*/
class RandomPSFactory :
	public ParticleSystemFactory
{
public:
	RandomPSFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands, int width, int prof) :
	  ParticleSystemFactory(sceneManager, templateName, totalBands), width(width), prof(prof) 
	  {	srand((unsigned)time(0)); }
	virtual ~RandomPSFactory(void);

	virtual Ogre::ParticleSystem * createInstance(const Ogre::String &name, int band);

private:
	int width, prof;
};

}

#endif