/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMIDNETWORKINDICATOR_H
#define CMIDNETWORKINDICATOR_H


/**
 * MConnectionMonitorObserver interface in CMIDNetworkIndicator class,
 * declaration using RConnectionMonitor as member of CMIDNetworkIndicator.
 */
#include <rconnmon.h>
#include <es_sock.h>
#include <es_enum.h>

class CMIDCanvas;
class CMIDCallIndicator;

/**
 * Class for checking network connection and showing the network indicator on
 * Canvas.
 *
 * @lib    lcdui.dll
 * @since  S60 v3.2
 */
NONSHARABLE_CLASS(CMIDNetworkIndicator) : public MConnectionMonitorObserver
{
    /* CMIDNetworkIndicator is a helper class for CMIDCanvas. */
    friend class CMIDCanvas;

    /* Functionality of both classes is tightly connected. */
    friend class CMIDCallIndicator;


public:

    /**
     * Two-phased constructor.
     *
     * @param  aCanvas Pointer to Canvas for which this network indicator is
     *         created.
     */
    static CMIDNetworkIndicator* NewL(CMIDCanvas* aCanvas);

    /**
     * Destructor.
     */
    ~CMIDNetworkIndicator();

    /**
     * Get the location of the network indicator.
     *
     * @since  S60 v5.0
     * @param  aPosition Returned indicator position.
     * @param  aSize Returned indicator size.
     */
    void GetLocation(TPoint& aPosition, TSize& aSize);


protected:

// from base class MConnectionMonitorObserver

    /**
     * From MConnectionMonitorObserver.
     * Called by the Connection Monitor when network status changes.
     *
     * @since  S60 v3.2
     * @param  aConnMonEvent Connection monitor event.
     */
    void EventL(const CConnMonEventBase& aConnMonEvent);


private:

    /**
     * Constructor.
     * @see CMIDCanvas::NewL()
     */
    CMIDNetworkIndicator();

    /**
     * Second phase construction.
     *
     * @since  S60 v3.2
     * @param  aCanvas Pointer to Canvas for which this network indicator is
     *         created.
     */
    void ConstructL(CMIDCanvas* aCanvas);

    /**
     * Initialize network resource.
     *
     * @since  S60 v3.2
     */
    void InitNetworkResource();

    /**
     * Main method to get the status of the network.
     * Checks how many connections are opened and sets the bearer type and
     * state of the connection.
     *
     * @since  S60 v3.2
     * @param  aConActivity Indicates current connection activity.
     */
    void ConnectionStatus(const TBool aConActivity);

    /**
     * Check the connection type.
     *
     * @since  S60 v3.2
     * @param  aNetworkBearer Network bearer.
     * @return ETrue if there is valid CSD/GPRS/CDMA connection, EFalse
     *         otherwise.
     */
    TBool IsGprsOrCsdConnection(TInt aNetworkBearer);

    /**
     * Tells whether there is some network activity or not.
     *
     * @since  S60 v5.0
     * @return Returns member iActivity value. ETrue if data are being
     *         transferred, EFalse otherwise.
     */
    TBool IsNetworkActive();

    /**
     * Close the connection.
     *
     * @since  S60 v3.2
     */
    void Close();

    /**
     * Draw network indicator.
     *
     * @since  S60 v3.2
     * @param  aGc Graphics context.
     * @param  aRect Area to be redrawn.
     * @param  aViewRect View rectangle.
     */
    void DrawNetworkIndicator(
        CFbsBitGc& aGc,
        const TRect& aRect,
        const TRect& aViewRect);

    /**
     * Repaint the network indicator.
     * This is called after there was a change in the network connection
     * status in order to update the network indicator bitmap.
     *
     * @since  S60 v3.2
     * @param  aRedraw Optional parameter. Invokes repaint if value is ETrue.
     */
    void Repaint(TBool aRedraw = EFalse);

    /**
     * Create the network resource for different bearers.
     * Later during drawing we will use these bitmaps.
     *
     * @since  S60 v3.2
     * @param  aBearer Network bearer.
     * @param  aStatus Status of the network bearer.
     */
    void CreateNetworkResourceL(TInt aBearer, TInt aStatus);

    /**
     * Clean network resource.
     *
     * @since  S60 v3.2
     */
    void CleanNetworkResource();

    /**
     * Clean temporary network resource.
     *
     * @since  S60 v5.0
     */
    void CleanTempNetworkResource();

    /**
     * Copy temp bitmaps to the bitmaps.
     *
     * @since  S60 v3.2
     */
    void CopyBitmaps();

    /**
     * Resize the network indicator bitmap according to the layout data.
     *
     * @since  S60 v3.2
     */
    void ResizeBitmap();

    /**
     * Get the layout for the network indicator bitmap.
     *
     * @since  S60 v3.2
     */
    void GetLayoutData();

    /**
     * Get member iDrawIndicator value.
     *
     * @since  S60 v5.0
     * @retun  ETrue if drawing indicator is enabled, EFalse otherwise.
     */
    TBool GetDrawIndicator();

    /**
     * Set member iDrawIndicator value.
     *
     * @since  S60 v5.0
     * @param  aDrawIndicator Indicates whether to draw indicator or not.
     */
    void SetDrawIndicator(TBool aDrawIndicator);

    /**
     * Set pointer to CallIndicator
     *
     * @since  S60 v5.0
     * @param  aCallindicator Pointer to CallIndicator.
     */
    void SetCallIndicator(CMIDCallIndicator* aCallIndicator);


private: // data

    /**
     * Canvas for which the network indicator is created.
     * Not own.
     */
    CMIDCanvas* iCanvas;

    /**
     * Connection monitor.
     */
    RConnectionMonitor iConnectionMonitor;

    /**
     * Connection bearer.
     */
    TInt iConnectionBearer;

    /**
     * Connection status.
     */
    TInt iConnectionStatus;

    /**
     * Connections count.
     */
    TUint iConnectionCount;

    /**
     * Size of network indicator.
     */
    TSize iNetworkIndicatorSize;

    /**
     * Position of network indicator.
     */
    TPoint iNetworkIndicatorPosition;

    /**
     * Indicates whether there is a network activity or not.
     */
    TBool iNetworkActivity;

    /**
     * Call indicator.
     * Not own.
     */
    CMIDCallIndicator* iCallIndicator;

    /**
     * Indicates whether drawing of network activity is enabled or not.
     */
    TBool iDrawIndicator;

    /**
     * Bitmaps used when drawing indicators.
     *
     * Bitmap to a store network icon during a call.
     * Own.
     */
    CFbsBitmap* iNetworkBitmap;

    /**
     * Bitmap to a store network icon mask during a call.
     * Own.
     */
    CFbsBitmap* iNetworkMaskBitmap;

    /**
     * Temporary bitmap to a store network icon during a call.
     * Own.
     */
    CFbsBitmap* iTempNetworkBitmap;

    /**
     * Temporary bitmap to a store network icon mask during a call.
     * Own.
     */
    CFbsBitmap* iTempNetworkMaskBitmap;
};

#endif // CMIDNETWORKINDICATOR_H
