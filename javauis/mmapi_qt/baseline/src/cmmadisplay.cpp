/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class implements MMMADisplay
*
*/


//  Include Files
#include "logger.h"

#include "cmmadisplay.h"
#include "mmmadisplaywindow.h"

// temporary
#include <w32std.h>

// Destructor (virtual by CBase)
CMMADisplay::~CMMADisplay()
{
/*
    if (iDirectContainer)
    {
        // Remove clip rect if set
        if (!iClipRect.IsEmpty())
        {
            LOG(EJavaMMAPI,EInfo,"CMMADisplay::~CMMADisplay(): Removing clip rect");
            iDirectContainer->MdcRemoveContentBounds(iClipRect);
        }

        // Remove this object from MDirectContainer
        iDirectContainer->MdcRemoveContent(this);
    }
    */
}


CMMADisplay::CMMADisplay():
        iVisible(ETrue),
        iFullScreen(EFalse),
        iContainerVisible(ETrue),
        iIsForeground(ETrue),
        iResetDrawRect(EFalse)
{
}

void CMMADisplay::Construct(MMAFunctionServer* eventSource , jobject javadisplayref)
{
	iEventSource = eventSource;
	iJni = iEventSource->getValidJniEnv();
	iJavaDisplayObject = iJni->NewGlobalRef(javadisplayref);
	iJavaDisplayClass = iJni->GetObjectClass(iJavaDisplayObject);
    // Components must have direct content.
   /* __ASSERT_LOG(EJavaMMAPI,EInfo,"aDirectContainer, User::Invariant());

    // Not owned
    iDirectContainer = aDirectContainer;

    // Get component visibility. Later visibility changes will
    // be informed through MDirectContent observer.
    iContainerVisible = iDirectContainer->MdcContainerVisibility();
    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::Construct iContainerVisible = %d", iContainerVisible);
    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::Construct iVisible = %d", iVisible);

    // Add this MDirectContent to the MDirectContainer. Cannot fail.
    iDirectContainer->MdcAddContent(this);
    */
}

TRect CMMADisplay::ScaleToFullScreen(const TSize& aFullScreenSize,
                                     const TSize& aSourceSize)
{
	LOG(EJavaMMAPI,EInfo,"CMMADisplay::ScaleToFullScreen() +");
    if ((aSourceSize.iWidth == 0) &&
            (aSourceSize.iHeight == 0))
    {
        // Avoid divide by zero.
        return TRect();
    }

    // Width or height will be full size.
    TRect drawRect(TPoint(0, 0),
                   aFullScreenSize);

    // Smaller dimension scale ratio will be scaled.
    // Changed to area calculation to avoid reals and dimension
    // with smaller area will be scaled.
    TInt vDif((aFullScreenSize.iWidth - aSourceSize.iWidth)
              * aFullScreenSize.iHeight);
    TInt hDif((aFullScreenSize.iHeight - aSourceSize.iHeight)
              * aFullScreenSize.iWidth);

    TPoint position(0, 0);

    // Check which side to scale to fullscreen size.
    if (hDif > vDif)
    {
        // Width is full screen width.
        // Scale height with aspect ratio.
        drawRect.iBr.iY = aFullScreenSize.iWidth * aSourceSize.iHeight
                          / aSourceSize.iWidth;
        // move rect middle of the screen
        position.iY = (aFullScreenSize.iHeight - drawRect.iBr.iY) / 2;
    }
    else
    {
        // Height is full screen height.
        // Scale width with aspect ratio.
        drawRect.iBr.iX = aFullScreenSize.iHeight * aSourceSize.iWidth
                          / aSourceSize.iHeight;
        // move rect middle of the screen
        position.iX = (aFullScreenSize.iWidth - drawRect.iBr.iX) / 2;
    }

    drawRect.Move(position);
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::ScaleToFullScreen() -");
    return drawRect;
}

// from MMMADisplay
void CMMADisplay::DrawFrameL(const CFbsBitmap* aBitmap)
{
    // This method is called only if bitmap is used.
    // Runs in mmapi thread
    if (iVisible && iWindow /*&& iDirectContainer*/)
    {
        iWindow->DrawFrameL(aBitmap);
        //iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
    }
}

void CMMADisplay::SetClippingRegion()
{
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion");

    if (!iWindow ||
            (iWindow->GetDisplayWindowType() == MMMADisplayWindow::EDisplayWindowTypeIsBitmap) ||
            iClipRect == iWindow->DrawRect() && iContainerVisible && iVisible)
    {
    	 LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion before return");
    	     return;
    }

    TBool refreshScreen(EFalse);
    // Remove first the current clip rect if set
   LOG2(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: iClipRect = %d X %d",iClipRect.Width(),iClipRect.Height());
    if (!iClipRect.IsEmpty())
    {
        LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: Removing old rect");
        //iDirectContainer->MdcRemoveContentBounds(iClipRect);
        iClipRect.SetRect(0, 0, 0, 0);
        refreshScreen = ETrue;
    }
    // If visible then set a new clip rect
    if (iVisible && iContainerVisible)
    {
    	LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: inside if (iVisible && iContainerVisible)");
        iClipRect = iWindow->DrawRect();
        LOG2(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: inside if (iVisible && iContainerVisible)iClipRect = %d X %d",iClipRect.Width(),iClipRect.Height());
        if (!iClipRect.IsEmpty())
        {
            LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: Adding new rect");
            // Add new clipping rect
         //   iDirectContainer->MdcAddContentBounds(iClipRect);
         SetContentBoundToJavaControl(iClipRect);
            refreshScreen = ETrue;
        }
    }
    if (refreshScreen)
    {
    	LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion: inside if(refreshScreen)");
        // refresh screen
       // iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
        RefreshJavaControl(iWindow->WindowRect());
    }
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetClippingRegion -");
}

void CMMADisplay::RemoveClippingRegion()
{
    // Called in mmapi thread
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::RemoveClippingRegion");

    if (!iWindow ||
            (iWindow->GetDisplayWindowType() == MMMADisplayWindow::EDisplayWindowTypeIsBitmap))
    {
        return;
    }
    // Remove first the current clip rect if set
    if (!iClipRect.IsEmpty())
    {
        LOG(EJavaMMAPI,EInfo,"CMMADisplay::RemoveClippingRegion: Removing old rect");
        //iDirectContainer->MdcRemoveContentBounds(iClipRect);
        RemoveContentBoundFromJavaControl(iClipRect);
        iClipRect.SetRect(0, 0, 0, 0);
        // refresh screen
        //iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
        RefreshJavaControl(iWindow->WindowRect());
    }
}

void CMMADisplay::AddClippingRegion()
{
    // Called in mmapi thread
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::AddClippingRegion");
		LOG2(EJavaMMAPI,EInfo,"CMMADisplay::AddClippingRegion iClipRect width = %d height = %d", iClipRect.Width() , iClipRect.Height());
		  
    if (!iWindow ||
            (iWindow->GetDisplayWindowType() ==
             MMMADisplayWindow::EDisplayWindowTypeIsBitmap) ||
            iClipRect == iWindow->DrawRect())
    {
        return;
    }
    // If visible then set a new clip rect
    if (iVisible)
    {
        iClipRect = iWindow->DrawRect();
        if (!iClipRect.IsEmpty())
        {
            LOG(EJavaMMAPI,EInfo,"CMMADisplay::AddClippingRegion: Adding new rect");
            // Add new clipping rect
          // iDirectContainer->MdcAddContentBounds(iClipRect);
          SetContentBoundToJavaControl(iClipRect);
            // refresh screen
           // iDirectContainer->MdcFlushContainer(
           //     iWindow->WindowRect());
           RefreshJavaControl(iWindow->WindowRect());
        }
    }
}

// from MMMADisplay
TSize CMMADisplay::DisplaySize()
{
    if (iWindow && iFullScreen)
    {
        return iWindow->DrawRect().Size();
    }
    else
    {
        return iUserRect.Size();
    }
}

// from MMMADisplay
void CMMADisplay::SetDisplaySizeL(const TSize& aSize)
{
    // user rect contains size set from java.
 iUserRect.SetSize(aSize);
 LOG2(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetDisplaySizeL iUserRect = %d X %d", iUserRect.Width() ,iUserRect.Height());
    // Size change has no effect if fullscreen mode is on.
    // New size could be used when fullscreen is turned off.
    if (iContainerVisible && !iFullScreen && iWindow)
    {
    	 	LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetVisible + iContainerVisible = %d", iContainerVisible);
        RemoveClippingRegion();
        LOG2(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetDisplaySizeL iUserRect after removingClipRegion = %d X %d", iUserRect.Width() ,iUserRect.Height());
        iWindow->SetDrawRect(iUserRect);
        AddClippingRegion();
        LOG2(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetDisplaySizeL iUserRect after AddingClipRegion = %d X %d", iUserRect.Width() ,iUserRect.Height());
    }
    else
    {
    	 LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetVisible + iContainerVisible = %d", iContainerVisible);
        iResetDrawRect = ETrue;
    }
}

// from MMMADisplay
void CMMADisplay::SetVisible(TBool aValue)
{
    iVisible = aValue;
    // Window may be set visible only if container is on screen,

    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetVisible + iContainerVisible = %d", iContainerVisible);
    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetVisible iVisible = %d", iVisible);
    if (!iIsForeground)
    {
        LOG(EJavaMMAPI,EInfo,"MID::CMMADisplay::SetVisible - iIsForeground = 0");
        return;
    }
    // if not it can not be set visible.
    if (iWindow && iContainerVisible)
    {
       // iWindow->SetVisible(aValue, EFalse);
       //MMAPI UI 3.x req. (had to comment above line and add below line which excutes in FS thread)
        iWindow->SetVisible(aValue, ETrue);
        SetClippingRegion();
    }
}


void CMMADisplay::SetContainerVisibility(TBool aValue)
{
	iContainerVisible = aValue;
	// TODO : delete the below check once implementation is done as it is used only for testing purpose
	if(iContainerVisible)
	LOG(EJavaMMAPI,EInfo,"CMMADisplay:SetContainerVisibility(): Container is visible ");
	else
	LOG(EJavaMMAPI,EInfo,"CMMADisplay:SetContainerVisibility(): Container is not visible ");

	HandleContainerVisibilityChanged(aValue);
}
// from MMMADisplay
void CMMADisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    // Sets new window. Ownership is not transferred.
    iWindow = aWindow;
}

// from MMMADisplay
MMMADisplayWindow* CMMADisplay::Window()
{
    return iWindow;
}

TBool CMMADisplay::IsVisible()
{
    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::IsVisible iContainerVisible = %d", iContainerVisible);
    LOG1(EJavaMMAPI,EInfo,"MID::CMMADisplay::IsVisible iVisible = %d", iVisible);
    // display is visible if container is on screen and
    // java side has set it visible
    return iVisible && iContainerVisible;
}

TBool CMMADisplay::IsFullScreen()
{
    return iFullScreen;
}

TBool CMMADisplay::HasContainer()
{
   // return iDirectContainer != NULL;
   return EFalse;   // temporary
}


void CMMADisplay::HandleContainerVisibilityChanged( TBool aVisible )
    {
    LOG1(EJavaMMAPI,EInfo, "CMMADisplay::HandleContainerVisibilityChanged aVisible = %d",
      					                                            aVisible );
    if(!iIsForeground && aVisible)
        {
        LOG(EJavaMMAPI,EInfo," CMMADisplay::HandleContainerVisibilityChanged Condition 1 ");
        return;
        }

    LOG(EJavaMMAPI,EInfo," CMMADisplay::HandleContainerVisibilityChanged After condition1");

    if (iWindow)
    {
        // midlet comes to foreground (0 to 1 transition),
        // Allow turn on or turn off based on aVisible
        if (aVisible != iWindow->IsVisible())
        {
            // Allow
            LOG(EJavaMMAPI,EInfo,"MID::CMMADisplay::MdcContainerVisibilityChanged Allow ");
        }
        else if( iContainerVisible == aVisible )
        {
            LOG(EJavaMMAPI,EInfo,"MID::CMMADisplay::MdcContainerVisibilityChanged iContainerVisible == aVisible ");
            // if state is not changed, we do not need to do it again
            return;
        }
    }

    LOG(EJavaMMAPI,EInfo,"MID::CMMADisplay::MdcContainerVisibilityChanged After condition2");

    if (iWindow)
    {
        LOG(EJavaMMAPI,EInfo,"MID::CMMADisplay::MdcContainerVisibilityChanged iWindow is valid ");
        // change is only needed if java side has set display visible or
        // if container loses focus
        if (!iContainerVisible || iVisible)
        {
            if (iResetDrawRect && aVisible && !iFullScreen)
            {
                iWindow->SetDrawRectThread(iUserRect);
                iResetDrawRect = EFalse;
            }
            if (iIsForeground)
            {
                iWindow->SetVisible(aVisible);
            }
            SetClippingRegion();
        }
    }
}

/*

void CMMADisplay::MdcItemContentRectChanged(const TRect& aContentRect,
        const TRect& aScreenRect)
{
    // To be overwritten if used
    __ASSERT_LOG(EJavaMMAPI,EInfo,"EFalse, User::Invariant());
}

void CMMADisplay::MdcContainerWindowRectChanged(const TRect&
	                                              #ifdef RD_JAVA_NGA_ENABLED
	                                               aRect
	                                              #endif
	                                              )
    {
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::MdcContainerWindowRectChanged +");

    #ifdef RD_JAVA_NGA_ENABLED
    if( iWindow )
    	{
    	LOG(EJavaMMAPI,EInfo,"CMMADisplay::MdcContainerWindowRectChanged, SetRWindowRect");
    	iWindow->SetRWindowRect(aRect, MMMADisplay::EUiThread);
    	}
    #endif
    }
void CMMADisplay::MdcContainerDestroyed()
{
    LOG(EJavaMMAPI,EInfo,"MMA::CMMADisplay::MdcContainerDestroyed");

    if (iWindow)
    {
        iWindow->ContainerDestroyed();
    }

    iDirectContainer = NULL;
}


void CMMADisplay::MdcAbortDSA()
{
    if (iWindow)
    {
        iWindow->AbortDSA();
    }
}


void CMMADisplay::MdcResumeDSA()
{
    if (iWindow)
    {
        iWindow->ResumeDSA();
    }
}

void CMMADisplay::UIGetCallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    if (iDirectContainer)
    {
        iDirectContainer->MdcGetUICallback(aConsumer, aCallbackId);
    }
}
*/


void CMMADisplay::GetCallbackInUiThread(TInt placeholder)
{
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::GetCallbackInUiThread +");








	 jmethodID getCallBackMethodID = iJni->GetMethodID(
                                         iJavaDisplayClass,
                                         "GetCallbackInUiThread",
                                         "(I)V");
   // LOG1(EJavaMMAPI,EInfo,"CMMADisplay::GetCallbackInUiThread getCallBackMethodID = %d",getCallBackMethodID);
    iJni->CallVoidMethod(iJavaDisplayObject,getCallBackMethodID,placeholder);
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::GetCallbackInUiThread -");
}

void CMMADisplay::CalledBackInUiThread(TInt placeholder)
{
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::CalledBackInUiThread +");
		iWindow->UICallback(placeholder);
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::CalledBackInUiThread -");

}

void CMMADisplay::SourceSizeChanged(TInt aJavaControlWidth, TInt aJavaControlHeight)
	{
		LOG2(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged(aSourceSize,aJavaControlWidth,aJavaControlHeight) + aJavaControlWidth = %d ,aJavaControlHeight = %d",aJavaControlWidth,aJavaControlHeight);
		iSourceSize = SourceSize();
		LOG2(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged(aSourceSize,aJavaControlWidth,aJavaControlHeight) + sourcesize = %d X %d",iSourceSize.iWidth , iSourceSize.iHeight);
		#ifdef RD_JAVA_NGA_ENABLED
    if ( iWindow )
			{
    	iWindow->SetVideoCropRegion( TRect( iUserRect.iTl, iSourceSize ) );
    	}
    #endif

    // size of canvas in java
    TSize canvasSize(aJavaControlWidth, aJavaControlHeight);
    fullScreenSize = canvasSize;
    TBool sourceIsBigger = (iSourceSize.iWidth > fullScreenSize.iWidth ||
                            iSourceSize.iHeight > fullScreenSize.iHeight);

    if (sourceIsBigger)
    {
        // Source is larger than display area.
        // Shrink draw are to fit in display.
        iWindow->SetDrawRect(ScaleToFullScreen(fullScreenSize, iSourceSize));
    }
    else
    {
        // source is smaller than display area
        iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));
    }

    SetClippingRegion();

    if (iUserRect.IsEmpty())
    {
        // Java side hasn't set size.
        iUserRect = iWindow->DrawRect();

        if (!sourceIsBigger)
        {
            // Addjusting rect to top left corner.
            iUserRect = TRect(iUserRect.Size());
        }
    }
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged(aSourceSize,aJavaControlWidth,aJavaControlHeight) -");
	}

void CMMADisplay::SetWindowResources(QWidget* qtWidget)
	{
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetWindowResources +");
		CCoeControl* control = 0;
		if(qtWidget && qtWidget ->winId())
			{
				control = reinterpret_cast<CCoeControl*>(qtWidget->winId());
			}

		CCoeEnv *coeEnv = control->ControlEnv();
		RWsSession * iWs = &(coeEnv->WsSession());
		CWsScreenDevice* iScreenDevice = coeEnv->ScreenDevice();
    RWindowBase* window = static_cast<RWindowBase*>(control->DrawableWindow());
    if (!iWindow)
    {
       return;
    }
     iWindow->ProcureWindowResourcesFromQWidget(iWs,iScreenDevice,window);
     LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetWindowResources -");
	}


void CMMADisplay::SetForeground(TBool aForeground, TBool aUseEventServer)
{
    LOG1(EJavaMMAPI,EInfo,"MMA::CMMADisplay::SetForeground %d", aForeground);
    iIsForeground = aForeground;

    if (aForeground)
    {
        if (iContainerVisible && !iWindow->IsVisible())
        {
            iWindow->SetVisible(ETrue, aUseEventServer);
        }
    }
    else
    {
        if (iWindow->IsVisible())
        {
            iWindow->SetVisible(EFalse, aUseEventServer);
        }
    }
}

TSize CMMADisplay::SourceSize()
	{
		LOG(EJavaMMAPI,EInfo,"CMMADisplay::SetSourceSizeToDisplay ");
		 return iUiPlayer->SourceSize();
	}
	
void CMMADisplay::SetUIPlayer(MMMAGuiPlayer* player)
	{
		iUiPlayer = player;
	}

	
void CMMADisplay::SourceSizeChanged(const TSize& aSourceSize)
{
    LOG(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged");
    
		#ifdef RD_JAVA_NGA_ENABLED
    if ( iWindow )
			{
    	iWindow->SetVideoCropRegion( TRect( iUserRect.iTl, aSourceSize ) );
    	}
    #endif

    iSourceSize = aSourceSize;
    jmethodID getDisplayWidthID = iJni->GetMethodID(
                                         iJavaDisplayClass,
                                         "getDisplayWidth",
                                         "()I");

    jmethodID getDisplayHeightID = iJni->GetMethodID(
                                         iJavaDisplayClass,
                                         "getDisplayHeight",
                                         "()I");


    TInt x =  iJni->CallIntMethod(iJavaDisplayObject,getDisplayWidthID);
    TInt y = iJni->CallIntMethod(iJavaDisplayObject,getDisplayHeightID);
		LOG2(EJavaMMAPI,EInfo,"CMMADisplay.cpp : SourceSizeChanged () fullScreenSize is x = %d ,y = %d ",x,y);
		// get the ScreenSize from canvas in java
    TSize canvasSize(x, y);
    fullScreenSize = canvasSize;
    TBool sourceIsBigger = (aSourceSize.iWidth > fullScreenSize.iWidth ||
                            aSourceSize.iHeight > fullScreenSize.iHeight);

    if (sourceIsBigger)
    {
        // Source is larger than display area.
        // Shrink draw are to fit in display.
        iWindow->SetDrawRect(ScaleToFullScreen(fullScreenSize, iSourceSize));
    }
    else
    {
        // source is smaller than display area
        iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));
    }

    SetClippingRegion();

    if (iUserRect.IsEmpty())
    {
        // Java side hasn't set size.
        iUserRect = iWindow->DrawRect();

        if (!sourceIsBigger)
        {
            // Addjusting rect to top left corner.
            iUserRect = TRect(iUserRect.Size());
        }
    }
}



void CMMADisplay::SetDisplayPosition(TInt uiControlLocationX,TInt uiControlLocationY,TInt	videoControlLocationX,TInt videoControlLocationY)
{
	 	LOG(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged + ");
		TPoint point(videoControlLocationX ,videoControlLocationY);
		//calling derived class fuction
	 	SetDisplayLocationL(point);
	  LOG(EJavaMMAPI,EInfo,"CMMADisplay::SourceSizeChanged - ");
}


void CMMADisplay::ResetJavaRectObject(const TRect& aRect)
	{
	LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject +");
	JNIEnv* validJni = iEventSource->getValidJniEnv();
  jmethodID setRectID = validJni->GetMethodID(
	                                         iJavaDisplayClass,
	                                         "setRect",
                                         "()V");

  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject --1");
  // set the value to java,so that we can access those from array
  jfieldID field = validJni->GetFieldID(iJavaDisplayClass, "rectDimension", "[I");
  if(field == NULL)
  {
	  // handle error
  }
  /* Write to the instance fields */
  jintArray javaDimensionarr = (jintArray)validJni->GetObjectField(iJavaDisplayObject, field);
  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject --2");
  jint* nativeRectDimensionArr = validJni->GetIntArrayElements(javaDimensionarr, NULL);
  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject --3");
    if (!nativeRectDimensionArr)
    {    // inputBuffer was already allocated
        validJni->ReleaseIntArrayElements(javaDimensionarr, nativeRectDimensionArr, JNI_ABORT);
        LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject Error in resetting rect dimension to java");
        return;
    }
  nativeRectDimensionArr[0] = aRect.iTl.iX;
  nativeRectDimensionArr[1] = aRect.iTl.iY;;
  nativeRectDimensionArr[2] = aRect.Width();
  nativeRectDimensionArr[3] = aRect.Height();
   // Now the dimension array in java is updated hence reset the java rect
   validJni->CallVoidMethod(iJavaDisplayObject,setRectID);
    
   validJni->ReleaseIntArrayElements(javaDimensionarr, nativeRectDimensionArr, JNI_COMMIT);
   LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::ResetJavaRectObject -");
}


void CMMADisplay::SetContentBoundToJavaControl(const TRect& aRect)
	{
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::SetContentBoundToJavaControl +");
		JNIEnv* validJni = iEventSource->getValidJniEnv();
		// Reset the java rect
		ResetJavaRectObject(aRect);
		jmethodID setContentBoundID = validJni->GetMethodID(
	                                         iJavaDisplayClass,
	                                         "setContentBound",
                                         "()V");
    // call java function                                     
		validJni->CallVoidMethod(iJavaDisplayObject,setContentBoundID);
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::SetContentBoundToJavaControl -");
	}
	
void CMMADisplay::RemoveContentBoundFromJavaControl(const TRect& aRect)
	{
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::RemoveContentBoundFromJavaControl +");
		JNIEnv* validJni = iEventSource->getValidJniEnv();
		// Reset the java rect
		ResetJavaRectObject(aRect);
		jmethodID removeContentBoundID = validJni->GetMethodID(
	                                         iJavaDisplayClass,
	                                         "removeContentBound",
                                         "()V");
    // call java function                                     
		validJni->CallVoidMethod(iJavaDisplayObject,removeContentBoundID);
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::RemoveContentBoundFromJavaControl -");
	}	
	
void CMMADisplay::RefreshJavaControl(const TRect& aRect)
	{
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::RefreshJavaControl +");
		JNIEnv* validJni = iEventSource->getValidJniEnv();
		// Reset the java rect
		//ResetJavaRectObject(aRect);
		jmethodID redrawControlID = validJni->GetMethodID(
	                                         iJavaDisplayClass,
	                                         "redrawControl",
                                         "()V");
    // call java function                                     
		validJni->CallVoidMethod(iJavaDisplayObject,redrawControlID);
		LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::RefreshJavaControl -");
	}		
	
//  END OF FILE
