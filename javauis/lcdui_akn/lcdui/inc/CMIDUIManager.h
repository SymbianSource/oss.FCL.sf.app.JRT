/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDUIMANAGER_H
#define CMIDUIMANAGER_H

#include <e32def.h>
#include <coemain.h>
#include <lcdui.h>

#include "CMIDCanvas.h" // required for zoom support

#include "CMIDMenuHandler.h"

const TInt KPESParameterLength = 4;//PointerEventSuppressor parameter max value 9999
const TInt KPESErrorValue = -1;//PointerEventSuppressor parameter error value -1

class CMIDDisplayable;

/**
 * The UIManager class is responsible for persisting data about the
 * state of the UI which would be appropriate to restore should the
 * application be shut down whilst in the background.
 *
 * The only data that is persisted in the reference implementation is
 * the zoom state of the Canvas. This prevents a midlet that has saved
 * its state on being destroyed, and decides to return to a canvas on being
 * restarted from appearing to forget its zoom state whilst in the background
 * - the user may be unaware that the application was actually shutdown whilst
 * in the background.
 *
 */
NONSHARABLE_CLASS(CMIDUIManager) : public CBase
        , public MMIDEnvObserver
{
public:
    static CMIDUIManager* NewL(MMIDEnv& aEnv);
    ~CMIDUIManager();

#ifdef CANVAS_ZOOM_SUPPORT
    enum TZoomState
    {
        EZoomNone=0,
        EZoomFourTimes=1,
        EZoomMIDP1Ref=2,
        EZoomMin=EZoomNone,
        EZoomMax=EZoomMIDP1Ref
    };

    /**
     *@return the current canvas zoom state.
     */
    TInt ZoomState() const;
#endif

    //PointerEventSuppressor default values
    enum TPESValues
    {
        EPESPointerMovementInTwips = 200,//1 twip == 1/1440 of an inch
        EPESTimeInMilliseconds = 350//default time 0.35s
    };


    /**
     * Increase the zoom size for Canvases.
     */
    void  ZoomIn();

    /**
     * Decrease the zoom size for Canvas's
     */
    void  ZoomOut();

    /**
     *@see MMIDEnvObserver
     */
    virtual void HandleResourceChangeL(TInt aType);

    /**
     * Notifies CMIDTickerControlled object, if it exists,
     * about switch on change
     *
     * @see MMIDEnvObserver
     */
    virtual void HandleSwitchOnL(TBool aSwitchOn);

    /**
     * Notifies CMIDTickerControlled object, if it exists,
     * about foreground change
     *
     * @see MMIDEnvObserver
     */
    virtual void HandleForegroundL(TBool aForeground);

    /**
     * Get a pointer to a singleton menu handler.
     * The menu handler is created if not exists.
     *
     * CMIDUIManager object is owning this singleton object and takes
     * care of deleting it as well.
     *
     * @return Pointer to the menu handler
     *
     * @since s60 5.0
     */
    CMIDMenuHandler* OpenMenuHandlerL();

    /**
     * Get a pointer to a singleton menu handler.
     * NOTE: the return value could be NULL.
     *
     * @return Pointer to the menu handler
     *
     * @since s60 5.0
     */
    CMIDMenuHandler* GetMenuHandler();

    /**
     * Get pointer to singleton instance.
     * The instance is created if not exists.
     *
     * CMIDUIManager object is owning this singleton object and takes
     * care of deleting it as well.
     *
     * @return Pointer to this singleton object.
     * @exception If construction fails.
     *
     * @since s60
     */
    CMIDNaviPaneController* OpenNaviPaneControllerL();

    /**
     * Get pointer to singleton instance.
     *
     * NOTE: the return value could be NULL.
     *
     * @return Pointer to this singleton object.
     * @exception If construction fails.
     *
     * @since s60
     */
    CMIDNaviPaneController* GetNaviPaneController();

    /**
     * Get pointer to singleton instance.
     * The instance is created if not exists.
     *
     * CMIDUIManager object is owning this singleton object and takes
     * care of deleting it as well.
     *
     * @return Pointer to this singleton object.
     * @exception If construction fails.
     *
     * @since s60
     */
    CMIDKeyDecoder* OpenKeyDecoderL();

    /**
     * Get pointer to singleton instance.
     *
     * NOTE: the return value could be NULL.
     *
     * @return Pointer to this singleton object.
     * @exception If construction fails.
     *
     * @since s60
     */
    CMIDKeyDecoder* GetKeyDecoder();

    /**
     * Get pointer to singleton object.
     * The instance is created if not exists.
     *
     * Object's lifetime is managed with help of reference counting.
     * NOTE: The caller is responsible to call CMIDUIManager::CloseScaler()
     * method for every CMIDUIManager::OpenScalerL() method call.
     *
     * @return Reference to this singleton object.
     * @exception If construction fails.
     * @see CMIDUIManager::CloseScaler
     *
     * @since s60
     */
    CMIDScaler* OpenScalerL();

    /**
     * Get pointer to singleton instance.
     *
     * NOTE: the return value could be NULL.
     *
     * @param aIsOwner If true then the reference count is increased.
     * @return Pointer to this singleton object.
     * @exception If construction fails.
     *
     * @since s60
     */
    CMIDScaler* GetScaler(TBool aIsOwner = EFalse);

    /**
     * Object's life cycle is controlled with help of reference counting.
     *
     * User must call this method per each CMIDUIManager::ReserveScaler
     * method call.
     *
     * @param aScaler Scaler instance that is going to be release.
     * @see CMIDUIManager::ReserveScaler
     * @since s60
     */
    void CloseScaler(CMIDScaler* aScaler = NULL);

    /**
     * Opens reference to the singleton object.
     * The instance is created if not exists.
     *
     * @param aDisplayable       MMIDisplayable that uses CMIDCanvasKeypad.
     * @return                   Pointer to the CMIDCanvasKeypad instance.
     *                           NOTE: Could also be NULL.
     * @since S60 5.0
     */
    CMIDCanvasKeypad* OpenCanvasKeypadL(MMIDDisplayable* aDisplayable = NULL);

    /**
     * Get pointer to singleton instance.
     *
     * NOTE: the return value could be NULL.
     *
     * @param aIsOwner If true then the reference count is increased.
     * @return                   Pointer to the CMIDCanvasKeypad instance.
     *                           NOTE: Could also be NULL.
     * @since S60 5.0
     */
    CMIDCanvasKeypad* GetCanvasKeypad(TBool aIsOwner = EFalse);

    /**
     * To close the refernce to this object.
     * If after calling CloseCanvasKeypad function there is no reference
     * to this singelton object CMIDKCanvasKeypad instance is destroyed.
     *
     * The caller is responsible to call CloseCanvasKeypad method for every
     * OpenCanvasKeypadL method call.
     *
     * @param aCanvasKeypad Object that is going to be closed
     * @since S60 5.0
     */
    void CloseCanvasKeypad(CMIDCanvasKeypad* aCanvasKeypad = NULL);

    /**
     * Get default displayable
     *
     * @param aCreateIfNeeded If true then object is created if doesn't
     * already exist.
     * @return Default displayable
     * @since s60
     */
    CMIDDisplayable* OpenDefaultDisplayableL(TBool aVisible);

    /**
     * Get default displayable
     *
     * @param aCreateIfNeeded If true then object is created if doesn't
     * already exist.
     * @return Default displayable
     * @since s60
     */
    CMIDDisplayable* GetDefaultDisplayable();

    /**
     * Deactivates default displayable
     *
     * From now on the default displayable can be used
     * only as a background for split view
     *
     * @since s60 9.2
     */
    inline void DeactivateDefaultDisplayable();

    /**
     * Get Gauge item data
     * @return Data pointer
     * @since s60 3.2
     */
    inline void* GetGaugeItemData()
    {
        return iGaugeItemData;
    }

    /**
     * Set Gauge item data
     * @param aGaugeItemData Data pointer
     * @since s60 3.2
     */
    inline void SetGaugeItemData(void* aGaugeItemData)
    {
        iGaugeItemData = aGaugeItemData;
    }

    /**
     * Read PointerEventSuppressor parameter values from JAD file
     * @since S60 5.0
     */
    TSize ReadPointerEventSuppressorValues();

    /**
     * Parse TPtrC to TSize (two values separated with comma)
     *
     * @since S60 5.0
     */
    TSize TwoDesValuesToTSize(TPtrC scrSize);

private:

    /**
     * Ctor
     * @param aEnv Java environment
     *
     * @since s60
     */
    CMIDUIManager(MMIDEnv& aEnv);

    void ConstructL();
    void StoreIniL();
    void RestoreIniL();
    CDictionaryStore* OpenIniFileLC(RFs& aFs) const;

    /**
     * Releases resources occupied by default displayable
     *
     * @since s60
     */
    void CloseDefaultDisplayable();

#ifdef CANVAS_ZOOM_SUPPORT
    void SetZoomState(TInt aState);
#endif

private:
    MMIDEnv&                iEnv;

#ifdef CANVAS_ZOOM_SUPPORT
    TInt                    iZoomState;
#endif
    CMIDMenuHandler*        iMenuHandler;
    CMIDNaviPaneController* iNaviPaneController;
    CMIDKeyDecoder*         iKeyDecoder;
    CMIDScaler*             iScaler;
    CMIDCanvasKeypad*       iCanvasKeypad;
    void*                   iGaugeItemData;
    CMIDDisplayable*        iDefaultDisplayable;
    CMIDDefaultBackground*  iDefaultBackground;
    TBool                   iDefDispDeactivated;
};

inline void CMIDUIManager::DeactivateDefaultDisplayable()
{
    iDefDispDeactivated = ETrue;
}

#endif // CMIDUIMANAGER_H

// End of File
