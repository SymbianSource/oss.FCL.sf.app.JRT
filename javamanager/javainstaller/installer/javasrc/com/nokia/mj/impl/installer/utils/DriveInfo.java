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
* Description:
*
*/


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.utils.Uid;


/**
 * Contains all information about a device drive needed by UI.
 */
public final class DriveInfo
{
    /**
     * Unknown drive type
     */
    public static final int UNKNOWN_DRIVE = 0;

    /**
     * Device memory
     */
    public static final int PHONE_MEMORY = 1;

    /**
     * Physically removable memory card
     */
    public static final int MEMORY_CARD = 2;

    /**
     * Internal mass storage is e.g. hard disk or memory card that cannot be removed
     */
    public static final int INTERNAL_MASS_STORAGE = 3;


    /**
     * Drive number. In Symbian OS A: drive is 0, B: is 1, C: is 2,... Z: is 25
     * In Linux: ?
     */
    protected int iDriveNumber;

    /**
     * The raw drive status.
     */
    protected int iDriveStatus;

    /**
     * Available free space on the drive.
     */
    private long iFreeSpace;

    // necessary S60 drive status bit masks

    // drive is internal and cannot be physically removed.
    private static final int E_DRIVE_INTERNAL = 0x1;

    // drive is physically removable.
    private static final int E_DRIVE_REMOVABLE = 0x2;

    // drive is externally mountable i.e. can be mounted from PC or from other devices.
    private static final int E_DRIVE_EXTERNALLY_MOUNTABLE = 0x400;


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Construct new DriveInfo object.
     *
     * @param aDriveNumber   The number of drive
     * @param aDriveStatus   The status of drive
     * @param aFreeSpace     The amount of free space in the drive
     */
    public DriveInfo(
        int aDriveNumber, int aDriveStatus, long aFreeSpace)
    {
        iDriveNumber = aDriveNumber;
        iDriveStatus = aDriveStatus;
        iFreeSpace = aFreeSpace;
    }

    /**
     * Get drive number.
     */
    public int getNumber()
    {
        return iDriveNumber;
    }

    /**
     * Returns the type of the drive.
     */
    public int getDriveType()
    {
        if ((iDriveStatus & E_DRIVE_INTERNAL) != 0)
        {
            if ((iDriveStatus & E_DRIVE_EXTERNALLY_MOUNTABLE) != 0)
            {
                return INTERNAL_MASS_STORAGE;
            }
            else
            {
                return PHONE_MEMORY;
            }
        }

        if ((iDriveStatus & E_DRIVE_REMOVABLE) != 0)
        {
            if ((iDriveStatus & E_DRIVE_EXTERNALLY_MOUNTABLE) != 0)
            {
                return MEMORY_CARD;
            }
        }

        return UNKNOWN_DRIVE;

    }

    /**
     * Get amount of free space on the drive.
     */
    public long getFreeSpace()
    {
        return iFreeSpace;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("DriveInfo:\n");
        buf.append("Number: ").append(iDriveNumber).append("\n");
        buf.append("Status: ").append(iDriveStatus).append("\n");
        buf.append("FreeSpace: ").append(iFreeSpace);
        return buf.toString();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Deny other code from using default constructor.
     */
    private DriveInfo()
    {
    }

}
