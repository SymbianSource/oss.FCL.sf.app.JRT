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

import java.util.Hashtable;
import java.util.Vector;


/**
 * Fetches product program/operator specific customizable settings.
 * In Linux platform some of these are fetched from ???
 *
 * @author Nokia Corporation
 * @version $Rev: 10289 $
 */
public final class CustomisationProperties extends CustomisationPropertiesBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    public CustomisationProperties()
    {
        // Load the real value of the properties from system
        super();

        Vector value = new Vector(5);
        // contains Integers, used to add Integer values to iProperties
        Vector integerValue = new Vector(5);

        value.addElement("/");
        iProperties.put(DefaultInstallationDrive, value);
        integerValue.addElement(new Integer(-1));  // Linux version understands this
        iProperties.put(IntegerPropertyPrefix + DefaultInstallationDrive, integerValue);

        value = new Vector(5);
        value.addElement("/");
        iProperties.put(PossibleInstallationDrives, value);
        integerValue = new Vector(5);
        integerValue.addElement(new Integer(-1));  // Linux version understands this
        iProperties.put(IntegerPropertyPrefix + PossibleInstallationDrives, integerValue);

        value = new Vector(5);
        value.addElement("Applications");
        iProperties.put(DefaultInstallationFolder, value);

        value = new Vector(5);
        value.addElement("Applications");
        value.addElement("Games");
        iProperties.put(PossibleInstallationFolders, value);

        value = new Vector(5);
        value.addElement("Nokia-MIDlet-Category");
        iProperties.put(ShellFolderAttributeNames, value);

        value = new Vector(5);
        value.addElement("/");
        iProperties.put(PreinstalledDefaultDrive, value);
        integerValue = new Vector(5);
        integerValue.addElement(new Integer(-1));  // Linux version understands this
        iProperties.put(IntegerPropertyPrefix + PreinstalledDefaultDrive, integerValue);
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

}
