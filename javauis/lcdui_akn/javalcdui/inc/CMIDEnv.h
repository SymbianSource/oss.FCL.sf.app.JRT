/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef CMIDENV_H
#define CMIDENV_H

#include <e32def.h>
#include <lcdui.h>

class CMIDKeyTranslator;
class CMIDToolkit;
class CMIDToLcduiObserver;


/**
 *
 * A MIDlet environment, one per MIDlet, providing utility methods for the
 * MIDP implementation
 *
 */
class CMIDEnv : public CBase, public MMIDEnv
{
public:
    CMIDEnv(CMIDToolkit* aToolkit, const TSize& aSize);
    void ConstructL();
    ~CMIDEnv();

    void SetUtils(MMIDUtils* aUtils);

public:
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TEventType aEventType,TInt aEventData,TInt aEventData1,TInt aEventData2);
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TEventType aEventType=ENoType,TInt aEventData=0);
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TInt aEventData);
    virtual TBool PostMidletEvent(TEventType aEventType);

    virtual MMIDNotifyEvent* NewNotifyL(MMIDComponent& aSource);
    virtual TBool   PostJavaNotify(MMIDNotifyEvent* aNotify);

    virtual TBool   TranslateKeyL(TMIDKeyEvent& aEvent, const TKeyEvent&, TEventCode aType);
    virtual TBool   PostKeyEvent(MMIDComponent& aSource, TMIDKeyEvent& aEvent);
    virtual void    ResetKeys();

    virtual MMIDDisplayable* Current();
    virtual TPtrC MidletName() const;
    virtual TUid MidletUid() const;
    virtual TPtrC MidletHome() const;
    virtual void DisplayableIsDestructed(const MMIDDisplayable* displayable);

#ifdef RD_SCALABLE_UI_V2
    // This function can be moved out from RD_SCALABLE_UI_V2 flag if needed.
    // It is behind this flag because currently it is used only by Touch.
    /**
    * Returns MIDlet Suite uid that is parsed from the MIDlet home path.
    * In practise this function assumes that the midlet home path includes
    * Midlet suite uid inside the brackets, for example:
    * "e:\epoc32\winscw\c\private\102033E6\MIDlets\[101e0a27]"
    *
    * Note:
    * MIDlet suite uid could be retrieved from the Java registry based on
    * the midlet uid. But that operation is very time consuming because it
    * requires lot of file accessing, especially when there are many midlets
    * installed in the device.
    * @since S60 5.0
    * @return   TUid iMidletSuiteUid or
    *            KNullUidValue if midlet suite uid not found or error occured.
    */
    TUid MidletSuiteUid();
#endif //RD_SCALABLE_UI_V2

    virtual TInt NumColors();
    virtual TDisplayMode DisplayMode();

    virtual void  SetCanvasZoomSize(const TSize& aSize);
    virtual TSize CanvasZoomSize();
    virtual TSize CanvasAssumedSize();
    virtual TInt MidletAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue);

    /**
     * @see MMIDEnv#MidletAttributeIsSetToVal()
     */
    virtual TBool MidletAttributeIsSetToVal(const TDesC& aAttributeName, const TDesC& aAttributeValue);

    /**
     * @see MMIDEnv#MidletAttributeContainsVal()
     */
    virtual TBool MidletAttributeContainsVal(const TDesC& aAttributeName, const TDesC& aAttributeValue);

    virtual void SetCanvasAssumedSize(const TSize& aSize);

    //
    virtual void AddObserverL(MMIDEnvObserver& aObserver);
    virtual void RemoveObserver(MMIDEnvObserver& aObserver);
    //
    void HandleSwitchOnL(TBool aSwitchOn);
    void HandleForegroundL(TBool aForeground);
    void HandleResourceChangeL(TInt aType);

    /**
     * @see MMIDEnv#ReserveCanvasFrameBuffer()
     */
    virtual CFbsBitmap* ReserveCanvasFrameBufferL(MMIDCanvas& aCanvas, const TSize& aSz);

    /**
     * @see MMIDEnv#ReleaseCanvasFrameBuffer()
     */
    virtual void ReleaseCanvasFrameBuffer(MMIDCanvas& aCanvas, CFbsBitmap*& aFrameBuffer);

    /**
     * @see MMIDEnv#CanvasHasBackground()
     */
    virtual TBool CanvasHasBackground(const MMIDCanvas& aCanvas) const;

    /**
     * Maps low level key event
     * @param    aMidKeyEvent key event output data
     * @param    aEvent key event input data
     * @see    virtual void MMIDEnv::MappingDataForKey( TKeyEvent& aEvent );
     */
    virtual void MappingDataForKey(TKeyEvent& aEvent, TEventCode aType);

    /**
     * Set last key event
     * @param    aEvent key event input data
     * @see    virtual void MMIDEnv::SetLastKeyEvent( const TKeyEvent& aEvent );
     */
    virtual void SetLastKeyEvent(const TKeyEvent& aEvent);

    /**
     * From MMIDEnv.
     * Gets an instance of ToLcduiObserver.
     *
     * @since S60 5.0
     * @return Pointer to ToLcduiObserver instance
     * @see iToLcduiObserver
     */
    virtual MMIDToLcduiObserver& ToLcduiObserver();

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * @see MMIDEnv#IsHardwareAccelerated()
     */
    virtual TBool IsHardwareAcceleratedL(
        MMIDEnv::THardwareType aHardwareType = MMIDEnv::EHardware3D);

private:
    /**
     * Checks and initializes 2D & 3D harware status
     * @return Hardware status @see MMIDEnv::THardwareType
     * @since S60 9.2
     */
    TInt InitHardwareStatusL();

#endif // RD_JAVA_NGA_ENABLED


private:
    /**
     * Encapsulated canvas (frame buffer) data
     */
    struct TCanvasData
    {

        /**
         * Ctor
         */
        TCanvasData()
                : iRefCount(0), iFrameBuffer(NULL), iHasBackground(EFalse)
        {
        }

        /**
         * Reset frame buffer data
         */
        inline void Reset()
        {
            delete iFrameBuffer;
            iFrameBuffer = NULL;
            iRefCount = 0;
        }
        TUint                       iRefCount;
        CFbsBitmap*                 iFrameBuffer;
        TBool                       iHasBackground;
        RMutex                      iLock;
    };

    /**
     * Autolocker class.
     */
    class TMutexAutoLock
    {
    public:
        /**
         * Ctor
         * @param aLock Syncronization primitive that is locked during construction
         */
        TMutexAutoLock(RMutex* aLock) : iLock(aLock)
        {
            iLock->Wait();
        }

        /**
         * Dtor. Signals the synchronization primitive
         */
        ~TMutexAutoLock()
        {
            iLock->Signal();
        }
    private:
        RMutex* iLock;
    };


    /**
     * Create a frame buffer for a <code>MMIDCanvas</code> instance.
     * @param   aCanvas Target canvas
     * @param   aWsBitmap If <code>ETrue</code> then the type of the frame buffer
     *          will be <code>WsBitmap</code> instead of a <code>CFbsBitmap</code>.
     * @param   aSz Frame buffer size
     * @return  Frame buffer instance
     * @since S60 5.0
     */
    CFbsBitmap* CreateCanvasFrameBufferL(
        MMIDCanvas& aCanvas, TBool aWsBitmap, const TSize& aSz);

private:
    enum {EGranularity = 4};

private:
    CMIDToolkit*                iToolkit;
    RArray<MMIDEnvObserver*>    iObservers;
    CMIDKeyTranslator*          iKeyTranslator;
    TSize                       iCanvasAssumedSize;
    TSize                       iCanvasZoomSize;
    TCanvasData                 iCanvasData;

#ifdef RD_SCALABLE_UI_V2
    // MIDlet suite uid
    TUid                        iMidletSuiteUid;
#endif //RD_SCALABLE_UI_V2

    /**
     * Provides possibility to make calls from non-LCDUI threads
     * to LCDUI ES thread.
     * Own.
     */
    CMIDToLcduiObserver*        iToLcduiObserver;

#ifdef RD_JAVA_NGA_ENABLED
    TInt                        iHardwareStatus;
#endif // RD_JAVA_NGA_ENABLED
};

#endif // CMIDENV_H
