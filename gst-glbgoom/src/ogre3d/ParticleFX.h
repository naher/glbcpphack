#ifndef __ParticleFX_H__
#define __ParticleFX_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

//class _OgreSampleClassExport Sample_ParticleFX : public SdkSample
class Sample_ParticleFX : public SdkSample
{
public:

	Sample_ParticleFX()
	{
		mInfo["Title"] = "Particle Effects";
		mInfo["Description"] = "Demonstrates the creation and usage of particle effects.";
		mInfo["Thumbnail"] = "thumb_particles.png";
		mInfo["Category"] = "Effects";
		mInfo["Help"] = "Use the checkboxes to toggle visibility of the individual particle systems.";
	}

    bool frameRenderingQueued(const FrameEvent& evt)
    {
		mFountainPivot->yaw(Degree(evt.timeSinceLastFrame * 30));   // spin the fountains around

		return SdkSample::frameRenderingQueued(evt);   // don't forget the parent class updates!
    }

	void checkBoxToggled(CheckBox* box)
	{
		// show or hide the particle system with the same name as the check box
		mSceneMgr->getParticleSystem(box->getName())->setVisible(box->isChecked());
	}

protected:

	void setupContent()
	{
		// setup some basic lighting for our scene
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));
		mSceneMgr->createLight()->setPosition(20, 80, 50);

		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Degree(0), Degree(15), 250);
		mTrayMgr->showCursor();

		// create an ogre head entity and place it at the origin
        Entity* ent = mSceneMgr->createEntity("Head", "ogrehead.mesh");
        mSceneMgr->getRootSceneNode()->attachObject(ent);
		
		setupParticles();   // setup particles
		setupTogglers();    // setup particle togglers
	}

	void setupParticles()
	{
		ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);  // set nonvisible timeout

		ParticleSystem* ps;

        // create some nice fireworks and place it at the origin
        psFireworks = ps = mSceneMgr->createParticleSystem("Fireworks", "Examples/Fireworks");
			psFireworksEmBox = psFireworks->getEmitter(0);
			psFireworksEmPoint = psFireworks->getEmitter(1);
			psFireworksAfLF = psFireworks->getAffector(0);
			psFireworksAfCF = psFireworks->getAffector(1);
		mSceneMgr->getRootSceneNode()->attachObject(ps);

        // create a green nimbus around the ogre head
        psGreenyNimbus = ps = mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus");
			psGreenyNimbusEmBox = ps->getEmitter(0);
			psGreenyNimbusAfLF;
			psGreenyNimbusAfCF;
		mSceneMgr->getRootSceneNode()->attachObject(ps);
       
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
	}

	void setupTogglers()
	{
		// create check boxes to toggle the visibility of our particle systems
		mTrayMgr->createLabel(TL_TOPLEFT, "VisLabel", "Particles");
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fireworks", "Fireworks", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain1", "Fountain A", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Fountain2", "Fountain B", 130)->setChecked(true);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Aureola", "Aureola", 130)->setChecked(false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Nimbus", "Nimbus", 130)->setChecked(false);
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
};

#endif
