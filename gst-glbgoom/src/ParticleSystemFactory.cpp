#include "ParticleSystemFactory.h"

namespace glb_ogre {

ParticleSystemFactory::ParticleSystemFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands)
{
	this->sceneManager = sceneManager;
	this->templateName = templateName;
	this->totalBands = totalBands;
}


ParticleSystemFactory::~ParticleSystemFactory(void)
{
}

}