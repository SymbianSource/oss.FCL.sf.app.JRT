/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// API used for getting the layout for the network indicator bitmap.
#include <AknUtils.h>

// For layout data for network indicator using
// AknLayoutScalable_Avkon::signal_pane_g2_cp().
#include <aknlayoutscalable_avkon.cdl.h>

// Using Layout_Meta_Data::IsLandscapeOrientation().
#include <layoutmetadata.cdl.h>

// For CSD network indicator.
#include <callstatus.mbg>

// For GPRS network indicator.
#include <avkon.mbg>

// Using constants for network indicator.
#include <nifvar.h>

// Using debug macros.
#include <j2me/jdebug.h>

#include <data_caging_path_literals.hrh>
// Using S60 common utils
#include <s60commonutils.h>

// Using CMIDRemConObserver for media keys listener.
#include "CMIDRemConObserver.h"

// Using CMIDCallIndicator class as member iCallIndicator.
#include "CMIDCallIndicator.h"

#include "CMIDNetworkIndicator.h"
#include "CMIDCanvas.h"


// Call status bitmap file.
_LIT(KCallStatusBitmapFileName, "callstatus.mbm");


// Alpha value for icon.
const TInt KAlphaValue = 65;

const TInt  KJVMProcessUid = 0x102033E6;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::NewL
// Two-phased constructor. Use this method to create a CMIDCanvas instance.
// ---------------------------------------------------------------------------
//
CMIDNetworkIndicator* CMIDNetworkIndicator::NewL(CMIDCanvas* aCanvas)
{
    CMIDNetworkIndicator* self = new(ELeave) CMIDNetworkIndicator;
    CleanupStack::PushL(self);
    self->ConstructL(aCanvas);
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::~CMIDNetworkIndicator
// Destructor.
// ---------------------------------------------------------------------------
//
CMIDNetworkIndicator::~CMIDNetworkIndicator()
{
    CleanNetworkResource();
    CleanTempNetworkResource();
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::GetLocation
// Get the location of the network indicator.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::GetLocation(TPoint& aPosition, TSize& aSize)
{
    aPosition = iNetworkIndicatorPosition;
    aSize = iNetworkIndicatorSize;
}


// ---------------------------------------------------------------------------
// From class MConnectionMonitorObserver.
// CMIDNetworkIndicator::EventL
// Called by the Connection Monitor.
//
// Nothing can leave.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::EventL(const CConnMonEventBase& aConnMonEvent)
{
    DEBUG_INT(">>> CMIDNetworkIndicator - EventL( %d )",
              aConnMonEvent.EventType());

    switch (aConnMonEvent.EventType())
    {
    case EConnMonCreateConnection:
    case EConnMonDeleteConnection:
    case EConnMonBearerInfoChange:
    case EConnMonConnectionStatusChange:
    {
        iNetworkActivity = EFalse;
        ConnectionStatus(iNetworkActivity);
        Repaint();
        break;
    }

    case EConnMonConnectionActivityChange:
    {
        // Refresh the network connection bitmap if connection
        // activity is true.
        CConnMonConnectionActivityChange*
        eventConnectionActivityChange;

        eventConnectionActivityChange =
            (CConnMonConnectionActivityChange*) &aConnMonEvent;

        iNetworkActivity =
            eventConnectionActivityChange->ConnectionActivity();

        ConnectionStatus(iNetworkActivity);
        Repaint(ETrue);
        break;
    }
    default:
    {
        DEBUG("<<< CMIDNetworkIndicator - EventL");
        return;
    }
    }
    DEBUG("<<< CMIDNetworkIndicator - EventL");

}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::CMIDNetworkIndicator
// Desctructor.
// ---------------------------------------------------------------------------
//
CMIDNetworkIndicator::CMIDNetworkIndicator()
{
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::ConstructL(CMIDCanvas* aCanvas)
{
    iCanvas = aCanvas;

    // get the layout data
    GetLayoutData();

    iConnectionMonitor.ConnectL();
    iConnectionMonitor.NotifyEventL(*this);

    // check connection status and create graphics resources
    InitNetworkResource();
    iNetworkActivity = EFalse;
    iDrawIndicator = ETrue;
    iCallIndicator = NULL;
    ConnectionStatus(iNetworkActivity);
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::InitNetworkResource
// Initialize network resource.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::InitNetworkResource()
{
    iNetworkBitmap = NULL;
    iNetworkMaskBitmap = NULL;
    iTempNetworkBitmap = NULL;
    iTempNetworkMaskBitmap = NULL;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::ConnectionStatus
// Main method to get the status of the network.
// Check how many connections are open and set the bearer type and state of
// the connection.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::ConnectionStatus(const TBool aConActivity)
{
    TRequestStatus requestStatus;
    TUint subConnCount;
    TUint connection_ID = KErrNone; // connection ID is set to zero
    iConnectionBearer = EBearerInfoUnknown;
    TBool conectionFound = EFalse;
    DEBUG_INT(">>> CMIDNetworkIndicator - ConnectionStatus( %d )",
              aConActivity);
    iConnectionMonitor.GetConnectionCount(iConnectionCount, requestStatus);
    User::WaitForRequest(requestStatus);

    // clean the bitmap only if call is not ongoing, there
    // is active call icon stored in the temp bitmap while calling
    if (iCallIndicator != NULL && !iCallIndicator->GetCallActivity())
    {
        CleanNetworkResource();
    }

    for (TUint i = 1; i <= iConnectionCount; i++)
    {
        // Index i must be between 1 and the connection count returned by
        // GetConnectionCount().
        TInt ret = iConnectionMonitor.GetConnectionInfo(i,
                   connection_ID,
                   subConnCount);

        if (ret == KErrNotFound)   // Connection has suddenly died.
        {
            continue;
        }

        iConnectionMonitor.GetIntAttribute(connection_ID,
                                           0, // sub Connection ID
                                           KBearerInfo,
                                           iConnectionBearer,
                                           requestStatus);

        User::WaitForRequest(requestStatus);

        iConnectionMonitor.GetIntAttribute(connection_ID,
                                           0, // sub Connection ID
                                           KConnectionStatus,
                                           iConnectionStatus,
                                           requestStatus);
        User::WaitForRequest(requestStatus);

        // Set events ON = 1 for BearerInfo.
        iConnectionMonitor.SetUintAttribute(connection_ID,
                                            0, // sub Connection ID
                                            KBearerGroupThreshold,
                                            1);

        iConnectionMonitor.SetUintAttribute(connection_ID,
                                            0, // sub Connection ID
                                            KActivityTimeThreshold,
                                            5);  // 5 second is minimum value


        TConnMonClientEnumBuf clientInfoBuf;
        RProcess process;
        RThread thread;
        // Check if connection is for MIDlet
        iConnectionMonitor.GetPckgAttribute(connection_ID,
                                            0, // sub Connection ID
                                            KClientInfo,
                                            clientInfoBuf,
                                            requestStatus);

        User::WaitForRequest(requestStatus);
        TUint count(clientInfoBuf().iCount);
        DEBUG_INT("CMIDNetworkIndicator::ConnectionStatus  Client count UIDs %D", count);
        for (TUint j = 0 ; j<count ; j++)
        {
            TUid uid(clientInfoBuf().iUid[j]);
            DEBUG_INT("CMIDNetworkIndicator::ConnectionStatus  Client UID:  %D", uid.iUid);
            if (uid.iUid == KJVMProcessUid)
            {
                conectionFound = ETrue;
            }
        }


        if (conectionFound)
        {
            DEBUG("CMIDNetworkIndicator::ConnectionStatus  Active connection of current MIDlet found");
            // Check for valid bearer type and connection activity.
            if (IsGprsOrCsdConnection(iConnectionBearer) && aConActivity)
            {

                // We have a valid GPRS or CSD connection and draw the indicator.
                // We are in the non leaving function so we cannot leave here.
                DEBUG("CMIDNetworkIndicator::ConnectionStatus - \
calling CreateNetworkResourceL");

                // Create the network resource.
                TRAPD(ignoreError,
                      CreateNetworkResourceL(iConnectionBearer,
                                             iConnectionStatus));
                if (ignoreError != KErrNone)
                {
                    DEBUG_INT("CMIDNetworkIndicator::ConnectionStatus - \
Exception from CreateNetworkResourceL. Error = %d", ignoreError);
                }
            }
        }

    } // end of for loop

}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::IsGprsOrCsdConnection
// Check the connection type. If there is valid CSD/GPRS/CDMA connection
// return true orwise return false.
// ---------------------------------------------------------------------------
//
TBool CMIDNetworkIndicator::IsGprsOrCsdConnection(TInt  aNetworkBearer)
{
    switch (aNetworkBearer)
    {
    case EBearerInfoCSD:
    case EBearerInfoHSCSD:
    case EBearerInfoWCDMA:
    case EBearerInfoCDMA2000:
    case EBearerInfoGPRS:
    case EBearerInfoEdgeGPRS:
    case EBearerInfoWcdmaCSD:
    case EBearerInfoHSDPA:
    case EBearerInfoHSUPA:
    case EBearerInfoHSxPA:
    {
        // There is valid GPRS/CSD/CDMA connection open.
        return ETrue;
    }
    default:
    {
        return EFalse;
    }
    } // end of switch
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::IsNetworkActive
// Returns the activity of the network.
// ---------------------------------------------------------------------------
//
TBool CMIDNetworkIndicator::IsNetworkActive()
{
    return iNetworkActivity;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::Close
// Close the connection.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::Close()
{
    iConnectionBearer = EBearerUnknown;
    CleanNetworkResource();
    CleanTempNetworkResource();
    iConnectionMonitor.Close();
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::DrawNetworkIndicator
// Draw network indicator.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::DrawNetworkIndicator(
    CFbsBitGc& aGc,
    const TRect& aRect,
    const TRect& aViewRect)
{
    if (iNetworkBitmap && iDrawIndicator)
    {
        // Network indicator exists.
        TRect networkIndicatorSizeRect =
            TRect(TPoint(0, 0), iNetworkIndicatorSize);

        TRect rect = networkIndicatorSizeRect;
        TRect rectIntersect;

        rect.Move(iNetworkIndicatorPosition);

        if (iCanvas->IsScalingOn())
        {
            // Graphics scaling is on.
            aGc.SetClippingRect(rect);
        }

        rectIntersect = rect;

        // Move to correct position for test aRect.Intersects.
        rectIntersect.Move(TPoint(aViewRect.iTl.iX,aViewRect.iTl.iY));

        if (aRect.Intersects(rectIntersect))
        {
#ifdef RD_JAVA_NGA_ENABLED
            if (iCanvas->IsEglAvailable())
            {
                aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
            }
#endif
            // Drawable area intersects with network indicator rect.
            if (iNetworkMaskBitmap)
            {
                aGc.SetPenColor(TRgb(0xffffff, KAlphaValue));
                aGc.SetBrushColor(TRgb(0xffffff, KAlphaValue));
                aGc.SetBrushStyle(CWindowGc::ESolidBrush);
                aGc.DrawRect(rectIntersect);

                aGc.BitBltMasked(iNetworkIndicatorPosition,
                                 iNetworkBitmap,
                                 networkIndicatorSizeRect,
                                 iNetworkMaskBitmap,
                                 EFalse);
            }
            else
            {
                aGc.BitBlt(iNetworkIndicatorPosition,
                           iNetworkBitmap,
                           TRect(TPoint(0, 0),
                                 iNetworkIndicatorSize));
            }
#ifdef RD_JAVA_NGA_ENABLED
            if (iCanvas->IsEglAvailable())
            {
                iCanvas->UpdateRect(rectIntersect);
            }
#endif
        }
    }
    iDrawIndicator = EFalse;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::Repaint
// Repaint the network indicator.
// This is called after there was a change in the network connection
// status in order to update the network indicator bitmap.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::Repaint(TBool aRedraw)
{

    // Refresh the connection status.
    // It also cleans the network indicator bitmap and creates a new one
    // according to the connection status.
    // It does not create any indicator bitmap if no connection is active.

    DEBUG("CMIDNetworkIndicator::Repaint()");
    if (iCanvas->IsVisible())
    {
        // If visible, update the canvas with the new NW indicator if any.

        if (iNetworkBitmap && !aRedraw)
        {
            // Draw the new NW indicator on top of the old framebuffer.
            // Assumption: the new NW icon has the same size as the previous
            // one so it will be drawn exactly on top of the previous one.
            iCanvas->DrawDeferred();
        }
        else
        {
            // No network indicator shall be displayed or it needs to be
            // redrawn. The framebuffer contains the previous NW indicator
            // bitmap and therefore the canvas needs to be repainted by java
            // Canvas.paint().

            // Calculate the network indicator area which needs to be
            // repainted by java.
            TInt x(iNetworkIndicatorPosition.iX);
            TInt y(iNetworkIndicatorPosition.iY);
            TInt w(iNetworkIndicatorSize.iWidth);
            TInt h(iNetworkIndicatorSize.iHeight);

            // Create the 'EPaint' event parameters as expected in Java.
            TInt pos((x << 16) | y);
            TInt size((w << 16) | h);

            // Post event to Java Canvas in order to invoke repaint.
            iCanvas->PostEvent(EPaint, pos, size);
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::CreateNetworkResourceL
// Create the network resource for different bearer.
// Later during drawing we will use these bitmaps.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::CreateNetworkResourceL(
    TInt aBearer,
    TInt aStatus)
{
    CleanTempNetworkResource();
    TFileName avkonbmpFilename = AknIconUtils::AvkonIconFileName();

    DEBUG_INT2(">>> CreateNetworkResourceL(%d, %d)", aBearer, aStatus);

    switch (aBearer)
    {
    case EBearerInfoCSD:
    case EBearerInfoHSCSD:
    case EBearerInfoWcdmaCSD:
    {
        // EMbmCallstatusQgn_indi_call_active.
        DEBUG("CreateNetworkResourceL() - \
EMbmCallstatusQgn_indi_call_active");

        avkonbmpFilename.SetLength(0);
        avkonbmpFilename.Append(KDC_APP_BITMAP_DIR);
        avkonbmpFilename.Append(KCallStatusBitmapFileName);
        avkonbmpFilename = java::util::S60CommonUtils::VerifiedFileNameL(avkonbmpFilename);

        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnIndiCallActive,
            iTempNetworkBitmap,
            iTempNetworkMaskBitmap,
            avkonbmpFilename,
            EMbmCallstatusQgn_indi_call_active,
            EMbmCallstatusQgn_indi_call_active_mask);
        break;
    }
    case EBearerInfoWCDMA:
    case EBearerInfoCDMA2000:
    {
        // iConnectionCount will be valid here, COZ we first get the
        // connection count. Then check the bearer type.
        if (iConnectionCount < 2)
        {
            switch (aStatus)
            {
            case KDataTransferTemporarilyBlocked:
                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalWcdmaContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_wcdma_suspended,
                    EMbmAvkonQgn_indi_signal_wcdma_suspended_mask
                );
                break;

            default:
                DEBUG_INT("CreateNetworkResourceL() - \
unhandled CDMA status: %d", aStatus);

            case KStartingConnection:
            case KLinkLayerOpen:
                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalWcdmaContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_wcdma_context,
                    EMbmAvkonQgn_indi_signal_wcdma_context_mask);
            }
        }
        else
        {
            DEBUG("CreateNetworkResourceL() - multipdp");

            AknsUtils::CreateIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiSignalWcdmaMultipdp,
                iTempNetworkBitmap,
                iTempNetworkMaskBitmap,
                avkonbmpFilename,
                EMbmAvkonQgn_indi_signal_wcdma_multipdp,
                EMbmAvkonQgn_indi_signal_wcdma_multipdp_mask);
        }
        break;
    }
    case EBearerInfoGPRS:
    {
        // iConnectionCount will be valid here, COZ we first get the
        // connection count. Then check the bearer type.
        if (iConnectionCount < 2)
        {
            switch (aStatus)
            {
            case KDataTransferTemporarilyBlocked:
            {
                DEBUG("<<< CreateNetworkResourceL() - \
suspended");

                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalPdContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_pd_suspended,
                    EMbmAvkonQgn_indi_signal_pd_suspended_mask
                );
                break;
            }
            default:
                DEBUG_INT("CreateNetworkResourceL() - \
unhandled GPRS status: %d", aStatus);

            case KStartingConnection:
            case KLinkLayerOpen:
                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalPdContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_pd_context,
                    EMbmAvkonQgn_indi_signal_pd_context_mask
                );
                break;
            }
        }
        else
        {
            DEBUG("CreateNetworkResourceL() - multipdp");

            AknsUtils::CreateIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiSignalPdMultipdp,
                iTempNetworkBitmap,
                iTempNetworkMaskBitmap,
                avkonbmpFilename,
                EMbmAvkonQgn_indi_signal_pd_multipdp,
                EMbmAvkonQgn_indi_signal_pd_multipdp_mask);
        }
        break;
    }
    case EBearerInfoEdgeGPRS:
    {
        // iConnectionCount will be valid here, COZ we first get the
        // connection count. Then check the bearer type.
        if (iConnectionCount < 2)
        {
            switch (aStatus)
            {
            case KDataTransferTemporarilyBlocked:
                DEBUG("<<< CreateNetworkResourceL() - \
suspended");

                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalEgprsContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_egprs_suspended,
                    EMbmAvkonQgn_indi_signal_egprs_suspended_mask
                );
                break;

            default:
                DEBUG_INT("CreateNetworkResourceL() - \
unhandled EdgeGPRS status: %d", aStatus);

            case KStartingConnection:
            case KLinkLayerOpen:
                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalEgprsContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_egprs_context,
                    EMbmAvkonQgn_indi_signal_egprs_context_mask);
                break;
            }
        }
        else
        {
            DEBUG("CreateNetworkResourceL() - multipdp");

            AknsUtils::CreateIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiSignalEgprsMultipdp,
                iTempNetworkBitmap,
                iTempNetworkMaskBitmap,
                avkonbmpFilename,
                EMbmAvkonQgn_indi_signal_egprs_multipdp,
                EMbmAvkonQgn_indi_signal_egprs_multipdp_mask);
        }
        break;
    }
    case EBearerInfoHSDPA:
    case EBearerInfoHSUPA:
    case EBearerInfoHSxPA:
    {
        // iConnectionCount will be valid here, COZ we first get the
        // connection count. Then check the bearer type.
        if (iConnectionCount < 2)
        {
            switch (aStatus)
            {
            case KDataTransferTemporarilyBlocked:
                DEBUG("<<< CreateNetworkResourceL() - \
suspended");

                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalHsdpaContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_hsdpa_suspended,
                    EMbmAvkonQgn_indi_signal_hsdpa_suspended_mask
                );
                break;

            default:
                DEBUG_INT("CreateNetworkResourceL() - \
unhandled HSxPA status: %d", aStatus);

            case KStartingConnection:
            case KLinkLayerOpen:
                AknsUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    KAknsIIDQgnIndiSignalHsdpaContext,
                    iTempNetworkBitmap,
                    iTempNetworkMaskBitmap,
                    avkonbmpFilename,
                    EMbmAvkonQgn_indi_signal_hsdpa_context,
                    EMbmAvkonQgn_indi_signal_hsdpa_context_mask
                );
                break;
            }
        }
        else
        {
            DEBUG("CreateNetworkResourceL() - multipdp");

            AknsUtils::CreateIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnIndiSignalHsdpaMultipdp,
                iTempNetworkBitmap,
                iTempNetworkMaskBitmap,
                avkonbmpFilename,
                EMbmAvkonQgn_indi_signal_hsdpa_multipdp,
                EMbmAvkonQgn_indi_signal_hsdpa_multipdp_mask);
        }
        break;
    }

    default:
        DEBUG("<<< CreateNetworkResourceL()");
        return;
    }// end of switch case

    // When call is ongoing, the created icon is not used but stored in
    // iTempNetworkBitmap and used after call ends.
    if (iCallIndicator != NULL && !iCallIndicator->GetCallActivity())
    {
        CleanNetworkResource();
        CopyBitmaps();

        // Resize the NW indicator bitmap according to the layout data.
        if (iNetworkBitmap)
        {
            CMIDNetworkIndicator::ResizeBitmap();
        }
    }

    DEBUG("<<< CreateNetworkResourceL()");
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::CleanNetworkResource
// Clean network resource.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::CleanNetworkResource()
{
    if (iNetworkBitmap != NULL)
    {
        delete iNetworkBitmap;
        iNetworkBitmap = NULL;
    }
    if (iNetworkMaskBitmap != NULL)
    {
        delete iNetworkMaskBitmap;
        iNetworkMaskBitmap = NULL;
    }
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::CleanTempNetworkResource
// Clean temporary network resource.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::CleanTempNetworkResource()
{
    if (iTempNetworkBitmap != NULL)
    {
        delete iTempNetworkBitmap;
        iTempNetworkBitmap = NULL;
    }
    if (iTempNetworkMaskBitmap != NULL)
    {
        delete iTempNetworkMaskBitmap;
        iTempNetworkMaskBitmap = NULL;
    }
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::CopyBitmaps
// Copy temp bitmaps to the bitmaps.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::CopyBitmaps()
{
    iNetworkBitmap = iTempNetworkBitmap;
    iNetworkMaskBitmap = iTempNetworkMaskBitmap;
    iTempNetworkBitmap = NULL;
    iTempNetworkMaskBitmap = NULL;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::ResizeBitmap
// Resize the network indicator bitmap according to the layout data.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::ResizeBitmap()
{
    ASSERT(iNetworkBitmap);

    // Setting size for network indicator bitmap
    AknIconUtils::SetSize(iNetworkBitmap, iNetworkIndicatorSize);

    if (iNetworkMaskBitmap)
    {
        // Setting size for network indicator mask
        AknIconUtils::SetSize(iNetworkMaskBitmap, iNetworkIndicatorSize);
    }
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::GetLayoutData
// Get the layout for the network indicator bitmap.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::GetLayoutData()
{
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screen);

    TAknLayoutRect canvasRect;
    canvasRect.LayoutRect(
        screen,
        AknLayoutScalable_Avkon::midp_canvas_pane().LayoutLine());

    TAknLayoutRect networkIndicatorRect;

    networkIndicatorRect.LayoutRect(
        canvasRect.Rect(),
        AknLayoutScalable_Avkon::signal_pane_g2_cp().LayoutLine());

    // Network indicator size rect is set.
    iNetworkIndicatorSize = networkIndicatorRect.Rect().Size();

    // Network indicator position is set.
    iNetworkIndicatorPosition = networkIndicatorRect.Rect().iTl;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::GetDrawIndicator
// Get member iDrawIndicator value.
// ---------------------------------------------------------------------------
//
TBool CMIDNetworkIndicator::GetDrawIndicator()
{
    return iDrawIndicator;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::SetDrawIndicator
// Set member iDrawIndicator value.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::SetDrawIndicator(TBool aDrawIndicator)
{
    iDrawIndicator = aDrawIndicator;
}


// ---------------------------------------------------------------------------
// CMIDNetworkIndicator::SetCallIndicator
// Set pointer to CallIndicator.
// ---------------------------------------------------------------------------
//
void CMIDNetworkIndicator::SetCallIndicator(
    CMIDCallIndicator* aCallIndicator)
{
    iCallIndicator = aCallIndicator;
}

// End of File.
