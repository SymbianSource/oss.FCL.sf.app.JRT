/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Hashtable;

/**
 * MidpAttributeValidator makes syntax checks for JAD and JAR (Manifest)
 * attributes. It also combines JAD and JAR attributes according to rules
 * from MIDP specification.
 *
 * JavaInstaller specific MidpAttributeValidator extends
 * com.nokia.mj.impl.utils.MidpAttributeValidator
 * by adding validation of Nokia-MIDlet-UID-n attributes.
 *
 * @see com.nokia.mj.impl.utils.MidpAttributeValidator
 */
public class MidpAttributeValidator
        extends com.nokia.mj.impl.utils.MidpAttributeValidator
{
    /**
     * Default constructor.
     */
    public MidpAttributeValidator()
    {
        super();
    }

    /**
     * Method for making info log entries.
     */
    protected void logInfo(String aMsg)
    {
        Log.log(aMsg);
    }

    /**
     * Method for making error log entries.
     */
    protected void logError(String aMsg)
    {
        Log.logError(aMsg);
    }

    /**
     * Makes extended check for attributes. Subclasses can add their
     * own attribute validation checks by overriding this method.
     * This method gets called when validate method is called.
     *
     * @param aAttrs attribute table to be validated
     * @throws InvalidAttributeException if attribute validation fails
     */
    protected void checkExtensions(Hashtable aAttrs)
    throws InvalidAttributeException
    {
        super.checkExtensions(aAttrs);
        checkNokiaMidletUid(aAttrs);
    }

    /**
     * Checks Nokia-MIDlet-UID-n attribute value validity.
     */
    private void checkNokiaMidletUid(Hashtable aAttrs)
    {
        // Check Nokia-MIDlet-UID-n attribute values
        for (int i = 1; true; i++)
        {
            String key = "Nokia-MIDlet-UID-" + i;
            Attribute attr = (Attribute)aAttrs.get(key);
            if (attr == null)
            {
                break; // no more Nokia-MIDlet-UID-n attributes
            }
            Uid uid = PlatformUid.createUid(attr.getValue());
            if (uid == null)
            {
                invalidAttribute(attr, "not valid UID");
            }
        }
    }
}
