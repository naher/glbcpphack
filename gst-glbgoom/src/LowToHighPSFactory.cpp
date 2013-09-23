#include "LowToHighPSFactory.h"

#define MAX_X_POS 500

namespace glb_ogre {

LowToHighPSFactory::LowToHighPSFactory(Ogre::SceneManager * sceneManager, const char * templateName, int totalBands) :
	ParticleSystemFactory(sceneManager, templateName, totalBands)
{}


LowToHighPSFactory::~LowToHighPSFactory(void)
{
}

Ogre::ParticleSystem * LowToHighPSFactory::createInstance(const Ogre::String &name, int band) {
	// Create the particle system using the built-in Ogre factory
	Ogre::ParticleSystem * ps = sceneManager->createParticleSystem(name, templateName);

	// Calculate its position according to the band
	// x axis position is determined by band number. Lower numbers to negative positions
	Ogre::Vector3 v((band*MAX_X_POS*2/(totalBands-1))-MAX_X_POS, -100, 0);
	
	// Create a node in the scene for the particle system
	sceneManager->getRootSceneNode()->createChildSceneNode(name, v)->attachObject(ps);
	
	return ps;
}

}