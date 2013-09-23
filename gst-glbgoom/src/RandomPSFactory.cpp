#include "RandomPSFactory.h"

namespace glb_ogre {

RandomPSFactory::~RandomPSFactory(void)
{
}

Ogre::ParticleSystem * RandomPSFactory::createInstance(const Ogre::String &name, int band)
{
	// Create the particle system using the built-in Ogre factory
	Ogre::ParticleSystem * ps = sceneManager->createParticleSystem(name, templateName);

	// Find a random location for the system particle
	Ogre::Vector3 v((rand() % width) - width/2, -100, (rand() % prof) - prof/2);

	// Create a node in the scene for the particle system
	sceneManager->getRootSceneNode()->createChildSceneNode(name, v)->attachObject(ps);
	
	return ps;
}

}