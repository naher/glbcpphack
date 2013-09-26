#ifndef __CompositePEAffector__
#define __CompositePEAffector__

#include "particleemitteraffector.h"
#include <vector>

namespace glb_ogre {

/**
* Composite Particle Emitter Affector. Its _updateEmitter calls
* every _updateEmitter of its components.
*/
class CompositePEAffector :
	public ParticleEmitterAffector
{

	// Internal class (functor) for for_each use
	class MyFunctorClass {           // function object type:
	public:
		MyFunctorClass(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) 
			: _emitter(emitter), _soundInfo(soundInfo), _spectrumInfo(spectrumInfo) {}
		void operator() (ParticleEmitterAffector_ptr const&  affector) {
			(*this)(affector.get());
		}
		void operator() (ParticleEmitterAffector * affector) {
			affector->_updateEmitter(_emitter, _soundInfo, _spectrumInfo);
		}
	private:
		Ogre::ParticleEmitter * _emitter;
		SoundInfo * _soundInfo;
		Message * _spectrumInfo;
	};

public:
	CompositePEAffector();
	CompositePEAffector(ParticleEmitterAffector_vector const & affectors);
	virtual ~CompositePEAffector(void);

	void addAffector(ParticleEmitterAffector_ptr & affector);
	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo);

private:
	ParticleEmitterAffector_vector _affectors;
};

}

#endif