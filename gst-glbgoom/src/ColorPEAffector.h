#ifndef __ColorPEAffector__
#define __ColorPEAffector__

#include "particleemitteraffector.h"

namespace glb_ogre {

/**
* This affector changes the color of particles emitted
* according to sound's speed and acceleration.
*/
class ColorPEAffector :
	public ParticleEmitterAffector
{
public:
	ColorPEAffector(void);
	virtual ~ColorPEAffector(void);

	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo);

private:
	int colorSwitch;
};

}

#endif