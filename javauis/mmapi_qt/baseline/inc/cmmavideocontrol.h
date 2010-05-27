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
* Description:  This class is a VideoControl.
*
*/


#ifndef CMMAVIDEOCONTROL_H
#define CMMAVIDEOCONTROL_H

//  INCLUDES
#include "cmmacontrol.h"
#include "mmmasnapshotreadycallback.h"

// mobitv fix  // 3.x QT based UI
//#include "CMIDToolkit.h"
//#include "lcdui.h"
// FORWARD DECLARATIONS
class MMIDComponent;
class MMMAEventPoster;
class MMMAGuiPlayer;
class MMMADisplay;
class CMMASnapshot;
class MMAFunctionServer;
class CMMAEvent;

//  CLASS DECLARATION
/**
*   This class is native side of the VideoControl java object.
*
*
*/
class CMMAVideoControl :
        public CMMAControl,
        public MMMASnapshotReadyCallback/*,    // 3.x QT based UI
        public MMIDEnvObserver*/
{
public:
    /**
     * @param aGuiPlayer Player that will be used.
     */
    IMPORT_C CMMAVideoControl(MMMAGuiPlayer* aGuiPlayer);

    /**
     * Deletes owned objects and removes global reference to
     * associated displayable object.
     */
    IMPORT_C ~CMMAVideoControl();

    /**
     * Finishes video control construction. This construction step
     * must be called from JNI, because JNIEnv is needed to create
     * callback methods to Java side.
     *
     * @param aVideoControl Used video control.
     * @param aControlObject Corresponding Java object.
     * @param aJni JNI environment used.
     * @param aEventPoster Used to post repaint and
     *                     snapshot ready events.
     */
    static void ConstructL(CMMAVideoControl* aVideoControl,
                           jobject aControlObject,
                           MMAFunctionServer* aEventSource,
                           JNIEnv* aJni,
                           MMMAEventPoster* aEventPoster/*,
                           CMIDToolkit* aToolkit*/); // mobitv fix // 3.x QT based UI

public: // New methods
    /**
     * Initializes display. Called when java display mode is initialized.
     * Created MMMADisplay object and sets it to associated gui player.
     *
     * @param aVideoControl Used video control.
     * @param aComponent Component that will be used to create display.
     * @param aDisplayHandle Java handle to created display will be set to
     *                       this variable. Still owned by video control.
     * @param aDeleteRefEvent Event posted in the destructor.
     */
    static void StaticInitL(CMMAVideoControl* aVideoControl,
                            jobject javaDisplayObject,
                            MMAFunctionServer* aEventSource,
                            TInt* aDisplayHandle,
                            CMMAEvent* aDeleteRefEvent,
                            TBool aGuiType);

    /**
     * Dynamic display mode initialization.
     * @see StaticInitL
     * @param aVideoControl Used video control.
     * @param aContentHandle Will contain handle to MMMADirectContent
     * @param aDeleteRefEvent Event posted in the destructor.
     */
    static void StaticInitDynamicModeL(CMMAVideoControl* aVideoControl,
                                       TInt* aContentHandle,
                                       MMAFunctionServer* aEventSource,
                                       jobject aGUIObject,
                                       CMMAEvent* aDeleteRefEvent);


    /**
     * Gets property according to property type.
     *
     * @param aControl Used video control.
     * @param aPropertyType Type defined in Java header.
     * @param aReturnValue Property value will be set to this parameter.
     */
    static void StaticGetPropertyL(CMMAVideoControl* aControl,
                                   TInt aPropertyType,
                                   TInt* aReturnValue);

    /**
     * Sets property that is defined by type. Amount of propery
     * parameters used depends on property type.
     *
     * @param aControl Used video control.
     * @param aPropertyType Type defined in Java header.
     * @param aPropertyA First value.
     * @param aPropertyB Second value.
     */
    static void StaticSetPropertyL(CMMAVideoControl* aControl,
                                   TInt aPropertyType,
                                   TInt aPropertyA,
                                   TInt aPropertyB);

    /**
     * Takes snapshot from the associated gui player. When ready
     * Java side will be informed with iSnapshotReadyMethod.
     *
     * @param aControl Used video control.
     * @param aProperties Properties used to create snapshot.
     */
    static void TakeSnapShotL(CMMAVideoControl* aControl,
                              const TDesC* aProperties);
    static void StaticSetForegroundL(CMMAVideoControl* aControl,
                                     TInt aForeground) ;

public: // From MMMASnapshotReadyCallback
    IMPORT_C void SnapshotReady();

public: // From CMMAControl
    IMPORT_C const TDesC& ClassName() const;
public:
    TBool IsForeground();
    void SetDisplayHandleToJavaPeer(MMAFunctionServer* eventSource ,jobject javaVideoControlPeer);

protected:
	// 3.x QT based UI
  //  void RegisterForegroundListenerL(CMIDToolkit* aToolkit); // mobitv fix

//public: // from MMIDEnvObserver

   // void HandleSwitchOnL(TBool /*aSwitchOn*/);

    //Handles the case when the MIDlet is brought to the foreground.
 //   void HandleForegroundL(TBool /*aForeground*/);

    // Handles a change to resources which are shared accross the environment.
//    void HandleResourceChangeL(TInt /*aType*/);

private:

    void SetForeground(TBool aForeground, TBool aUseEventServer);

protected:
    /**
     * Not owned.
     */
    MMMAEventPoster* iEventPoster;

    /**
     * Not owned.
     */
    MMMAGuiPlayer* iGuiPlayer;

    /**
     * Not owned.
     */
    JNIEnv* iJni;

    /**
     * Owned object. Used to control displayable object.
     */
    MMMADisplay* iDisplay;

    /**
     * Owned object. Created when snapshot is taken and destroyed
     * when snapshot is ready. Destroyed also in destuctor if control is
     * destroyed when taking snapshot.
     */
    CMMASnapshot* iSnapshot;

    /**
     * Java video control object.
     */
    jobject iListenerObject;

    /**
     * Used inform Java side when snapshot is ready or error occures.
     */
    jmethodID iSnapshotReadyMethod;

    /**
     * Owned object. Not created by the class.
     * Event is used to destroy reference to associated displayable object,
     * when video control is destroyed.
     */
    CMMAEvent* iDeleteRefEvent;

    /**
     * When video display is set to full screen mode, old
     * video position & size is stored to this member. When
     * full screen mode is turned off, this member is used to
     * find out if display size has been changed during full
     * screen mode. This is needed to generate a SIZE_CHANGED
     * event.
     */
    TSize iOldDisplaySize;
    // mobitv fix
    //CMIDToolkit* iToolkit;
   // MMIDEnv* iMidEnv;
    TBool iIsForeground;

};

#endif // CMMAVIDEOCONTROL_H
