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
* Description:  Bitmap display that draws to the Java CustomItem's bitmap.
*
*/


//  Include Files
#include <logger.h>
//#include <lcdui.h>

#include "cmmaitemdisplay.h"
#include "cmmabitmapwindow.h"

// CONSTRUCTION
// Static constructor, leaves pointer to cleanup-stack
CMMAItemDisplay* CMMAItemDisplay::NewLC(MMAFunctionServer* aEventSource , jobject aItemDispObj)
{
    CMMAItemDisplay* self = new(ELeave) CMMAItemDisplay();
    CleanupStack::PushL(self);
    self->Construct(aEventSource, NULL, aItemDispObj);
    return self;
}


// Destructor (virtual by CBase)
CMMAItemDisplay::~CMMAItemDisplay()
{
}


CMMAItemDisplay::CMMAItemDisplay(/*MMIDCustomItem* aCustomItem*/)
//: iItem(aCustomItem)
{
    iVisible = ETrue; // Item is visible at startup
}

void CMMAItemDisplay::SizeChangedL(CMMAItemDisplay* /*aDisplay*/,
                                   TInt /*aWidth*/,
                                   TInt /*aHeight*/)
// currently width and height is ignored
// may be used later to layout the image.
{
    /*
    if (aDisplay->iWindow)
    {
        CFbsBitmap* bitmap = aDisplay->iItem->FrameBuffer();
        aDisplay->iWindow->SetDestinationBitmapL(bitmap);
    }
    */
}

void CMMAItemDisplay::SetFullScreenL(TBool aFullScreen)
{
    iFullScreen = aFullScreen;
    if (!iWindow)
    {
        return;
    }
    if (aFullScreen)
    {
        // switch to fullscreen
        iWindow->SetDrawRect(ScaleToFullScreen(
                                 iWindow->WindowSize(), iSourceSize));
    }
    else
    {
        // switch to normal screen
        iWindow->SetDrawRect(iUserRect);
    }
}

void CMMAItemDisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    CMMADisplay::SetWindowL(aWindow);
    if (!iWindow)
    {
        LOG(EJavaMMAPI, EInfo, "CMMAItemDisplay::SetWindowL: NULL window, returning");
        return;
    }
    iSourceSize = iWindow->WindowSize();
    /* CFbsBitmap* bitmap = iItem->FrameBuffer();

     if (bitmap)
     {
         iWindow->SetDestinationBitmapL(bitmap);
     }
    */
}

void CMMAItemDisplay::SetDisplayLocationL(const TPoint& /*aPosition*/)
{
    // In item( USE_GUI_PRIMITIVE ) mode, this call will be ignored.
}


TPoint CMMAItemDisplay::DisplayLocation()
{
    // Java Item's location is always 0, 0
    return TPoint(0, 0);
}




void CMMAItemDisplay:: SourceSizeChanged(TInt aJavaControlWidth, TInt aJavaControlHeight,TInt x, TInt y,TRect aBoundsRect)
{
    JELOG2(EJavaMMAPI);
    LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged(aSourceSize,aJavaControlWidth,aJavaControlHeight) + aJavaControlWidth = %d ,aJavaControlHeight = %d",aJavaControlWidth,aJavaControlHeight);
    iSourceSize = SourceSize();
    //iUserRect = TRect(103,0,aJavaControlWidth,aJavaControlHeight);
    //int x = 103;
    //int y = 0;

    LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged(aSourceSize,aJavaControlWidth,aJavaControlHeight) + sourcesize = %d X %d",iSourceSize.iWidth , iSourceSize.iHeight);
    TSize itemSize(aJavaControlWidth, aJavaControlHeight);

    //iUserRect.iTl.iX = iUserRect.iTl.iX + x;
    //iUserRect.iTl.iY = iUserRect.iTl.iY + y;

    TRect visibleRect(0,0,0,0);
    visibleRect.iBr.iX = visibleRect.iBr.iX + aJavaControlWidth;
    visibleRect.iBr.iY = visibleRect.iBr.iY + aJavaControlHeight;

    // To Do remove adding amrgine once lcdui add them
    /*
    visibleRect.iTl.iX = visibleRect.iTl.iX + 5;
    visibleRect.iTl.iY = visibleRect.iTl.iY + 8;
    visibleRect.iBr.iX = visibleRect.iBr.iX + 5;
    visibleRect.iBr.iY = visibleRect.iBr.iY + 8;
    */

    // Add the form top left co-od to visible rect - to avoid form title

    visibleRect.iTl.iY = visibleRect.iTl.iY + aBoundsRect.iTl.iY;
    visibleRect.iBr.iY = visibleRect.iBr.iY + aBoundsRect.iTl.iY;

    // add the exact location co-od of custom item
    visibleRect.iTl.iX = visibleRect.iTl.iX + x;
    visibleRect.iTl.iY = visibleRect.iTl.iY + y;
    visibleRect.iBr.iX = visibleRect.iBr.iX + x;
    visibleRect.iBr.iY = visibleRect.iBr.iY + y;
    LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - aBoundsRect.iTl %d , %d",aBoundsRect.iTl.iX,aBoundsRect.iTl.iY);
    LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - aBoundsRect.ibr %d , %d",aBoundsRect.iBr.iX,aBoundsRect.iBr.iY);

    TPoint topLeft(0,0);
#ifdef RD_JAVA_NGA_ENABLED
    if (iWindow)
    {
        //iWindow->SetVideoCropRegion( TRect( iUserRect.iTl, itemSize ) );
        iWindow->SetVideoCropRegion(TRect(topLeft , iSourceSize));
    }
#endif



    TBool sourceIsBigger = (iSourceSize.iWidth > itemSize.iWidth ||
                            iSourceSize.iHeight > itemSize.iHeight);
    // size of canvas in java
    /*TSize canvasSize(aJavaControlWidth, aJavaControlHeight);
    iFullScreenSize = canvasSize;
    TBool sourceIsBigger = (iSourceSize.iWidth > iFullScreenSize.iWidth ||
                            iSourceSize.iHeight > iFullScreenSize.iHeight);



    if(itemSize.iWidth == 0 && itemSize.iHeight == 0)
    {
        LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - item size is (0,0)");
        LOG1(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - iUserRect.iTl.iHeight = %d",iUserRect.Size().iHeight);
        iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));
    }
    else
    {
    if (sourceIsBigger)
    {
        // Source is larger than display area.
        // clip the source to fit into item
        LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - source is bigger than display area");
        iWindow->SetDrawRect(ScaleToFullScreen(iFullScreenSize, iSourceSize));
        LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - source is bigger than display area - after SetDrawRect");

    }
    else
    { */
    // source is smaller than display area
    if (iSourceSize.iHeight == itemSize.iHeight && iSourceSize.iWidth == itemSize.iWidth)
    {
        LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - source is smaller than display area");
        LOG1(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - iSourceSize.iWidth = %d",iSourceSize.iWidth);
        LOG1(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - iSourceSize.iHeight = %d",iSourceSize.iHeight);

        //iWindow->SetWindowRect(TRect(TPoint(5,100),TPoint(133,196)),MMMADisplay::EMmaThread);
        iWindow->SetWindowRect(visibleRect,MMMADisplay::EMmaThread);
        iWindow->SetRWindowRect(aBoundsRect,MMMADisplay::EMmaThread);
        //iWindow->SetRWindowRect(TRect(TPoint(0,92),TPoint(360,487)),MMMADisplay::EMmaThread);
        iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));
    }
    else
    {
        if (sourceIsBigger)
        {
            LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - SOURCE IS BIGGER");
            TPoint temp(iUserRect.iTl.iX-x,iUserRect.iTl.iY-y);



            LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - visibleRect.iTl %d , %d",visibleRect.iTl.iX,visibleRect.iTl.iY);
            LOG2(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged - visibleRect.iBr %d , %d",visibleRect.iBr.iX,visibleRect.iBr.iY);
            iWindow->SetWindowRect(visibleRect,MMMADisplay::EMmaThread);
            iWindow->SetRWindowRect(aBoundsRect,MMMADisplay::EMmaThread);
            iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));

            //iWindow->SetWindowRect(TRect(iUserRect.iTl,itemSize),MMMADisplay::EMmaThread);
            //iWindow->SetRWindowRect(TRect(temp,itemSize),MMMADisplay::EMmaThread);

            //iWindow->SetWindowRect(TRect(TPoint(127,100),TPoint(255,96)),MMMADisplay::EMmaThread);
            //iWindow->SetRWindowRect(TRect(TPoint(127,100),TPoint(255,96)),MMMADisplay::EMmaThread);
            //iWindow->SetRWindowRect(TRect(TPoint(0,92),TPoint(360,579)),MMMADisplay::EMmaThread);




            //iWindow->SetDrawRect(TRect(temp, iSourceSize));
            //iWindow->SetDrawRect(TRect(TPoint(0,0), iSourceSize));
        }
        else
        {
        }
    }



    SetClippingRegion();

    /*
    if (iUserRect.IsEmpty())
    {
        // Java side hasn't set size.
        iUserRect = iWindow->DrawRect();

        //if (!sourceIsBigger)
        //{
        // Addjusting rect to top left corner.
        iUserRect = TRect(iUserRect.Size());
        //}
    }*/
    if (iUserRect.IsEmpty())
    {
        // Java side hasn't set size.
        LOG1(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged()iUserRect is set to source size iSourceSize - %d",iSourceSize.iHeight);
        iUserRect.SetSize(iSourceSize);
        LOG1(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged()iUserRect is set to source size iUserRect - %d",iUserRect.Size().iHeight);
    }
    LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::SourceSizeChanged(aJavaControlWidth,aJavaControlHeight)-");
}

void CMMAItemDisplay::StaticSourceSize(CMMAItemDisplay* aDisplay,
                                       TSize* aSize)
{
    LOG(EJavaMMAPI,EInfo,"CMMAItemDisplay::StaticSourceSize +");

    // To DO check if this is correct
    //*aSize = aDisplay->iUserRect.Size();
    *aSize = aDisplay->SourceSize();

}

/*void CMMAItemDisplay::MdcItemContentRectChanged(const TRect& aContentRect,
        const TRect& aScreenRect)
{
    if (iWindow)
    {
        // Change windows rect.
        iWindow->SetWindowRect(aScreenRect, MMMADisplay::EUiThread);
        TRect drawRect = aContentRect;
        drawRect.Move(- aScreenRect.iTl);
        iWindow->SetDrawRectThread(drawRect);
    }
    SetClippingRegion();
}
*/
/*void CMMAItemDisplay::MdcContentBoundsChanged(const TRect& aRect)
{
    // Do nothing in Item display
}
*/
//  END OF FILE
