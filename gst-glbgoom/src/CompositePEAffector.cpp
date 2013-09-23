#include "CompositePEAffector.h"

namespace glb_ogre {

CompositePEAffector::CompositePEAffector(void)
{
}

CompositePEAffector::CompositePEAffector(ParticleEmitterAffector_vector const & affectors)
{
	_affectors = affectors;
}


CompositePEAffector::~CompositePEAffector(void)
{
}

void CompositePEAffector::addAffector(ParticleEmitterAffector_ptr & affector)
{
	_affectors.push_back(affector);
}

void CompositePEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo)
{
	MyFunctorClass myAffector(emitter, soundInfo, spectrumInfo);
	for_each(_affectors.begin(), _affectors.end(), myAffector);

	/*
	for_each(_affectors.begin(), _affectors.end(), 
		boost::bind(&ParticleEmitterAffector::_updateEmitter, 
			boost::bind(&ParticleEmitterAffector_ptr::get, _1), emitter, soundInfo, spectrumInfo));
	*/

	/*for (int i = 0; i < _affectors.size(); i++) {
		_affectors[i]->_updateEmitter(emitter, soundInfo, spectrumInfo);
	}*/
}

}