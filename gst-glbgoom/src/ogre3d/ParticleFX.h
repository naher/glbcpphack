#ifndef __ParticleFX_H__
#define __ParticleFX_H__

#include "SdkSample.h"
#include "../LowToHighPSFactory.h"
#include "../RandomPSFactory.h"

#include "../ColorPEAffector.h"
#include "../ColorBandPEAffector.h"
#include "../VelocityPEAffector.h"
#include "../GoomPEAffector.h"

#include <sstream>

#define BANDS 14

using namespace Ogre;
using namespace OgreBites;
/**
* Class responsible for creating graphic components in Ogre scene.
*/
class Sample_ParticleFX : public SdkSample
{
public:

	Sample_ParticleFX()
		: emitters_affectors(128)
	{
		mInfo["Title"] = "Particle Effects";
		mInfo["Description"] = "Demonstrates the creation and usage of particle effects.";
		mInfo["Thumbnail"] = "thumb_particles.png";
		mInfo["Category"] = "Effects";
		mInfo["Help"] = "Use the checkboxes to toggle visibility of the individual particle systems.";
	}

    bool frameRenderingQueued(const FrameEvent& evt)
    {
		mFountainPivot->yaw(Degree(evt.timeSinceLastFrame * 90));   // spin the fountains around

		// rotate camera
		/*mCameraMan->setYawPitchDist(Degree(cameraPositionX), Degree(cameraPositionY), cameraDistance);
		cameraPositionX += evt.timeSinceLastFrame * cameraHRotationSpeed;
		cameraPositionY += evt.timeSinceLastFrame * cameraVRotationSpeed;*/

		return SdkSample::frameRenderingQueued(evt);   // don't forget the parent class updates!
    }

	void checkBoxToggled(CheckBox* box)
	{
		// show or hide the particle system with the same name as the check box
		mSceneMgr->getParticleSystem(box->getName())->setVisible(box->isChecked());
	}

protected:

	float cameraPositionX, cameraPositionY;

	/**
	* Sets up the graphic components at the init phase.
	*/
	void setupContent()
	{
	// set camera initial positions
		cameraPositionX = 0; cameraPositionY = 20; cameraDistance = 550;
		cameraHRotationSpeed = 0; cameraVRotationSpeed = 0;
		
	// setup some basic lighting for our scene
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));
		light = mSceneMgr->createLight();
		light->setPosition(0, 80, 50);

	// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Degree(0), Degree(15), 550);
		mTrayMgr->showCursor();

	// Create a prefab plane
		ManualObject* manual = mSceneMgr->createManualObject("manual");
		manual->begin("Examples/BeachStones", RenderOperation::OT_TRIANGLE_LIST);
 
		// define vertex position of index 0..3
		manual->position(-10000.0, -100.0, 10000.0); manual->textureCoord(0.0,100.0);
		manual->position( 10000.0, -100.0, -10000.0); manual->textureCoord(100.0,0.0);
		manual->position(-10000.0, -100.0, -10000.0); manual->textureCoord(0.0,0.0);
		manual->position( 10000.0, -100.0, 10000.0); manual->textureCoord(100.0,100.0);
		manual->position( 10000.0, -100.0, -10000.0); manual->textureCoord(100.0,0.0);
		manual->position(-10000.0, -100.0, 10000.0); manual->textureCoord(0.0,100.0);
 
		manual->end();
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
		
	// create Globant logo
		SceneNode* nodeGlbLogo = mSceneMgr->getRootSceneNode()->createChildSceneNode("GlobantLogo");
		Entity* glbLogo = mSceneMgr->createEntity("GlobantLogo0", "GlobantLogo.mesh");

		nodeGlbLogo->attachObject(glbLogo);
		nodeGlbLogo->setScale(30, 30, 30);
		nodeGlbLogo->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_X));
		nodeGlbLogo->setPosition(0, 40, 0);

	// create an ogre head entity
		Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
		SceneNode * nodeOgre = mSceneMgr->getRootSceneNode()->createChildSceneNode("OgreHead");
		nodeOgre->attachObject(ogreHead);
		nodeOgre->setPosition(0,-50,0);

	// create sky
		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
		
		setupParticles();   // setup particles
		setupTogglers();    // setup particle togglers
	}

	void setupParticles()
	{
		glb_ogre::ParticleEmitterAffector_vector affectors;

		ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout

		ParticleSystem* ps;

        // create some nice fireworks and place it at the origin
        psFireworks = ps = mSceneMgr->createParticleSystem("Fireworks", "Examples/Fireworks");
			psFireworksEmBox = psFireworks->getEmitter(0);
			psFireworksEmPoint = psFireworks->getEmitter(1);
			psFireworksAfLF = psFireworks->getAffector(0);
			psFireworksAfCF = psFireworks->getAffector(1);
		mSceneMgr->getRootSceneNode()->attachObject(ps);
		psFireworksEmBox->setMinParticleVelocity(150);

		// add a goom affector to fireworks ps
		affectors.push_back(glb_ogre::ParticleEmitterAffector_ptr(new glb_ogre::GoomPEAffector));
		emitters_affectors.push_back(std::make_pair(psFireworks->getEmitter(0), affectors));
		affectors.clear();

        // create a green nimbus line around the ogre head
		glb_ogre::ParticleSystemFactory * psFactory = new glb_ogre::LowToHighPSFactory(mSceneMgr,"Examples/GreenyNimbus",BANDS);
					
		// create an affector for each nimbus created
		for (int i = 0; i < BANDS; i++) {
			affectors.clear();
			affectors.push_back(glb_ogre::ParticleEmitterAffector_ptr(
				new glb_ogre::ColorBandPEAffector(i, (float)((i+1)%BANDS)/BANDS, 
													 (float)((i+1)%(BANDS/2))/BANDS, 
													 (float)((i+1)%(BANDS/3))/BANDS, -46)));
			std::stringstream str; str << "Nimbus" << i;
			emitters_affectors.push_back(
				std::make_pair(psFactory->createInstance(str.str(), i)->getEmitter(0), affectors));
		}
		       
        psRain = ps = mSceneMgr->createParticleSystem("Rain", "Examples/Rain");  // create a rainstorm
        ps->fastForward(5);   // fast-forward the rain so it looks more natural
			psRainEmBox = ps->getEmitter(0);
			psRainAfLF;
        mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 1000, 0))->attachObject(ps);

        // create aureola around ogre head perpendicular to the ground
        psAureola = ps = mSceneMgr->createParticleSystem("Aureola", "Examples/Aureola");
			psAureolaEmBox = ps->getEmitter(0);
			psAureolaAfLF;
			psAureolaAfCF2;
			psAureolaAlR;
        mSceneMgr->getRootSceneNode()->attachObject(ps);

        // create shared pivot node for spinning the fountains
        mFountainPivot = mSceneMgr->getRootSceneNode()->createChildSceneNode();

        psPurpleFountain1 = ps = mSceneMgr->createParticleSystem("Fountain1", "Examples/PurpleFountain");  // create fountain 1
			psPurpleFountain1EmPoint = psPurpleFountain1->getEmitter(0);
			psPurpleFountain1AfLF;
			psPurpleFountain1AfCF;
        // attach the fountain to a child node of the pivot at a distance and angle
		mFountainPivot->createChildSceneNode(Vector3(200, -100, 0), Quaternion(Degree(20), Vector3::UNIT_Z))->attachObject(ps);
        
        psPurpleFountain2 = ps = mSceneMgr->createParticleSystem("Fountain2", "Examples/PurpleFountain");  // create fountain 2
			psPurpleFountain2EmPoint = psPurpleFountain2->getEmitter(0);
			psPurpleFountain2AfLF;
			psPurpleFountain2AfCF;
        // attach the fountain to a child node of the pivot at a distance and angle
		mFountainPivot->createChildSceneNode(Vector3(-200, -100, 0), Quaternion(Degree(-20), Vector3::UNIT_Z))->attachObject(ps);

		// create affectors for fountain ps
		affectors.clear();
		affectors.push_back(glb_ogre::ParticleEmitterAffector_ptr(new glb_ogre::ColorPEAffector));
		affectors.push_back(glb_ogre::ParticleEmitterAffector_ptr(new glb_ogre::VelocityPEAffector));
		emitters_affectors.push_back(std::make_pair(psPurpleFountain1->getEmitter(0), affectors));
		emitters_affectors.push_back(std::make_pair(psPurpleFountain2->getEmitter(0), affectors));
		
	}

	void setupTogglers()
	{
		// create check boxes to toggle the visibility of our particle systems
		mTrayMgr->createLabel(TL_TOPLEFT, "VisLabel", "Particles");
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fireworks", "Fireworks", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain1", "Fountain A", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain2", "Fountain B", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Aureola", "Aureola", 130)->setChecked(false);
		//mTrayMgr->createCheckBox(TL_TOPLEFT, "Nimbus", "Nimbus", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Rain", "Rain", 130)->setChecked(false);
	}

	SceneNode* mFountainPivot;

public:
	ParticleSystem * psFireworks;
		ParticleEmitter  * psFireworksEmBox;
		ParticleEmitter  * psFireworksEmPoint;
		ParticleAffector * psFireworksAfLF;
		ParticleAffector * psFireworksAfCF;
	ParticleSystem * psGreenyNimbus;
		ParticleEmitter  * psGreenyNimbusEmBox;
		ParticleAffector * psGreenyNimbusAfLF;
		ParticleAffector * psGreenyNimbusAfCF;
	ParticleSystem * psRain;
		ParticleEmitter  * psRainEmBox;
		ParticleAffector * psRainAfLF;
	ParticleSystem * psAureola;
		ParticleEmitter  * psAureolaEmBox;
		ParticleAffector * psAureolaAfLF;
		ParticleAffector * psAureolaAfCF2;
		ParticleAffector * psAureolaAlR;
	ParticleSystem * psPurpleFountain1;
		ParticleEmitter  * psPurpleFountain1EmPoint;
		ParticleAffector * psPurpleFountain1AfLF;
		ParticleAffector * psPurpleFountain1AfCF;
	ParticleSystem * psPurpleFountain2;
		ParticleEmitter  * psPurpleFountain2EmPoint;
		ParticleAffector * psPurpleFountain2AfLF;
		ParticleAffector * psPurpleFountain2AfCF;
	Light * light;
	float cameraHRotationSpeed, cameraVRotationSpeed, cameraDistance;
	glb_ogre::EmitterAffectors_vector emitters_affectors;
};

#endif
