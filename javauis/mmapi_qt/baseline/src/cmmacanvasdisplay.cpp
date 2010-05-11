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
* Description:  Draws to Canvas.
*
*/


//  Include Files
#include <logger.h>
//#include <lcdui.h> // MMIDCanvas

#ifdef EXTENDED_LCDUI_CANVAS
#include <MMIDCanvasExtended.h>
#endif

#include "cmmacanvasdisplay.h"
#include "cmmabitmapwindow.h"
#include "mmafunctionserver.h"

// CONSTRUCTION
// Static constructor, leaves pointer to cleanup-stack
CMMACanvasDisplay* CMMACanvasDisplay::NewLC(MMAFunctionServer* aEventSource, jobject obj/*MMIDCanvas* aCanvas*/)
{
	CMMACanvasDisplay* self =
        new(ELeave) CMMACanvasDisplay(aEventSource, obj/*aCanvas*/);

    CleanupStack::PushL(self);
    self->Construct(aEventSource,obj);
    return self;
}

// Destructor (virtual by CBase)
CMMACanvasDisplay::~CMMACanvasDisplay()
{
}

CMMACanvasDisplay::CMMACanvasDisplay(MMAFunctionServer* aEventSource ,jobject aJavaDisplayRef)
{
	/*
iJni = aEventSource->getValidJniEnv();
javaDisplayObject = iJni->NewGlobalRef(javadisplayref);
javaDisplayClass = iJni->GetObjectClass(javaDisplayObject);
*/
}


/*
void CMMACanvasDisplay::SourceSizeChanged(const TSize& aSourceSize)
{
    LOG(EJavaMMAPI,EInfo,"CMMACanvasDisplay::SourceSizeChanged");

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

   // TSize fullScreenSize(100,100);    // TO-Do remove hardcoded with the relevent one
	LOG2(EJavaMMAPI,EInfo,"CMMACanvasdisplay.cpp : SourceSizeChanged () fullScreenSize is x = %d ,y = %d ",x,y);
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

*/

void CMMACanvasDisplay::SetFullScreenL(TBool aFullScreen)
{
	LOG(EJavaMMAPI,EInfo,"CMMACanvasDisplay::SetFullScreenL +");
    iFullScreen = aFullScreen;
    if (iContainerVisible)
    {
        RemoveClippingRegion();

        if (aFullScreen)
        {
            // use new scaled rect
            // iWindow->SetDrawRect(ScaleToFullScreen(fullScreenSize, iSourceSize));
            iWindow->SetDrawRectThread(ScaleToFullScreen(fullScreenSize, iSourceSize));
		}
        else
        {
			// use size set from java
            //iWindow->SetDrawRect(iUserRect);
            iWindow->SetDrawRectThread(iUserRect);
        }

        AddClippingRegion();
    }
    LOG(EJavaMMAPI,EInfo,"CMMACanvasDisplay::SetFullScreenL +");
}

void CMMACanvasDisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    LOG( EJavaMMAPI, EInfo, "CMMACanvasDisplay::SetWindowL");
    CMMADisplay::SetWindowL(aWindow);
    if (!iWindow)
    {
        LOG( EJavaMMAPI, EInfo, "CMMACanvasDisplay::SetWindowL: NULL window, returning");
        return;
    }
	/*
    CFbsBitmap* bitmap = iCanvas->FrameBuffer();

    __ASSERT_DEBUG(bitmap,
                   User::Panic(_L("Canvas has no bitmap"),
                               KErrNotFound));

    iWindow->SetDestinationBitmapL(bitmap);

    // Check that container exists
    User::LeaveIfNull(iDirectContainer);

    LOG2( EJavaMMAPI, EInfo, "MMA::CMMACanvasDisplay::SetWindowL iDirectContainer->MdcContentBounds() TL %d %d", iDirectContainer->MdcContentBounds().iTl.iX, iDirectContainer->MdcContentBounds().iTl.iY);
    LOG2( EJavaMMAPI, EInfo, "MMA::CMMACanvasDisplay::SetWindowL iDirectContainer->MdcContentBounds() BR %d %d", iDirectContainer->MdcContentBounds().iBr.iX, iDirectContainer->MdcContentBounds().iBr.iY);
	*/
	LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::before calling  BoundRect");
	TRect boundrect = BoundRect();
    iWindow->SetWindowRect(boundrect/*iDirectContainer->MdcContentBounds()*/, MMMADisplay::EMmaThread);

    #ifdef RD_JAVA_NGA_ENABLED
    //iWindow->SetRWindowRect( iDirectContainer->MdcContainerWindowRect(),
    //                         MMMADisplay::EMmaThread );
	iWindow->SetRWindowRect( boundrect,
     							MMMADisplay::EMmaThread );
    #endif
    
    SetClippingRegion();
}



TRect& CMMACanvasDisplay::BoundRect()
{
  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect +");
  jmethodID getBoundRectID = iJni->GetMethodID(
	                                         iJavaDisplayClass,
	                                         "getBoundRect",
                                         "()V");

  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect --1");
  // set the value to java,so that we can access those from array
  iJni->CallVoidMethod(iJavaDisplayObject,getBoundRectID);
  jfieldID field = iJni->GetFieldID(iJavaDisplayClass, "displayboundarr", "[I");
  if(field == NULL)
  {
	  // handle error
  }
  /* Read the instance field s */
  jintArray javaboundinfoarr = (jintArray)iJni->GetObjectField(iJavaDisplayObject, field);
  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect --2");
  jint* nativeboundinfoarr = iJni->GetIntArrayElements(javaboundinfoarr, NULL);
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect --3");
    if (!nativeboundinfoarr)
    {    // outputBuffer was already allocated
        iJni->ReleaseIntArrayElements(javaboundinfoarr, nativeboundinfoarr, JNI_ABORT);
        // return invalid rect.
        TRect rect(0,0,0,0);
        return rect;
    }
  LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect --4");
// create TRect
  TInt xcoordinate = nativeboundinfoarr[0];
  TInt ycoordinate = nativeboundinfoarr[1];
  TInt width = nativeboundinfoarr[2];
  TInt height = nativeboundinfoarr[3];
  LOG2(EJavaMMAPI,EInfo,"CMMACanvasDisplay: BoundRect() co-ordinate of topleftcorner is x = %d,y =%d",xcoordinate,ycoordinate);
  LOG2(EJavaMMAPI,EInfo,"CMMACanvasDisplay: BoundRect() size of bound rect is width = %d,height =%d",width,height);
  TPoint topleft(xcoordinate,ycoordinate);
  TSize rectsize(width,height);
  TRect boundRect(topleft,rectsize);
  iJni->ReleaseIntArrayElements(javaboundinfoarr, nativeboundinfoarr, JNI_COMMIT);
   LOG(EJavaMMAPI,EInfo,"MMA::CMMACanvasDisplay::BoundRect -");
  return boundRect;
}

// ask java side peer about the container rect size
// currently assuming the boundrect and containerrect will be same in case of canvas
TRect& CMMACanvasDisplay::ContainerWindowRect()
{

  return BoundRect();

}


void CMMACanvasDisplay::SetDisplayLocationL(const TPoint& aPosition)
{
    // Move iUserRect top left corner to aPosition.
    TSize size(iUserRect.Size());
    iUserRect.iTl = aPosition;
    iUserRect.SetSize(size);

    if (iContainerVisible && !iFullScreen && iWindow)
    {
        iWindow->SetDrawRect(iUserRect);
        SetClippingRegion();
    }
    else
    {
        iResetDrawRect = ETrue;
    }
}

TPoint CMMACanvasDisplay::DisplayLocation()
{
    if (iWindow && iFullScreen)
    {
        return iWindow->DrawRect().iTl;
    }
    else
    {
        return iUserRect.iTl;
    }
}





/*
void CMMACanvasDisplay::MdcContentBoundsChanged(const TRect& aRect)
{
    LOG2(EJavaMMAPI,EInfo,"MID::CMMACanvasDisplay::MdcContentBoundsChanged aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    LOG2(EJavaMMAPI,EInfo,"MID::CMMACanvasDisplay::MdcContentBoundsChanged aRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    if (iWindow)
    {
        // Set new rect to window.
        iWindow->SetWindowRect(aRect, MMMADisplay::EUiThread);

        if (iFullScreen)
        {
            TRect fullRect = ScaleToFullScreen(iCanvas->ContentSize(),
                                               iSourceSize);

            // use SetDrawRectThread because this code is executed
            // in lcdui thread
            iWindow->SetDrawRectThread(fullRect);
        }
        else
        {
            // move to user defined position.
            iWindow->SetPosition(iUserRect.iTl);
        }
    }
    SetClippingRegion();
}
*/

//  END OF FILE
