#include "ColorBandPEAffector.h"

namespace glb_ogre {

ColorBandPEAffector::~ColorBandPEAffector(void)
{
}

void ColorBandPEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) {
	emitter->setEmissionRate(300);

	float factor;		// f(factor) : (minDb, maxDb) -> [1, 0]    (linear)

	if (spectrumInfo->magnitude[band] < minDb) factor = 1;
	else if (spectrumInfo->magnitude[band] > maxDb) factor = 0;
	else factor = 1 - (abs(spectrumInfo->magnitude[band]-minDb)) / (abs(maxDb-minDb));
	std::cout << spectrumInfo->magnitude[band] << std::endl;

	Ogre::ColourValue colorV (baseRed * factor, baseGreen * factor, baseBlue * factor);

	emitter->setColour(colorV);
}

}