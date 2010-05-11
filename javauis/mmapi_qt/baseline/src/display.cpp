#include "logger.h"
#include "com_nokia_microedition_media_control_BaseDisplay.h"
#include "cmmadisplay.h"

/*
 * Class:     com_nokia_microedition_media_control_BaseDisplay
 * Method:    _setVisible
 * Signature: (IIZ)I
 */
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_BaseDisplay__1nativeMethodCallInUiThread
  (JNIEnv *, jobject, jint nativeDisplayHandle, jint nativeFunctionID)
  {
   LOG(EJavaMMAPI,EInfo,"JNI_Display.cpp : nativeMethodCallInUiThread +");
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
   display ->CalledBackInUiThread(nativeFunctionID);
   LOG(EJavaMMAPI,EInfo,"JNI_Display.cpp : nativeMethodCallInUiThread -");
  }

