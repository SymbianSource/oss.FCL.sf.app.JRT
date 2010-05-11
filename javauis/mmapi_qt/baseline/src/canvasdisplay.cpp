#include "logger.h"
#include "com_nokia_microedition_media_control_MMACanvasDisplay.h"
#include "cmmacanvasdisplay.h"
#include "qwidget.h"
#include "cmmasurfacewindow.h"


/*
 * Class:     com_nokia_microedition_media_control_mmacanvasdisplay
 * Method:    _setVisible
 * Signature: (IIZ)I
 */
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setVisible
  (JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isVisible)
  {
   LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setVisible +");
	    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
        TBool visible = (TBool)isVisible;
        display ->SetVisible(visible);
   LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setVisible -");
  }

LOCAL_C void LocalSetContainerVisible(CMMADisplay* display,TBool visible)
{
	 display ->SetContainerVisibility(visible);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setContainerVisible
(JNIEnv *, jobject,jint aEventSourceHandle, jint nativeDisplayHandle, jboolean isActive)
{
  LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setContainerVisible +");
  MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
	 // need to call in Function server thread context because CMMADisplay's JNI pointer
	 // is valid in that thread context only.
  
		    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	        TBool visible = (TBool)isActive;
	        
	         TInt error = eventSource->ExecuteTrap(&LocalSetContainerVisible,
					                                          display,
					                                          		visible);
	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setContainerVisible -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setFullScreenMode
(JNIEnv *, jobject, jint nativeDisplayHandle, jboolean isFullScreen)
{
  LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setFullScreenMode  +");
		    CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	        TBool mode = (TBool)isFullScreen;
	        display ->SetFullScreenL(mode);
  LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setFullScreenMode  -");
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setWindowToNative
(JNIEnv *, jobject,jint nativeDisplayHandle, jint qtWidgetHandle)
{
	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setWindowToNative + ");
	 QWidget *widget = reinterpret_cast<QWidget*>(qtWidgetHandle);
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	 display->SetWindowResources(widget);
	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setWindowToNative - ");
}


/*
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1nativeDummy
(JNIEnv *, jobject,)
{
*/

LOCAL_C void LocalSourceSizeChanged(CMMADisplay* display,TInt width,TInt height)
{
	display->SourceSizeChanged(width,height);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setNativeWindowSize
(JNIEnv *, jobject,jint nativeDisplayHandle, jint aEventSourceHandle, jint eswtControlwidth,jint eswtControlheight)
{
	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setNativeWindowSize + ");
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	 TInt width = eswtControlwidth;
	 TInt height = eswtControlheight;
	 MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
	 // need to call in Function server thread context because CMMADisplay's JNI pointer
	 // is valid in that thread context only.
   TInt error = eventSource->ExecuteTrap(&LocalSourceSizeChanged,
					                                          display,
					                                          		width,
					                                          			height);

	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : setNativeWindowSize - ");
}

LOCAL_C void LocalSetDisplayPosition(CMMADisplay* display,TInt uiX,TInt uiY,TInt vcX,TInt vcY)
{
	display->SetDisplayPosition(uiX,uiY,vcX,vcY);
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setPosition
(JNIEnv *, jobject,jint nativeDisplayHandle,jint aEventSourceHandle,jint uiX,jint uiY,jint vcX,jint vcY)
{
	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setPosition + ");
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	 TInt uiControlLocationX = uiX;
	 TInt uiControlLocationY = uiY;
	 TInt videoControlLocationX = vcX;
	 TInt videoControlLocationY = vcY;
	 MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
	 // need to call in Function server thread context because CMMADisplay's JNI pointer
	 // is valid in that thread context only.
   TInt error = eventSource->ExecuteTrap(&LocalSetDisplayPosition,
					                                          display,
					                                          		uiControlLocationX,
					                                          				uiControlLocationY,
					                                          						videoControlLocationX,
					                                          								videoControlLocationY);

	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setPosition - ");
}




LOCAL_C void LocalSetDisplaySize(CMMADisplay* display,TInt vcX,TInt vcY)
{
	LOG2(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : LocalSetDisplaySize vcX = %d,vcY=%d ",vcX,vcY);
	TSize size(vcX,vcY);
	display->SetDisplaySizeL(size);
}
		 																										
JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_MMACanvasDisplay__1setDisplaySize
(JNIEnv *, jobject,jint nativeDisplayHandle,jint aEventSourceHandle,jint width,jint height)
{
	 LOG2(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setDisplaySize +  width = %d,height = %d",width,height);
	 CMMADisplay* display = reinterpret_cast<CMMADisplay*>(nativeDisplayHandle);
	 TInt vc_width = width;
	 TInt vc_height = height;
	 MMAFunctionServer* eventSource =  reinterpret_cast<MMAFunctionServer *>(aEventSourceHandle);
	 // need to call in Function server thread context because CMMADisplay's JNI pointer
	 // is valid in that thread context only.
	 LOG2(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setDisplaySize +  vc_width = %d,vc_height = %d",vc_width,vc_height);
   TInt error = eventSource->ExecuteTrap(&LocalSetDisplaySize,
					                                          display,
					                                          		vc_width,
					                                          				vc_height);

	 LOG(EJavaMMAPI,EInfo,"JNI_canvasdisplay.cpp : _setDisplaySize - ");
}		 																												