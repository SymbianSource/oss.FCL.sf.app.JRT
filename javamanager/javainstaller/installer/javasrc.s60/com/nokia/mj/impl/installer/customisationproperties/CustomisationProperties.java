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


package com.nokia.mj.impl.installer.customisationproperties;

import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.utils.Uid;

import java.util.Hashtable;
import java.util.Vector;


/**
 * Fetches product program/operator specific customizable settings.
 * In S60 platform some of these are fetched from central repository,
 * rest are asked from S60 ApplicationShell
 */
public final class CustomisationProperties extends CustomisationPropertiesBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    public CustomisationProperties()
    {
        // create iProperties
        super();

        // contains Strings, used to add String values to iProperties
        Vector value = new Vector(5);
        // contains Integers, used to add Integer values to iProperties
        Vector integerValue = new Vector(5);

        // Which drive is S60 default phone memory drive.
        // Using the phone memory drive as the default installation drive
        // for performance reasons (starting application from flash memory is
        // faster than starting from e.g. memory card)
        int driveNum = 2; // default drive is 'C:'
        try
        {
            driveNum = SysUtil.getDefaultPhoneMemory();
        }
        catch (InstallerException e)
        {
            Log.logWarning(
                "CustomisationProperties using default value, error ",
                e);
        }
        // In Symbian drive 'A' has number 0, 'B' has number 1 etc
        integerValue.addElement(new Integer(driveNum));
        iProperties.put(IntegerPropertyPrefix + DefaultInstallationDrive, integerValue);

        // Now add the default installation drive also as String to iProperties
        // to enable asking it in both forms.
        value.addElement(mapDriveToString(driveNum));
        iProperties.put(DefaultInstallationDrive, value);

        // Remember default phone memory drive for later use
        int defaultPhoneMemory = driveNum;


        // Get information of all possible installation drives.
        // In S60 this means all user visible drives.
        integerValue = new Vector(5);
        Vector driveInfos = new Vector();
        Vector driveTypes = new Vector(5);
        try
        {
            SysUtil.getUserVisibleDrives(driveInfos);
            // now driveInfos contains a vector of DriveInfo objects,
            // split it into two vectors containing Integer values
            // (drive number and drive type)
            for (int nInd = 0; nInd < driveInfos.size(); nInd++)
            {
                integerValue.addElement(
                    new Integer(
                        ((DriveInfo)driveInfos.elementAt(nInd)).getNumber()));
                driveTypes.addElement(
                    new Integer(
                        ((DriveInfo)driveInfos.elementAt(nInd)).getDriveType()));
            }
        }
        catch (InstallerException e)
        {
            Log.logWarning(
                "CustomisationProperties using default value, error ",
                e);
            integerValue.addElement(new Integer(2));  // 'C:'
            integerValue.addElement(new Integer(4));  // 'E:'
            // C: drive is internal device memory
            driveTypes.addElement(new Integer(DriveInfo.PHONE_MEMORY));
            // E: drive is memory card
            driveTypes.addElement(new Integer(DriveInfo.MEMORY_CARD));
        }
        iProperties.put(IntegerPropertyPrefix + PossibleInstallationDrives, integerValue);
        iProperties.put(IntegerPropertyPrefix + TypesOfPossibleInstallationDrives, driveTypes);
        // Convert Integer values in vector to Strings
        value = new Vector(5);
        int nCount = integerValue.size();
        for (int nInd = 0; nInd < nCount; nInd++)
        {
            value.addElement(mapDriveToString(((Integer)integerValue.get(nInd)).intValue()));
        }
        iProperties.put(PossibleInstallationDrives, value);

        value = new Vector(5);
        value.addElement(S60DefaultInstallationFolder);
        iProperties.put(DefaultInstallationFolder, value);

        // Read the comma separated logical names of
        // the possible installation folders from cenrep
        String folders = null;
        try
        {
            folders = SysUtil.getRepositoryStringValue(
                          SysUtil.REPO_ID_JAVA_INST_VARIATION,
                          SysUtil.REPO_KEY_JAVA_INST_SHELL_FOLDER_VALUE);
        }
        catch (InstallerException e)
        {
            Log.logWarning(
                "Cannot read S60 possible installation folders, using default. ",
                e);
        }
        value = splitCommaSeparatedString(folders);
        // default installation folder is always allowed
        value.addElement(S60DefaultInstallationFolder);
        iProperties.put(PossibleInstallationFolders, value);

        // The value of this configuration property is a comma separated
        // list of JAD/JAR attribute names. Split the string into
        // single names
        String attributeNames = null;
        try
        {
            attributeNames = SysUtil.getRepositoryStringValue(
                                 SysUtil.REPO_ID_JAVA_INST_VARIATION,
                                 SysUtil.REPO_KEY_JAVA_INST_SHELL_FOLDER_ATTR);
        }
        catch (InstallerException e)
        {
            Log.logWarning(
                "Cannot read S60 shell folder attribute name, using default",
                e);
        }
        value = splitCommaSeparatedString(attributeNames);
        // 'Nokia-MIDlet-Category' must be supported always,
        // so add it if it is not in list
        if ((null == attributeNames) ||
                (attributeNames.indexOf(NokiaShellFolderAttributeName) == -1))
        {
            value.addElement(NokiaShellFolderAttributeName);
        }
        iProperties.put(ShellFolderAttributeNames, value);

        value = new Vector(5);
        integerValue = new Vector(5);

        // By default the work directories of the preinstalled midlets
        // are created to default phone memory if the midlets are preinstalled
        // from ROM or other read only drive.
        value.addElement(mapDriveToString(defaultPhoneMemory));
        integerValue.addElement(new Integer(defaultPhoneMemory));

        iProperties.put(PreinstalledDefaultDrive, value);
        iProperties.put(IntegerPropertyPrefix + PreinstalledDefaultDrive, integerValue);
    }

    /*** ----------------------------- PACKAGE  --------------------------- */

    /**
     * Returns string that contains the Symbian drive letter
     * 'A'..'Z' + ':' that corresponds to the aDrive parameter
     *
     * @param aDrive int 0..25
     * @return drive string
     */
    String mapDriveToString(int aDrive)
    {
        char drive[] = {(char)('A' + aDrive), ':'};
        String strDrive = new String(drive);

        return strDrive;
    }


    /*** ----------------------------- PRIVATE  --------------------------- */

    /**
     * If the name of the installation folder is empty string, S60 AppShell
     * shows the application in the default folder.
     */
    private static final String S60DefaultInstallationFolder = "";

}
