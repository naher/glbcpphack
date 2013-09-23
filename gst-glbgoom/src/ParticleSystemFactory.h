#ifndef __ParticleSysFactory__
#define __ParticleSysFactory__

#include "ogre3d/SdkSample.h"

namespace glb_ogre {

class ParticleSystemFactory
{
public:
	ParticleSystemFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands);
	~ParticleSystemFactory(void);

	/**
	* Creates an instance of a Particle System. Adds a node with the same name to the scene's root node.
	* Places the node according to the band, lower bands to the negative x axis.
	* band: [0, totalBands)
	*/
	virtual Ogre::ParticleSystem * createInstance(const Ogre::String &name, int band) = 0;

protected:
	Ogre::SceneManager * sceneManager;
	std::string templateName;
	int totalBands;
};

}

#endif