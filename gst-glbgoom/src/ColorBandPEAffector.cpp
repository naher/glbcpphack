#include "ColorBandPEAffector.h"

namespace glb_ogre {

ColorBandPEAffector::~ColorBandPEAffector(void)
{
}

void ColorBandPEAffector::_updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo) {
	emitter->setEmissionRate(300);

	int total = 0;
	for (int i = 0; i < spectrumInfo->spectrum_bands; i++)
		total += spectrumInfo->magnitude[i];
	if (total == 0) // no new message
		return;

	float factor;		// f(factor) : (minDb, maxDb) -> [1, 0]    (linear)

	if (spectrumInfo->magnitude[band] <= minDb) factor = 1;
	else if (spectrumInfo->magnitude[band] > maxDb) factor = 0;
	else factor = 1 - (abs(spectrumInfo->magnitude[band]-minDb)) / (abs(maxDb-minDb));
	//std::cout << spectrumInfo->magnitude[band] << std::endl;

	//Ogre::ColourValue colorV (1-factor, 1-factor, 1-factor);

	/*Ogre::ColourValue colorV(baseRed * soundInfo->accelvar / soundInfo->accelAvg,
							 baseGreen * soundInfo->accelvar / soundInfo->accelAvg,
							 baseBlue * soundInfo->accelvar / soundInfo->accelAvg);*/

	/*if (band == 1)
		std::cout << spectrumInfo->magnitude[0] << " "  << spectrumInfo->magnitude[1] << " "
		 << spectrumInfo->magnitude[2] << " " << spectrumInfo->magnitude[3] << " "
		  << spectrumInfo->magnitude[4] << " " << spectrumInfo->magnitude[5] << " " << std::endl;*/

	/*factor = spectrumInfo->magnitude[band] / (soundInfo->volumeAvg*100) * -1;
	int segundos = spectrumInfo->timestamp/100000;
	int diferencia = spectrumInfo->timestamp/1000000-soundInfo->cycle;

	if (band == 1)
		std::cout << segundos << " | " << soundInfo->cycle << std::endl;
		*/
	Ogre::ColourValue colorV(baseRed - factor,
							 baseGreen - factor,
							 baseBlue - factor);

	if (baseRed > 1 || baseGreen > 1 || baseBlue > 1)
		std::cout << baseRed << " " << baseGreen << " " << baseBlue << std::endl;

	emitter->setColour(colorV);
}

}