#ifndef __VelocityPEAffector__
#define __VelocityPEAffector__

#include "particleemitteraffector.h"

namespace glb_ogre {

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