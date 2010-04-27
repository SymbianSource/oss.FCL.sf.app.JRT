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


package com.nokia.mj.impl.installer.integrityservice;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;

import java.io.IOException;

/**
 * Operand class holds integrity service Operand information.
 * @see Operation
 */
class Operand
{
    // Drive states.
    private static final int DRIVE_UNKNOWN = -1;
    private static final int DRIVE_PRESENT = 1;
    private static final int DRIVE_NOT_PRESENT = 2;

    // Arrays for caching drive info so that info is not
    // fetched multiple times during installation.
    // These are static, i.e. shared between all Operand instances.
    private static final int MAX_DRIVE_COUNT = 26;
    private static int[] iDriveStates = new int[MAX_DRIVE_COUNT];
    private static int[] iDriveUniqIds = new int[MAX_DRIVE_COUNT];

    // Operand member data.
    private int iDrive = DRIVE_UNKNOWN; // a=0, b=1, ...
    private int iDriveUniqId = DRIVE_UNKNOWN; // Unique drive identifier
    private String iFile = null;

    static
    {
        // Initialize drive info arrays.
        for (int i = 0; i < MAX_DRIVE_COUNT; i++)
        {
            iDriveStates[i] = DRIVE_UNKNOWN;
            iDriveUniqIds[i] = DRIVE_UNKNOWN;
        }
    }

    /**
     * This constructor is called from Operator constructor.
     */
    public Operand(String aFile) throws IOException
    {
        iFile = FileOps.getCanonicalPath(aFile);
        iDrive = FileOps.getDrive(iFile);
        iDriveUniqId = findDriveUniqId(iDrive);
    }

    /**
     * This constructor is called when Operator is read from journal file.
     */
    public Operand(int aDrive, int aDriveUniqId, String aFile)
    throws IOException
    {
        iDrive = aDrive;
        iDriveUniqId = aDriveUniqId;
        iFile = aFile;
    }

    public int getDrive()
    {
        return iDrive;
    }

    public int getDriveUniqId()
    {
        return iDriveUniqId;
    }

    public String getFile()
    {
        return iFile;
    }

    /**
     * Returns true if it is possible to make a rollback for
     * this operand. In order for the rollback to be possible,
     * the operand drive must be present and present drive's
     * unique id must match to the one in the operand.
     */
    public boolean canRollback()
    {
        if (!isDrivePresent())
        {
            Log.logError("IntegrityService Operand.canRollback: " +
                         "cannot rollback because drive " + getDrive() +
                         " is not present");
            Log.logError("IntegrityService Operand.canRollback: " +
                         "failed operand: " + toString());
            return false;
        }
        int currentUniqId = findDriveUniqId(getDrive());
        if (getDriveUniqId() != currentUniqId)
        {
            Log.logError("IntegrityService Operand.canRollback: " +
                         "cannot rollback because drive " + getDrive() +
                         " unique id [" + currentUniqId +
                         "] does not match to the one in operand");
            Log.logError("IntegrityService Operand.canRollback: " +
                         "failed operand: " + toString());
            return false;
        }
        return true;
    }

    /**
     * Returns true if drive for this operand is present.
     * If drive is not specified in the operand, this method
     * returns true.
     */
    synchronized private boolean isDrivePresent()
    {
        int drive = getDrive();
        if (drive == DRIVE_UNKNOWN)
        {
            // Drive is not specified, assume that drive is present.
            return true;
        }
        boolean present = false;
        int driveState = iDriveStates[drive];
        switch (driveState)
        {
        case DRIVE_UNKNOWN:
            present = SysUtil.isDrivePresent(drive);
            iDriveStates[drive] =
                (present? DRIVE_PRESENT: DRIVE_NOT_PRESENT);
            break;
        case DRIVE_PRESENT:
            present = true;
            break;
        case DRIVE_NOT_PRESENT:
            present = false;
            break;
        }
        return present;
    }

    /**
     * Returns drive unique identifier for given drive letter.
     * Drive letters are specified starting from 0 as follows:
     * 0=A, 1=B, 2=C, ..., 25=Z.
     * If drive is not known (given drive parameter value is -1)
     * this method returns -1 as drive unique id.
     */
    synchronized private static int findDriveUniqId(int aDrive)
    {
        if (aDrive == DRIVE_UNKNOWN)
        {
            // Drive is not specified, so we cannot return
            // unique id for it.
            return DRIVE_UNKNOWN;
        }
        if (aDrive < 0 || aDrive >= MAX_DRIVE_COUNT)
        {
            InstallerException.internalError
            ("IntegrityService Operand.findDriveUniqId: invalid drive index " +
             aDrive);
        }
        int uniqId = iDriveUniqIds[aDrive];
        if (uniqId == DRIVE_UNKNOWN)
        {
            uniqId = SysUtil.getDriveUniqId(aDrive);
            iDriveUniqIds[aDrive] = uniqId;
        }
        return uniqId;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("[").append(intToHex(getDrive(), 1)).append("]");
        buf.append("[").append(intToHex(getDriveUniqId(), 4)).append("]");
        buf.append(getFile());
        return buf.toString();
    }

    /**
     * Returns hexadecimal string containing aByteCount least
     * significant bytes from given aInt value.
     */
    public static String intToHex(int aInt, int aByteCount)
    {
        int[] bytes = new int[aByteCount];
        for (int i = 0; i < bytes.length; i++)
        {
            bytes[i] = aInt & 0xff;
            aInt >>= 8;
        }
        StringBuffer result = new StringBuffer();
        for (int i = bytes.length-1; i >= 0; i--)
        {
            if (bytes[i] < 16)
            {
                result.append("0");
            }
            result.append(Integer.toString(bytes[i], 16));
        }
        return result.toString();
    }
}
