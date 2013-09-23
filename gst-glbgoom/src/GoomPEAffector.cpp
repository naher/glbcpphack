#include "GoomPEAffector.h"

namespace glb_ogre {

GoomPEAffector::GoomPEAffector(void)
{
}


GoomPEAffector::~GoomPEAffector(void)
{
}

void GoomPEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo)
{
	if (soundInfo->timeSinceLastGoom < 3) {
		emitter->setEmissionRate(soundInfo->goomPower*10);
		emitter->setParticleVelocity(soundInfo->goomPower*500);
	} else {
		emitter->setEmissionRate(0);
	}
}

}