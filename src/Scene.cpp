#include "Scene.h"
#include "cAudio/cAudio.h"

Scene::Scene( Ogre::Root* root, OIS::Mouse* mouse, OIS::Keyboard* keyboard )
{
	mRoot = root;
	mMouse = mouse;
	mKeyboard = keyboard;
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC); 

	// Set up main Lighting and Shadows in Scene:
	mSceneMgr->setAmbientLight( Ogre::ColourValue(0.1f,0.1f,0.1f) );
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->setShadowFarDistance( 30 );

	createRoom();
	createCameras();
}

Scene::~Scene()
{
	if (mSceneMgr) delete mSceneMgr;
}

cAudio::IAudioSource* mysound;
cAudio::IListener* listener;
void initializeAudio(int audiocardnum = 0) {
  cAudio::IAudioManager* audioMgr = cAudio::createAudioManager(false);
  
  std::cout << "\nAvailable Playback Devices: \n";
  cAudio::IAudioDeviceList* pDeviceList = cAudio::createAudioDeviceList();
  unsigned int deviceCount = pDeviceList->getDeviceCount();
  cAudio::cAudioString defaultDeviceName = pDeviceList->getDefaultDeviceName();
  for(unsigned int i=0; i<deviceCount; ++i)
  {
    cAudio::cAudioString deviceName = pDeviceList->getDeviceName(i);
    if(deviceName.compare(defaultDeviceName) == 0)
      std::cout << i << "): " << deviceName.c_str() << " [DEFAULT] \n";
    else
      std::cout << i << "): " << deviceName.c_str() << " \n";
  }
  std::cout << std::endl;
  
  //Initialize the manager with the user settings
  audioMgr->initialize(pDeviceList->getDeviceName(0).c_str());
  CAUDIO_DELETE pDeviceList;
  pDeviceList = 0;
  
  //Grab the listener object, which allows us to manipulate where "we" are in the world
  //It's useful to bind this to a camera if you are using a 3d graphics engine
  listener = audioMgr->getListener();
  
  //Create a IAudio object and load a sound from a file
  mysound = audioMgr->create("bling", "../media/0.wav",true); //TODO
  
  //Set the IAudio Sound to play3d and loop
  //play3d takes 4 arguments play3d(toloop,x,y,z,strength)
  listener->setPosition(cAudio::cVector3(0,0,0));
  mysound->play3d(cAudio::cVector3(0,0,0),.5f,true);
  mysound->setVolume(1.0f);
  mysound->setMinDistance(1.0f);
  mysound->setMaxAttenuationDistance(100.0f);  
}

Ogre::SceneNode* cubeNode;
void Scene::createRoom()
{
        initializeAudio();
	mRoomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RoomNode");

	cubeNode = mRoomNode->createChildSceneNode();
	Ogre::Entity* cubeEnt = mSceneMgr->createEntity( "Cube.mesh" );
	cubeEnt->getSubEntity(0)->setMaterialName( "CubeMaterialRed" );
	cubeNode->attachObject( cubeEnt );
	cubeNode->setPosition( 1.0, 0.0, 0.0 );

//      Ogre::SceneNode* cubeNode2 = mRoomNode->createChildSceneNode();
// 	Ogre::Entity* cubeEnt2 = mSceneMgr->createEntity( "Cube.mesh" );
// 	cubeEnt2->getSubEntity(0)->setMaterialName( "CubeMaterialGreen" );
// 	cubeNode2->attachObject( cubeEnt2 );
// 	cubeNode2->setPosition( 3.0, 0.0, 0.0 );
// 	cubeNode->setScale( 0.5, 0.5, 0.5 );
// 	cubeNode2->setScale( 0.5, 0.5, 0.5 );
// 	
// 	Ogre::SceneNode* cubeNode3 = mRoomNode->createChildSceneNode();
// 	Ogre::Entity* cubeEnt3 = mSceneMgr->createEntity( "Cube.mesh" );
// 	cubeEnt3->getSubEntity(0)->setMaterialName( "CubeMaterialWhite" );
// 	cubeNode3->attachObject( cubeEnt3 );
// 	cubeNode3->setPosition( -1.0, 0.0, 0.0 );
// 	cubeNode3->setScale( 0.5, 0.5, 0.5 );

        Ogre::Entity* roomEnt = mSceneMgr->createEntity( "Room.mesh" );
	roomEnt->setCastShadows( false );
	mRoomNode->attachObject( roomEnt );

	Ogre::Light* roomLight = mSceneMgr->createLight();
	roomLight->setType(Ogre::Light::LT_POINT);
	roomLight->setCastShadows( true );
	roomLight->setShadowFarDistance( 30 );
	roomLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	roomLight->setSpecularColour( .25, .25, .25 );
	roomLight->setDiffuseColour( 0.85, 0.76, 0.7 );

	roomLight->setPosition( 5, 5, 5 );

	mRoomNode->attachObject( roomLight );
}

void Scene::createCameras()
{
	mCamLeft = mSceneMgr->createCamera("LeftCamera");
	mCamRight = mSceneMgr->createCamera("RightCamera");

	// Create a scene nodes which the cams will be attached to:
	mBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("BodyNode");
	mBodyTiltNode = mBodyNode->createChildSceneNode();
	mHeadNode = mBodyTiltNode->createChildSceneNode("HeadNode"); 
	mBodyNode->setFixedYawAxis( true );	// don't roll!  

	mHeadNode->attachObject(mCamLeft);
	mHeadNode->attachObject(mCamRight);

	// Position cameras according to interpupillary distance
	float dist = 0.05;
	/*if (mRift->isAttached())
	{
		dist = mRift->getStereoConfig().GetIPD();
	}*/
	mCamLeft->setPosition( -dist/2.0f, 0.0f, 0.0f );
	mCamRight->setPosition( dist/2.0f, 0.0f, 0.0f );

	// If possible, get the camera projection matricies given by the rift:	
	/*if (mRift->isAttached())
	{
		mCamLeft->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Left() );
		mCamRight->setCustomProjectionMatrix( true, mRift->getProjectionMatrix_Right() );
	}*/
	mCamLeft->setFarClipDistance( 50 );
	mCamRight->setFarClipDistance( 50 );

	mCamLeft->setNearClipDistance( 0.001 );
	mCamRight->setNearClipDistance( 0.001 );

	/*mHeadLight = mSceneMgr->createLight();
	mHeadLight->setType(Ogre::Light::LT_POINT);
	mHeadLight->setCastShadows( true );
	mHeadLight->setShadowFarDistance( 30 );
	mHeadLight->setAttenuation( 65, 1.0, 0.07, 0.017 );
	mHeadLight->setSpecularColour( 1.0, 1.0, 1.0 );
	mHeadLight->setDiffuseColour( 1.0, 1.0, 1.0 );
	mHeadNode->attachObject( mHeadLight );*/

	mBodyNode->setPosition( 4.0, 1.5, 4.0 );
	//mBodyNode->lookAt( Ogre::Vector3::ZERO, Ogre::SceneNode::TS_WORLD );

	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	light->setCastShadows( false );
	light->setAttenuation( 65, 1.0, 0.07, 0.017 );
	light->setSpecularColour( .25, .25, .25 );
	light->setDiffuseColour( 0.35, 0.27, 0.23 );
	mBodyNode->attachObject(light);
}

void Scene::updateSound(Ogre::SceneNode *n) {
  cAudio::cVector3 a = cAudio::cVector3(cubeNode->_getDerivedPosition().x, cubeNode->_getDerivedPosition().y, cubeNode->_getDerivedPosition().z);
  mysound->setPosition(a);
  cAudio::cVector3 pos = cAudio::cVector3(n->_getDerivedPosition().x, n->_getDerivedPosition().y, n->_getDerivedPosition().z);
  listener->setPosition(pos);
  Ogre::Quaternion direction = n->getOrientation();
  Ogre::Vector3 dir = direction * Ogre::Vector3::NEGATIVE_UNIT_Z;
  listener->setDirection(cAudio::cVector3(dir.x, dir.y, dir.z));
}

void Scene::update( float dt )
{
	float forward = (mKeyboard->isKeyDown( OIS::KC_W ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_S ) ? 0 : -1);
	float leftRight = (mKeyboard->isKeyDown( OIS::KC_A ) ? 0 : 1) + (mKeyboard->isKeyDown( OIS::KC_D ) ? 0 : -1);

	if( mKeyboard->isKeyDown( OIS::KC_LSHIFT ) )
	{
		forward *= 3;
		leftRight *= 3;
	}
	
	Ogre::Vector3 dirX = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_X;
	Ogre::Vector3 dirZ = mBodyTiltNode->_getDerivedOrientation()*Ogre::Vector3::UNIT_Z;

	mBodyNode->setPosition( mBodyNode->getPosition() + dirZ*forward*dt + dirX*leftRight*dt );
        Scene::updateSound(mHeadNode);
}

//////////////////////////////////////////////////////////////
// Handle Rift Input:
//////////////////////////////////////////////////////////////

void Scene::setRiftPose( Ogre::Quaternion orientation, Ogre::Vector3 pos )
{
	mHeadNode->setOrientation( orientation );
	mHeadNode->setPosition( pos );
        Scene::updateSound(mHeadNode);
}

void Scene::setIPD( float IPD )
{
	mCamLeft->setPosition( -IPD/2.0f, 0.0f, 0.0f );
	mCamRight->setPosition( IPD/2.0f, 0.0f, 0.0f );
}

//////////////////////////////////////////////////////////////
// Handle Input:
//////////////////////////////////////////////////////////////

bool Scene::keyPressed( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::keyReleased( const OIS::KeyEvent& e )
{
	return true;
}
bool Scene::mouseMoved( const OIS::MouseEvent& e )
{
	if( mMouse->getMouseState().buttonDown( OIS::MB_Left ) )
	{
		mBodyNode->yaw( Ogre::Degree( -0.3*e.state.X.rel ) );
		mBodyTiltNode->pitch( Ogre::Degree( -0.3*e.state.Y.rel ) );
	}
	return true;
}
bool Scene::mousePressed( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}
bool Scene::mouseReleased( const OIS::MouseEvent& e, OIS::MouseButtonID id )
{
	return true;
}