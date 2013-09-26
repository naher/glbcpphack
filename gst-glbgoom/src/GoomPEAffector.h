#ifndef __GoomPEAffector__
#define __GoomPEAffector__

#include "particleemitteraffector.h"

namespace glb_ogre {

class GoomPEAffector :
	public ParticleEmitterAffector
{
public:
	GoomPEAffector(void);
	virtual ~GoomPEAffector(void);

	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo);
};

}

#endif