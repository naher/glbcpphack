#ifndef __VelocityPEAffector__
#define __VelocityPEAffector__

#include "particleemitteraffector.h"

namespace glb_ogre {

/**
* This affector changes the velocity and time to live of particles emitted
* according to sound's speed and acceleration.
*/
class VelocityPEAffector :
	public ParticleEmitterAffector
{
public:
	VelocityPEAffector(void);
	virtual ~VelocityPEAffector(void);

	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo);
};

}

#endif