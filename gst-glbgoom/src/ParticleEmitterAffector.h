#ifndef __ParticleEmitterAffector__
#define __ParticleEmitterAffector__

#include "ogre3d/SdkSample.h"
#include "goom_plugin_info.h"
#include "spectrumshared.h"

namespace glb_ogre {

/**
* Interface for Particle Emitter Affectors. These are meant to modify particle emitters with sound info.
*/
class ParticleEmitterAffector
{
public:
	ParticleEmitterAffector(void) {};
	~ParticleEmitterAffector(void) {};
	
	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) = 0;
};

typedef boost::shared_ptr<ParticleEmitterAffector> ParticleEmitterAffector_ptr;
typedef std::vector<ParticleEmitterAffector_ptr>   ParticleEmitterAffector_vector;
typedef std::pair<Ogre::ParticleEmitter*, ParticleEmitterAffector_vector> EmitterAffectors_pair;
typedef std::vector<EmitterAffectors_pair> EmitterAffectors_vector;

}

#endif