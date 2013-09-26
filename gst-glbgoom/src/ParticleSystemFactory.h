#ifndef __ParticleSysFactory__
#define __ParticleSysFactory__

#include "ogre3d/SdkSample.h"

namespace glb_ogre {

/**
* This class allows a fast aand easy creation of several particle systems.
*/
class ParticleSystemFactory
{
public:
	/**
	* Constructor.
	* sceneManager: The Ogre3D scene manager. Usually accesible through Ogre's root.
	* templateName: allowed names are found in Resources/Samples/Media/particle/Examples.particle
	* totalBands: number of frequency bands used to allocate particle systems in space.
	*/
	ParticleSystemFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands);
	virtual ~ParticleSystemFactory(void);

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