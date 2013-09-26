#ifndef __ColorBPEAffector__
#define __ColorBPEAffector__

#include "particleemitteraffector.h"

namespace glb_ogre {

/**
* This affector changes the color of particles emitted
* according to spectrum's frequency analysis.
*/
class ColorBandPEAffector :
	public ParticleEmitterAffector
{
public:
	ColorBandPEAffector(int band, float baseRed, float baseGreen, float baseBlue, int maxDb) 
		: band(band), baseRed(baseRed), baseGreen(baseGreen), baseBlue(baseBlue), maxDb(maxDb), minDb(-49) {}
	virtual ~ColorBandPEAffector(void);

	virtual void _updateEmitter(Ogre::ParticleEmitter * emitter, SoundInfo * soundInfo, Message * spectrumInfo);

private:
	int band, maxDb, minDb;
	float baseRed, baseGreen, baseBlue;
};

}

#endif