/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for global volume
*
*/


#ifndef CMMAGLOBALVOLUME_H
#define CMMAGLOBALVOLUME_H

// INTERNAL INCLUDES
#include "cmmavolumecontrol.h"
#include "mmmavolumekeyslistener.h"

class CMMAPlayer;
class CMMAForeground;
class CRepository;
class CMMAVolumeKeysListener;

//  CLASS DECLARATION
/**
 * This class is used for global volume setting
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CMMAGlobalVolume) : public CBase,
        public MMMAVolumeKeysListener
{
private: // Type definitions

    // Volume control, index and player holder
    class TMMAPlayerHolder
    {
    public:
        // Volume index
        TInt iVolumeIndex;
        // Player which contains the volume control
        CMMAPlayer* iPlayer;
        // Volume control from the player
        CMMAVolumeControl* iVolumeControl;
    };

public: // Constructor and destructors

    /**
     * NewL
     * Creates an instance from this class
     * @param aForeground Foreground listener. The ownership is transferred
     *        to this class
     * @return New instance from this class. The caller takes ownership
     *         of the returned object
     */
    static CMMAGlobalVolume* NewL(CMMAForeground* aForeground);

    /**
     * Destructor
     */
    virtual ~CMMAGlobalVolume();

public: // New methods

    /**
     * Adds new player for listening global volume. Players which
     * do not have volume control are simply ignored
     *
     * @param aPlayer Player which receives volume key notifications
     */
    void AddPlayerL(CMMAPlayer* aPlayer);

    /**
     * Removes player from listening global volume
     */
    void RemovePlayer(CMMAPlayer* aPlayer);

public: // From MMMAVolumeKeysListener

    /**
     * Handles volume up event
     */
    void VolumeUp();

    /**
     * Handles volume down event
     */
    void VolumeDown();

private: // New methods

    /**
     * Finds volume control from a player
     *
     * @param aPlayer Player from which the volume control is searched
     * @return Volume control instance. NULL is returned if the player
     *         does not support volume control feature
     */
    CMMAVolumeControl* FindVolumeControl(CMMAPlayer* aPlayer);

    /**
     * Sets volume level for all registered volume controls
     * @param aLevel New volume level
     */
    void SetControlVolumeLevels(TInt aLevel);

private: // Private constructors

    /**
     * C++ constructor
     * @param aForeground Foreground listener
     */
    CMMAGlobalVolume(CMMAForeground* aForeground);

    /**
     * Second phase constructor
     */
    void ConstructL();

protected: // Data

    // Current global volume level
    TInt iLevel;
    // Foreground listener. Owned
    CMMAForeground* iForeground;
    // MMA setting repository. Owned
    CRepository* iSettingsStore;
    // Volume keys listener. Owned
    CMMAVolumeKeysListener* iVolumeKeysListener;
    // Array of volume controls
    RArray< TMMAPlayerHolder > iControlList;
};

#endif // CMMAGLOBALVOLUME_H

// End of file
