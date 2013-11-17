#include <jni.h>
#include <time.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <lua-headers/moai_lua.h>

#include <moai-android/moaiext-android.h>
#include <moai-android/moaiext-jni.h>

#include <moai-core/headers.h>
#include <moai-core/host.h>
#include <moai-sim/headers.h>
#include <moai-sim/host.h>
#include <moai-util/host.h>
#include <moai-http-client/host.h>
#include <moai-luaext/host.h>

#if MOAI_WITH_BOX2D
	#include <moai-box2d/host.h>
#endif

#if MOAI_WITH_CHIPMUNK
	#include <moai-chipmunk/host.h>
#endif

#if MOAI_WITH_FMOD_EX
	#include <moai-fmod-ex/host.h>
#endif

#if MOAI_WITH_UNTZ
	#include <moai-untz/host.h>
#endif

//================================================================//
// Input event locking queue
//================================================================//

	template < class T >
	class LockingQueue {
	public:
		pthread_mutex_t mutex;
		int tail;
		int num;

		static const int kMaxMessages = 100;

		T messages [ kMaxMessages ];

		//----------------------------------------------------------------//
		void Push ( const T &message ) {

			pthread_mutex_lock ( &mutex );

			if ( num >= LockingQueue::kMaxMessages ) {

				printf ( "ERROR: g_MessageQueue, kMaxMessages (%d) exceeded\n", LockingQueue::kMaxMessages );
			}
			else {

				int head = ( tail + num) % LockingQueue::kMaxMessages;

			 	messages [ head ] = message;
				++num;

				if ( num >= LockingQueue::kMaxMessages )  {

				 	num -= LockingQueue::kMaxMessages;
				}
			}

			pthread_mutex_unlock ( &mutex );
		}

		//----------------------------------------------------------------//
		int Pop ( T &message ) {

			pthread_mutex_lock ( &mutex );

			int result = 0;

			if ( num > 0) {

				result = 1;
				message = messages [ tail ];
				++tail;

				if ( tail >= LockingQueue::kMaxMessages) {

					tail -= LockingQueue::kMaxMessages;
				}

				--num;
			}

			pthread_mutex_unlock ( &mutex );

			return result;
		}
	};

	struct InputEvent {

		enum {
			INPUTEVENT_LEVEL,
			INPUTEVENT_COMPASS,
			INPUTEVENT_LOCATION,
			INPUTEVENT_TOUCH,
		};

		// all
		int 	m_type;
		int 	m_deviceId;
		int 	m_sensorId;

		// touch, level
		float 	m_x;
		float 	m_y;

		// level
		float 	m_z;

		// compass
		int 	m_heading;

		// touch
		int  	m_touchId;
		bool 	m_down;
		int  	m_tapCount;

		// location
		double 	m_longitude;
		double 	m_latitude;
		double 	m_altitude;
		float  	m_hAccuracy;
		float  	m_vAccuracy;
		float  	m_speed;
	};

	LockingQueue < InputEvent > * inputQueue = NULL;

//================================================================//
// JNI set up
//================================================================//

	JavaVM* jvm;

	//----------------------------------------------------------------//
	int JNI_OnLoad ( JavaVM* vm, void* reserved ) {

		jvm = vm;

		return JNI_VERSION_1_4;
	}

//================================================================//
// Miscellaneous JNI Functions
//================================================================//

	//----------------------------------------------------------------//
	extern "C" int Java_com_ziplinegames_moai_Moai_AKUCreateContext ( JNIEnv* env, jclass obj ) {

		return AKUCreateContext ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUDeleteContext ( JNIEnv* env, jclass obj, jint contextId ) {

		AKUDeleteContext ( contextId );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUDetectGfxContext ( JNIEnv* env, jclass obj ) {

		AKUDetectGfxContext ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUEnqueueCompassEvent ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jfloat heading ) {

		InputEvent ievent;

		ievent.m_type = InputEvent::INPUTEVENT_COMPASS;

		ievent.m_deviceId = deviceId;
		ievent.m_sensorId = sensorId;

		ievent.m_heading = heading;

		inputQueue->Push ( ievent );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUEnqueueLevelEvent ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jfloat x, jfloat y, jfloat z ) {

		InputEvent ievent;

		ievent.m_type = InputEvent::INPUTEVENT_LEVEL;

		ievent.m_deviceId = deviceId;
		ievent.m_sensorId = sensorId;

		ievent.m_x = x;
		ievent.m_y = y;
		ievent.m_z = z;

		inputQueue->Push ( ievent );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUEnqueueLocationEvent ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jdouble longitude, jdouble latitude, jdouble altitude, jfloat hAccuracy, jfloat vAccuracy, jfloat speed ) {

		InputEvent ievent;

		ievent.m_type = InputEvent::INPUTEVENT_LOCATION;

		ievent.m_deviceId = deviceId;
		ievent.m_sensorId = sensorId;

		ievent.m_longitude = longitude;
		ievent.m_latitude = latitude;
		ievent.m_altitude = altitude;
		ievent.m_hAccuracy = hAccuracy;
		ievent.m_vAccuracy = vAccuracy;
		ievent.m_speed = speed ;

		inputQueue->Push ( ievent );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUEnqueueTouchEvent ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jint touchId, jboolean down, jint x, jint y ) {

		InputEvent ievent;

		ievent.m_type = InputEvent::INPUTEVENT_TOUCH;

		ievent.m_deviceId = deviceId;
		ievent.m_sensorId = sensorId;

		ievent.m_touchId = touchId;
		ievent.m_down = down;
		ievent.m_x = x;
		ievent.m_y = y;

		inputQueue->Push ( ievent );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUExtLoadLuacrypto ( JNIEnv* env, jclass obj ) {

		#if MOAI_WITH_CRYPTO
			AKUExtLoadLuacrypto ();
		#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUExtLoadLuacurl ( JNIEnv* env, jclass obj ) {

		#if MOAI_WITH_CURL
			AKUExtLoadLuacurl ();
		#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUExtLoadLuasocket ( JNIEnv* env, jclass obj ) {

		AKUExtLoadLuasocket ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUExtLoadLuasql ( JNIEnv* env, jclass obj ) {

		#if MOAI_WITH_SQLITE3
			AKUExtLoadLuasql ();
		#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUExtLoadLuayaml ( JNIEnv* env, jclass obj ) {

		#if MOAI_WITH_YAML
			AKUExtLoadLuayaml ();
		#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUFinalize	( JNIEnv* env, jclass obj ) {
        #if MOAI_WITH_BOX2D
            AKUFinalizeBox2D ();
        #endif

        #if MOAI_WITH_CHIPMUNK
            AKUFinalizeChipmunk ();
        #endif

        AKUFinalizeUtil ();
        AKUFinalizeSim ();
		AKUFinalize ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUFMODExInit ( JNIEnv* env, jclass obj ) {

#if MOAI_WITH_FMOD_EX
		AKUFmodExInit ();
#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUInit ( JNIEnv* env, jclass obj ) {

		MOAIAppAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIAppAndroid );

		MOAIBrowserAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIBrowserAndroid );

		MOAIDialogAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIDialogAndroid );

		MOAIMoviePlayerAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIMoviePlayerAndroid );

		MOAIKeyboardAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIKeyboardAndroid );

#if MOAI_WITH_BOX2D
		AKUInitializeBox2D ();
#endif

#if MOAI_WITH_CHIPMUNK
		AKUInitializeChipmunk ();
#endif

#ifndef DISABLE_ADCOLONY
		MOAIAdColonyAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIAdColonyAndroid );
#endif

#ifndef DISABLE_BILLING
		MOAIBillingAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIBillingAndroid );
#endif

#ifndef DISABLE_CHARTBOOST
		MOAIChartBoostAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIChartBoostAndroid );
#endif

#ifndef DISABLE_CRITTERCISM
		MOAICrittercismAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAICrittercismAndroid );
#endif

#ifndef DISABLE_FACEBOOK
		MOAIFacebookAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAIFacebookAndroid );
#endif

#ifndef DISABLE_NOTIFICATIONS
		MOAINotificationsAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAINotificationsAndroid );
#endif

#ifndef DISABLE_TAPJOY
		MOAITapjoyAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAITapjoyAndroid );
#endif

#ifndef DISABLE_TWITTER
		MOAITwitterAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAITwitterAndroid );
#endif

#ifndef DISABLE_TSTOREWALL
		MOAITstoreWallAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAITstoreWallAndroid );
#endif

#ifndef DISABLE_TSTOREGAMECENTER
		MOAITstoreWallAndroid::Affirm ();
		REGISTER_LUA_CLASS ( MOAITstoreGamecenterAndroid );
#endif

		AKURunData ( moai_lua, moai_lua_SIZE, AKU_DATA_STRING, AKU_DATA_ZIPPED );

		inputQueue = new LockingQueue < InputEvent > ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUInitializeUtil ( JNIEnv* env, jclass obj ) {
        AKUInitializeUtil ();
    }

    //----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUInitializeSim ( JNIEnv* env, jclass obj ) {
        AKUInitializeSim ();
    }

    //----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUInitializeHttpClient ( JNIEnv* env, jclass obj ) {
        AKUInitializeHttpClient ();
    }

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUMountVirtualDirectory ( JNIEnv* env, jclass obj, jstring jvirtualPath, jstring jarchive ) {

		JNI_GET_CSTRING ( jvirtualPath, virtualPath );
		JNI_GET_CSTRING ( jarchive, archive );

		AKUMountVirtualDirectory ( virtualPath, archive );

		JNI_RELEASE_CSTRING ( jvirtualPath, virtualPath );
		JNI_RELEASE_CSTRING ( jarchive, archive );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUPause ( JNIEnv* env, jclass obj, jboolean paused ) {

		AKUPause ( paused );

		if ( paused ) {

#if MOAI_WITH_UNTZ
			AKUUntzSuspend ();
#endif
		} else {

#if MOAI_WITH_UNTZ
			AKUUntzResume ();
#endif
		}
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUReleaseGfxContext ( JNIEnv* env, jclass obj ) {

		AKUReleaseGfxContext ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKURender ( JNIEnv* env, jclass obj ) {

		AKURender ();
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUReserveInputDevices ( JNIEnv* env, jclass obj, jint total ) {

		AKUReserveInputDevices ( total );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUReserveInputDeviceSensors ( JNIEnv* env, jclass obj, jint deviceId, jint total ) {

		AKUReserveInputDeviceSensors ( deviceId, total );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKURunScript ( JNIEnv* env, jclass obj, jstring jfilename ) {

		JNI_GET_CSTRING ( jfilename, filename );

		AKURunScript ( filename );

		JNI_RELEASE_CSTRING ( jfilename, filename );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetConnectionType ( JNIEnv* env, jclass obj, jlong connectionType ) {

		MOAIEnvironment::Get ().SetValue ( MOAI_ENV_connectionType,	( int )connectionType );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetContext ( JNIEnv* env, jclass obj, jint contextId ) {

		AKUSetContext ( contextId );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetDeviceProperties ( JNIEnv* env, jclass obj, jstring jappName, jstring jappId, jstring jappVersion, jstring jabi, jstring jdevBrand, jstring jdevName, jstring jdevManufacturer, jstring jdevModel, jstring jdevProduct, jint jnumProcessors, jstring josBrand, jstring josVersion, jstring judid ) {

		JNI_GET_CSTRING ( jappName, appName );
		JNI_GET_CSTRING ( jappId, appId );
		JNI_GET_CSTRING ( jappVersion, appVersion );
		JNI_GET_CSTRING ( jabi, abi );
		JNI_GET_CSTRING ( jdevBrand, devBrand );
		JNI_GET_CSTRING ( jdevName, devName );
		JNI_GET_CSTRING ( jdevManufacturer, devManufacturer );
		JNI_GET_CSTRING ( jdevModel, devModel );
		JNI_GET_CSTRING ( jdevProduct, devProduct );
		JNI_GET_CSTRING ( josBrand, osBrand );
		JNI_GET_CSTRING ( josVersion, osVersion );
		JNI_GET_CSTRING ( judid, udid );

		MOAIEnvironment& environment = MOAIEnvironment::Get ();

		environment.SetValue ( MOAI_ENV_appDisplayName,		appName );
		environment.SetValue ( MOAI_ENV_appID,				appId );
		environment.SetValue ( MOAI_ENV_appVersion,			appVersion );
		environment.SetValue ( MOAI_ENV_cpuabi,				abi );
		environment.SetValue ( MOAI_ENV_devBrand,			devBrand );
		environment.SetValue ( MOAI_ENV_devName,			devName );
		environment.SetValue ( MOAI_ENV_devManufacturer,	devManufacturer );
		environment.SetValue ( MOAI_ENV_devModel,			devModel );
		environment.SetValue ( MOAI_ENV_devProduct,			devProduct );
		environment.SetValue ( MOAI_ENV_numProcessors,		jnumProcessors );
		environment.SetValue ( MOAI_ENV_osBrand,			osBrand );
		environment.SetValue ( MOAI_ENV_osVersion,			osVersion );
		environment.SetValue ( MOAI_ENV_udid,				udid );

		JNI_RELEASE_CSTRING ( jappName, appName );
		JNI_RELEASE_CSTRING ( jappId, appId );
		JNI_RELEASE_CSTRING ( jappVersion, appVersion );
		JNI_RELEASE_CSTRING ( jabi, abi );
		JNI_RELEASE_CSTRING ( jdevBrand, devBrand );
		JNI_RELEASE_CSTRING ( jdevName, devName );
		JNI_RELEASE_CSTRING ( jdevManufacturer, devManufacturer );
		JNI_RELEASE_CSTRING ( jdevModel, devModel );
		JNI_RELEASE_CSTRING ( jdevProduct, devProduct );
		JNI_RELEASE_CSTRING ( josBrand, osBrand );
		JNI_RELEASE_CSTRING ( josVersion, osVersion );
		JNI_RELEASE_CSTRING ( judid, udid );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetDocumentDirectory ( JNIEnv* env, jclass obj, jstring jpath ) {

		JNI_GET_CSTRING ( jpath, path );

		MOAIEnvironment::Get ().SetValue ( MOAI_ENV_documentDirectory,	path );

		JNI_RELEASE_CSTRING ( jpath, path );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputConfigurationName ( JNIEnv* env, jclass obj, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputConfigurationName ( name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputDevice ( JNIEnv* env, jclass obj, jint deviceId, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputDevice ( deviceId, name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputDeviceCompass ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputDeviceCompass ( deviceId, sensorId, name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputDeviceLevel ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputDeviceLevel ( deviceId, sensorId, name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputDeviceLocation ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputDeviceLocation ( deviceId, sensorId, name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetInputDeviceTouch ( JNIEnv* env, jclass obj, jint deviceId, jint sensorId, jstring jname ) {

		JNI_GET_CSTRING ( jname, name );

		AKUSetInputDeviceTouch ( deviceId, sensorId, name );

		JNI_RELEASE_CSTRING ( jname, name );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetScreenDpi ( JNIEnv* env, jclass obj, jint dpi ) {

		AKUSetScreenDpi ( dpi );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetScreenSize ( JNIEnv* env, jclass obj, jint width, jint height ) {

		AKUSetScreenSize ( width, height );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetViewSize ( JNIEnv* env, jclass obj, jint width, jint height ) {

		AKUSetViewSize ( width, height );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetWorkingDirectory ( JNIEnv* env, jclass obj, jstring jpath ) {

		JNI_GET_CSTRING ( jpath, path );

		ZLFileSys::SetCurrentPath ( path );
		MOAILuaRuntime::Get ().SetPath ( path );

		JNI_RELEASE_CSTRING ( jpath, path );
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUUntzInit ( JNIEnv* env, jclass obj ) {

#if MOAI_WITH_UNTZ
		AKUInitializeUntz ();
#endif
	}

	//----------------------------------------------------------------//
	extern "C" void Java_com_ziplinegames_moai_Moai_AKUUpdate ( JNIEnv* env, jclass obj ) {

		InputEvent ievent;
		while ( inputQueue->Pop ( ievent )) {

			switch ( ievent.m_type ) {

			case InputEvent::INPUTEVENT_TOUCH:
				AKUEnqueueTouchEvent ( ievent.m_deviceId, ievent.m_sensorId, ievent.m_touchId, ievent.m_down, ievent.m_x, ievent.m_y );
				break;
			case InputEvent::INPUTEVENT_LEVEL:
				AKUEnqueueLevelEvent ( ievent.m_deviceId, ievent.m_sensorId, ievent.m_x, ievent.m_y, ievent.m_z );
				break;
			case InputEvent::INPUTEVENT_COMPASS:
				AKUEnqueueCompassEvent ( ievent.m_deviceId, ievent.m_sensorId, ievent.m_heading );
				break;
			case InputEvent::INPUTEVENT_LOCATION:
				AKUEnqueueLocationEvent ( ievent.m_deviceId, ievent.m_sensorId, ievent.m_longitude, ievent.m_latitude, ievent.m_altitude, ievent.m_hAccuracy, ievent.m_vAccuracy, ievent.m_speed );
				break;
			}
		}

		AKUUpdate ();
	}

	extern "C" void Java_com_ziplinegames_moai_Moai_AKUSetDeviceLocale ( JNIEnv* env, jclass obj, jstring jlangCode, jstring jcountryCode ) {
		JNI_GET_CSTRING ( jlangCode, langCode );
		JNI_GET_CSTRING ( jcountryCode, countryCode );

		MOAIEnvironment& environment = MOAIEnvironment::Get ();

		environment.SetValue ( MOAI_ENV_languageCode, langCode );
		environment.SetValue( MOAI_ENV_countryCode, countryCode );

		JNI_RELEASE_CSTRING ( jlangCode, langCode );
		JNI_RELEASE_CSTRING ( jcountryCode, countryCode );
	}
