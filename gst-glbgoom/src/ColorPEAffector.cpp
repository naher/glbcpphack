#include "ColorPEAffector.h"

namespace glb_ogre {

ColorPEAffector::ColorPEAffector(void)
{
	colorSwitch = 0;
}


ColorPEAffector::~ColorPEAffector(void)
{
}

void ColorPEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) {
	emitter->setEmissionRate(300);

	double acceleration = soundInfo->accelvar / (soundInfo->accelAvg+0.000001);
	double speed = soundInfo->speedvar / (soundInfo->speedAvg+0.000001);
	double volume = soundInfo->volume / (soundInfo->volumeAvg+0.000001);

	if (soundInfo->goomPower*10 > 1 && soundInfo->timeSinceLastGoom > 10) {
		colorSwitch < 2 ? colorSwitch++ : colorSwitch = 0;
	}
	//ColourValue colorV(soundInfo.speedvar*12-0.2,soundInfo.speedvar*12-0.2,1);
	Ogre::ColourValue colorV;
	switch (colorSwitch) {
		case 0:	colorV = *(new Ogre::ColourValue(1-speed/3, 1-acceleration/3, 1-volume/2-acceleration/6));
		break;
		case 1:	colorV = *(new Ogre::ColourValue(1-acceleration/3, 1-speed/3, 1-volume/2-acceleration/6));
		break;
		case 2:	colorV = *(new Ogre::ColourValue(1-volume/2-acceleration/6, 1-acceleration/3-speed/6, 1-speed/3));
		break;
	}
	emitter->setColour(colorV);
}

}