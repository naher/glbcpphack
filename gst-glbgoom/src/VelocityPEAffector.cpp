#include "VelocityPEAffector.h"

namespace glb_ogre {

VelocityPEAffector::VelocityPEAffector(void)
{
}


VelocityPEAffector::~VelocityPEAffector(void)
{
}

void VelocityPEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) {
	double acceleration = soundInfo->accelvar / (soundInfo->accelAvg+0.000001);
	double speed = soundInfo->speedvar / (soundInfo->speedAvg+0.000001);
	double volume = soundInfo->volume / (soundInfo->volumeAvg+0.000001);

	Ogre::Real partVel = 0;
	partVel = 70 + volume*4 + speed*8 + speed*acceleration*acceleration*20;

	if (partVel > 280) partVel = 280;
	emitter->setParticleVelocity(partVel);
	emitter->setTimeToLive(partVel/76);
}

}