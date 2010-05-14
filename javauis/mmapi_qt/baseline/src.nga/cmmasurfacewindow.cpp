/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements MMMADisplayWindow functionality
*               in graphics surface based displays for Helix engine.
*
*/

//  Include Files
#include <logger.h>
#include "cmmasurfacewindow.h"
#include "cmmaplayer.h"

// Used for iDisplay member
#include "mmmadisplay.h"

CMMASurfaceWindow* CMMASurfaceWindow::NewL(
    MMAFunctionServer* aEventSource,
    CMMAPlayer* aPlayer)
{
    CMMASurfaceWindow* self =
        new(ELeave) CMMASurfaceWindow(aEventSource,
                                      aPlayer);
    return self;
}

// Destructor (virtual by CBase)
CMMASurfaceWindow::~CMMASurfaceWindow()
{
    // It's not allowed to delete any nonsharable object here.
    // This must be done in Destroy().
    // If the iDisplay is set,
    // instance should be deleted by sending
    // event from UI to be received by MUiEventConsumer.
}

CMMASurfaceWindow::CMMASurfaceWindow(
    MMAFunctionServer* aEventSource,
    CMMAPlayer* aPlayer):
        iEventSource(aEventSource),
        iPlayer(aPlayer),
        iVideoDisplayInitState(EUIResourcesAndSurfaceParametersNotSet)
{
    // Empty rect until video size is known
    iContentRect.SetRect(0, 0, 0, 0);
    iParentRect.SetRect(0, 0, 0, 0);
    iRWindowRect.SetRect(0, 0, 0, 0);
}

void CMMASurfaceWindow::SetDestinationBitmapL(CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

void CMMASurfaceWindow::DrawFrameL(const CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

void CMMASurfaceWindow::SetDrawRectThread( const TRect& aRect )
    {
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDrawRectThread TL %d %d",
    	aRect.iTl.iX, aRect.iTl.iY );
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDrawRectThread BR %d %d",
    	aRect.iBr.iX, aRect.iBr.iY );

    iContentRect = aRect;
    
    TInt error = StaticRedrawVideo(*this);
    if ( KErrNone != error  )
		 {
     ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::SetDrawRectThread, StaticRedrawVideo error = %d", error);
     }
    }

void CMMASurfaceWindow::SetRWindowRect(const TRect& aRect,
                                       MMMADisplay::TThreadType aThreadType)
{
		LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetRWindowRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetRWindowRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iRWindowRect = aRect;

    if (MMMADisplay::EMmaThread == aThreadType)
    {
        if (iDisplay)
        {
				/*		    iDisplay->UIGetCallback( *this,
                                    CMMASurfaceWindow::ESetDrawRect);
        */                            
	     // MMAPI UI 3.x req.
	     iDisplay->GetCallbackInUiThread( (TInt)CMMASurfaceWindow::ESetDrawRect );
       LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetRWindowRect after GetCallbackInUiThread " );

        }
    }
    else if (MMMADisplay::EUiThread == aThreadType)
    {
        /*TInt error = StaticRedrawVideo(*this);
        if (KErrNone != error)
        {
		   LOG1(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetRWindowRect, StaticRedrawVideo error = %d", error);
        }
				*/
		   LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetRWindowRect else GetCallbackInUiThread + " );
		   iDisplay->GetCallbackInUiThread( (TInt)CMMASurfaceWindow::ESetDrawRect );
       LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetRWindowRect else GetCallbackInUiThread -" );
    }
}

void CMMASurfaceWindow::SetDrawRect( const TRect& aRect )
		{
		LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDrawRect TL %d %d",
    	aRect.iTl.iX, aRect.iTl.iY );
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDrawRect BR %d %d",
    	aRect.iBr.iX, aRect.iBr.iY );

		iContentRect = aRect;
		if ( iDisplay )
		  {
		 // iDisplay->UIGetCallback( *this,
		  //                         CMMASurfaceWindow::ESetDrawRect );
		   // MMAPI UI 3.x req.
		   iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ESetDrawRect);
       LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetDrawRect, after GetCallbackInUiThread");
		  }
		}

TInt CMMASurfaceWindow::StaticRedrawVideo(CMMASurfaceWindow& aSurfaceWindow)
{
    TRAPD(error, aSurfaceWindow.RedrawVideoL());
    return error;
}

void CMMASurfaceWindow::RedrawVideoL()
{
    if (!iMediaClientVideoDisplay)
    {
      LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::RedrawVideoL: no MediaClientVideoDisplay set, aborting -" );
        return;
    }

    if (iWindow)
    {
        TRect contentRect;
        if (iVisible)
        {
            contentRect = iContentRect;
            ScaleVideoL(contentRect);
        }
        else
        {
            TRect emptyRect(0,0,0,0);
            contentRect = emptyRect;
        }

        // align parent rect with respect to RWindow
        TRect relativeParentRect;
        relativeParentRect = iParentRect;
        relativeParentRect.Move(-iRWindowRect.iTl);

        // setting video draw rect and adjusting it to window
        TRect drawRect = contentRect;
        drawRect.Move(relativeParentRect.iTl);

        // Area where should be drawn is the intersection of drawRect and relativeParentRect.
        TRect areaRect = relativeParentRect;
        areaRect.Intersection(drawRect);

        iMediaClientVideoDisplay->SetWindowClipRectL(*iWindow,
                areaRect,
                iVideoCropRegion);

        iMediaClientVideoDisplay->SetVideoExtentL(*iWindow,
                areaRect,
                iVideoCropRegion);

			LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::RedrawVideoL: RedrawWindows() areaRect is %d X %d",areaRect.Width(),areaRect.Height() );

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
    }
}

void CMMASurfaceWindow::ScaleVideoL(const TRect& aRect)
{
    // return without scaling incase cropregion dimensions are zero
    if (!(iVideoCropRegion.Width() && iVideoCropRegion.Height()))
    {
        return;
    }

    TReal32 numerator = 1.0f * (aRect.Width() - iVideoCropRegion.Width());
    TReal32 denominator = 1.0f * iVideoCropRegion.Width();
    // new video width percent(relative to the original width) to
    // which video has to be scaled.
    TReal32 scaleWidthPercent = 100.0f + (100.0f * (numerator/denominator));

    numerator = 1.0f * (aRect.Height() - iVideoCropRegion.Height());
    denominator = 1.0f * iVideoCropRegion.Height();
    // new video height percent(relative to the original height) to
    // which video has to be scaled.
    TReal32 scaleHeightPercent = 100.0f + (100.0f * (numerator/denominator));

	LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow:: scaleWidthPercent, scaleHeightPercent %d %d",
	scaleWidthPercent, scaleHeightPercent );

    iMediaClientVideoDisplay->SetScaleFactorL(scaleWidthPercent,
            scaleHeightPercent,
            iVideoCropRegion);
}

const TRect& CMMASurfaceWindow::DrawRect()
{
    return iContentRect;
}

TSize CMMASurfaceWindow::WindowSize()
{
    return iParentRect.Size();
}

void CMMASurfaceWindow::SetPosition(const TPoint& aPosition)
{
    iContentRect = TRect(aPosition, iContentRect.Size());
    TInt error = StaticRedrawVideo(*this);
    if (KErrNone != error)
    {
   ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::SetPosition, StaticRedrawVideo error = %d", error);
    }
}

TBool CMMASurfaceWindow::IsVisible() const
  {
  LOG1( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::IsVisible %d ", iVisible );
  return iVisible;
  }

void CMMASurfaceWindow::SetVisible(TBool aVisible, TBool aUseEventServer)
{
  LOG1( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetVisible aVisible %d", aVisible );
    RPointerArray< CMMAPlayer > players = iEventSource->Players();

    if (players.Find(iPlayer) != KErrNotFound)
    {
    LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetVisible : Player found");
        if (aVisible != iVisible)
        {
    	LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetVisible: Changed visibility" );
            iVisible = aVisible;
            if (iDisplay)
            {
      			 //MMAPI UI 3.x req.
     					iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ESetDrawRect);
     					LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetVisible,GetCallbackInUiThread");
            }
            

											          /*  if (aUseEventServer)
											            {
											                TInt error = StaticRedrawVideo(*this);
											                if (KErrNone != error)
											                {
											         ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::SetVisible, StaticRedrawVideo error = %d", error);
											                }
											            }
											            else // in MMA thread, so switch to UI thread
											            {
											                if (iDisplay)
											                {
													      				//iDisplay->UIGetCallback(*this,
													     				  //                     CMMASurfaceWindow::ESetDrawRect );
													    				  //MMAPI UI 3.x req.
													     					iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ESetDrawRect);
											         					LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::SetVisible,GetCallbackInUiThread");
											                }
											            }
											            */
        }
    }
}

void CMMASurfaceWindow::SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType /*aThreadType*/)
{
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetWindowRect aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetWindowRect aRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iParentRect = aRect;
}

void CMMASurfaceWindow::SetVideoCropRegion(const TRect& aRect)
{
    // video size
    iVideoCropRegion = aRect;
}

const TRect& CMMASurfaceWindow::WindowRect()
{
    return iParentRect;
}

void CMMASurfaceWindow::ContainerDestroyed()
{
    // We are in UI thread context now.
    CleanVideoDisplay();
}

void CMMASurfaceWindow::SetDisplay(MMMADisplay *aDisplay)
{
    LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDisplay +" );

    if (iDisplay != aDisplay)
    {
        if (iDisplay)
        {
            // Clear the resources created within the old Display
         	 iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ECleanVideoDisplay );
        }

        // Set the new Display
        iDisplay = aDisplay;

        /*if ( iDisplay )
            {
            // Get a DSA resources for the new Display
            //iDisplay->UIGetDSAResources( *this, MMMADisplay::EMmaThread );
            CMMACanvasDisplay* display =  static_cast< CMMACanvasDisplay* >( iDisplay );

            display->GetWindowResources( this, MMMADisplay::EMmaThread );
            }

            */
        }

    LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::SetDisplay -" );
    }

void CMMASurfaceWindow::ContainerSet()
    {
    	LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ContainerSet" );
    // We are in UI thread now

    // Container was probably not set when
    // iDisplay was set,
    // we can now try get the DSA stuff again
    if (iDisplay)
    {
        // Get a DSA stuff for the new Display
//        iDisplay->UIGetDSAResources( *this, MMMADisplay::EUiThread );
        }
    }

void CMMASurfaceWindow::Destroy()
    {
    LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::Destroy" );
    // Delete itself
    delete this;
    }

void CMMASurfaceWindow::ProcureWindowResourcesFromQWidget(RWsSession * aWs,
    CWsScreenDevice* aScreenDevice,
    RWindowBase* aWindow)
	{
		iWs = aWs;
    iScreenDevice = aScreenDevice;
    iWindow = aWindow;
		LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget" );
		switch ( iVideoDisplayInitState )
		  {
		  	LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -7" );
			case EUIResourcesAndSurfaceParametersNotSet:
			     {
			     LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -8" );
			     iVideoDisplayInitState =
			     EUIResourcesSetAndSurfaceParametersNotSet;
			     }
			     break;
			case ESurfaceParametersSetAndUIResourcesNotSet:
			     {
			     	LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -9" );
			     iVideoDisplayInitState =
			     EUIResourcesAndSurfaceParametersSet;
			     }
			     break;
			 // can not occur
			case EUIResourcesSetAndSurfaceParametersNotSet:
		  case EUIResourcesAndSurfaceParametersSet:
			default:
           {LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -10" );
      		 __ASSERT_DEBUG( EFalse, User::Invariant() );
           }
			     break;
		  }

		if( iVideoDisplayInitState == EUIResourcesAndSurfaceParametersSet )
			{
				LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -11" );
			TRAPD(error, InitVideoDisplayL());
			LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::ProcureWindowResourcesFromQWidget  -12" );
			if ( KErrNone != error )
				{
				ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::MdcDSAResourcesCallback, error = %d", error);
				}
			}

	}
/*
void CMMASurfaceWindow::MdcDSAResourcesCallback(
    RWsSession &aWs,
    CWsScreenDevice &aScreenDevice,
    RWindowBase &aWindow )
    {
    LOG(EJavaMMAPI,EInfo, "MID::CMMASurfaceWindow::MdcDSAResourcesCallback" );

    // We are in UI thread context now.
    iWs = &aWs;
    iScreenDevice = &aScreenDevice;
    iWindow = &aWindow;

		switch ( iVideoDisplayInitState )
		  {
			case EUIResourcesAndSurfaceParametersNotSet:
			     {
			     iVideoDisplayInitState =
			     EUIResourcesSetAndSurfaceParametersNotSet;
			     }
			     break;
			case ESurfaceParametersSetAndUIResourcesNotSet:
			     {
			     iVideoDisplayInitState =
			     EUIResourcesAndSurfaceParametersSet;
			     }
			     break;
			 // can not occur
			case EUIResourcesSetAndSurfaceParametersNotSet:
		  case EUIResourcesAndSurfaceParametersSet:
			default:
           {
      		 __ASSERT_DEBUG( EFalse, User::Invariant() );
           }
			     break;
		  }

		if( iVideoDisplayInitState == EUIResourcesAndSurfaceParametersSet )
			{
			TRAPD(error, InitVideoDisplayL());
			if ( KErrNone != error )
				{
				LOG1(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::MdcDSAResourcesCallback, error = %d", error);
				}
			}
    }
*/
void CMMASurfaceWindow::UICallback( TInt aCallbackId )
    {
    // We are in UI thread context now.
    LOG1( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::MdcUICallback CallbackId = %d", aCallbackId );

    TInt error = KErrNone;
    switch (aCallbackId)
    {
    case ERemoveSurface:
    {
        DoRemoveSurface();
    }
    break;
    case ESetDrawRect:
    {
        error = StaticRedrawVideo(*this);
           ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::MdcUICallback,StaticRedrawVideo error = %d", error);
    }
    break;
    case EInitVideoDisplay:
    {
        TRAP(error, InitVideoDisplayL());
			     ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::MdcUICallback,InitVideoDisplayL error = %d", error);
    }
    break;
    case ESetChangedSurfaceParameters:
    {
        DoSetChangedSurfaceParameters();
    }
    break;
    case EResetSurfaceParameters:
    {
        DoResetSurfaceParameters();
    }
    break;
    case ECleanVideoDisplay:
    {
        CleanVideoDisplay();
    }
    break;
    case EDestroyWindow:
    {
        Destroy();
    }
    break;
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    break;
    }
}

void CMMASurfaceWindow::SetSurfaceParameters(const TSurfaceId& aSurfaceId,
        const TRect& aCropRect,
        const TVideoAspectRatio& aPixelAspectRatio)
{
    iSurfaceId = aSurfaceId;
    iCropRect = aCropRect;
    iPixelAspectRatio = aPixelAspectRatio;

    switch ( iVideoDisplayInitState )
		  {
			case EUIResourcesAndSurfaceParametersNotSet:
			     {
			     iVideoDisplayInitState =
			     ESurfaceParametersSetAndUIResourcesNotSet;
			     }
			     break;
			case EUIResourcesSetAndSurfaceParametersNotSet:
			     {
			     iVideoDisplayInitState =
			     EUIResourcesAndSurfaceParametersSet;
			     }
			     break;
			  // control reaches below two switch cases when
			  // playback is looped using setLoopCount() in java.
		  case ESurfaceParametersSetAndUIResourcesNotSet:
		  	   {
			     }
			     break;
			  // update surface parameters and return with out calling
			  // InitVideoDisplayL again.
			case EUIResourcesAndSurfaceParametersSet:
			     {
			    // iDisplay->UIGetCallback( *this, CMMASurfaceWindow::EResetSurfaceParameters );
			    //MMAPI UI 3.x req.
			    LOG(EJavaMMAPI,EInfo,"CMMASurfaceWindow::SetSurfaceParameters : switch case EUIResourcesAndSurfaceParametersSet +");
			    iDisplay->GetCallbackInUiThread( (TInt)CMMASurfaceWindow::EResetSurfaceParameters );
			    LOG(EJavaMMAPI,EInfo,"CMMASurfaceWindow::SetSurfaceParameters : switch case EUIResourcesAndSurfaceParametersSet -");
			     return;
			     }
			     // break; not reachable
			default: // can not occur
           {
      		 __ASSERT_DEBUG( EFalse, User::Invariant() );
           }
			     break;
		  }

    if ( iVideoDisplayInitState == EUIResourcesAndSurfaceParametersSet )
    	{
    	//iDisplay->UIGetCallback( *this, CMMASurfaceWindow::EInitVideoDisplay );
    	//MMAPI UI 3.x req.
    	iDisplay->GetCallbackInUiThread( (TInt)CMMASurfaceWindow::EInitVideoDisplay );
			LOG(EJavaMMAPI,EInfo,"CMMASurfaceWindow::SetSurfaceParameters,EUIResourcesAndSurfaceParametersSet");
    	}
		}

void CMMASurfaceWindow::SetChangedSurfaceParameters(const TSurfaceId& aSurfaceId,
        const TRect& aCropRect,
        const TVideoAspectRatio& aPixelAspectRatio)
{
    iSurfaceId = aSurfaceId;
    iCropRect = aCropRect;
    iPixelAspectRatio = aPixelAspectRatio;

    if (iDisplay)
    {
        //iDisplay->UIGetCallback(*this, CMMASurfaceWindow::ESetChangedSurfaceParameters);
    	LOG(EJavaMMAPI,EInfo,"CMMASurfaceWindow::SetChangedSurfaceParameters + ");
    	iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ESetChangedSurfaceParameters);
    	LOG(EJavaMMAPI,EInfo,"CMMASurfaceWindow::SetChangedSurfaceParameters - ");

	}
}

void CMMASurfaceWindow::RemoveSurface()
{
    if (iDisplay)
    {
        iDisplay->GetCallbackInUiThread((TInt)CMMASurfaceWindow::ERemoveSurface);                         	
    }
}

void CMMASurfaceWindow::DoRemoveSurface()
{
    if (iMediaClientVideoDisplay)
    {
        iMediaClientVideoDisplay->RemoveSurface();
        LOG(EJavaMMAPI,EInfo,"MID::CMMASurfaceWindow::DoRemoveSurface, Surface Removed");

    }
}

void CMMASurfaceWindow::DoResetSurfaceParameters()
{
    __ASSERT_DEBUG((iMediaClientVideoDisplay != NULL), User::Invariant());

    if (iMediaClientVideoDisplay)
    {
    	iMediaClientVideoDisplay->RemoveSurface();
        TInt error = iMediaClientVideoDisplay->SurfaceCreated(iSurfaceId,
                     iCropRect,
                     iPixelAspectRatio,
                     iVideoCropRegion);

      ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::DoResetSurfaceParameters,SurfaceCreated error = %d", error);

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
    }
}

void CMMASurfaceWindow::DoSetChangedSurfaceParameters()
{
    if (iMediaClientVideoDisplay)
    {
        TInt error = iMediaClientVideoDisplay->SurfaceParametersChanged(iSurfaceId,
                     iCropRect,
                     iPixelAspectRatio);

			ELOG1( EJavaMMAPI, "MID::CMMASurfaceWindow::DoSetChangedSurfaceParameters,SurfaceParametersChanged, error = %d", error);

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
    }
}

void CMMASurfaceWindow::InitVideoDisplayL()
{
    if (iVideoDisplayInitState != EUIResourcesAndSurfaceParametersSet)
    {
        User::Leave(KErrNotReady);
    }

    // check that this is the first time we are creating instance.
    if (iMediaClientVideoDisplay)
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }

    iMediaClientVideoDisplay =
        CMediaClientVideoDisplay::NewL(iScreenDevice->GetScreenNumber(),
                                       iSurfaceId,
                                       iCropRect,
                                       iPixelAspectRatio);

    // video is not scaled untill user requests explicitly
    // so retain same video width & height.
    TReal32 scaleWidthPercent = 100.0f;
    TReal32 scaleHeightPercent = 100.0f;

    // video rotation feature not supported in MMAPI
    TVideoRotation videoRotation(EVideoRotationNone);
    // no automatic scaling, can be controlled only via VideoControl
    TAutoScaleType autoScaleType = EAutoScaleNone;

    // always align video to the top left corner of the display area
    TInt horizontalPosition(EHorizontalAlignLeft);
    TInt verticalPosition(EVerticalAlignTop);

    TInt error = iMediaClientVideoDisplay->SurfaceCreated(iSurfaceId,
                 iCropRect,
                 iPixelAspectRatio,
                 iVideoCropRegion);

		ELOG1( EJavaMMAPI,  "MID::CMMASurfaceWindow::InitVideoDisplayL error = %d", error );
    User::LeaveIfError(error);

    iMediaClientVideoDisplay->AddDisplayWindowL(iWindow,
            iContentRect,
            iVideoCropRegion,
            iContentRect,
            scaleWidthPercent,
            scaleHeightPercent,
            videoRotation,
            autoScaleType,
            horizontalPosition,
            verticalPosition,
            (RWindow*)iWindow);

    RedrawVideoL();
}

void CMMASurfaceWindow::CleanVideoDisplay()
{
		LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::CleanVideoDisplay +" );

    SetVisible(EFalse, ETrue);
    if (iMediaClientVideoDisplay)
    {
        if (iWindow)
        {
            iMediaClientVideoDisplay->RemoveDisplayWindow(*iWindow);
        }
        iMediaClientVideoDisplay->RemoveSurface();
        delete iMediaClientVideoDisplay;

        iMediaClientVideoDisplay = NULL;
        iWindow = NULL;
        iScreenDevice = NULL;
        iWs = NULL;
    }

    LOG( EJavaMMAPI, EInfo,  "MID::CMMASurfaceWindow::CleanVideoDisplay -" );
}


CMMAPlayer* CMMASurfaceWindow::UiPlayer()
	{
		  return iPlayer;
	}
//  END OF FILE
