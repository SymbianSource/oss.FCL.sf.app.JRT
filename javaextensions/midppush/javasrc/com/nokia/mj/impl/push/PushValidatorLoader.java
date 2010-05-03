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


package com.nokia.mj.impl.push;

import java.io.IOException;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Logger;
import java.lang.String;
import java.lang.StringBuffer;
import javax.microedition.io.ConnectionNotFoundException;
import java.lang.ClassNotFoundException;
import java.lang.IllegalArgumentException;
import java.lang.IllegalAccessException;
import java.lang.InstantiationException;
import java.util.Hashtable;

/**
 * This class parses scheme from URI, loads protocol instance of specific PushValidator interface
 * and validates URI and 'filter' arguments passed as arguments to PushRegistry.registerConnection()
 * operation or read from .jad/.jar files(= static push registration).
 * Protocol specific validator is searched from com.nokia.mj.impl.gcf.protocol.XXX' package. XXX means
 * scheme of the protocol(e.g. 'com.nokia.mj.impl.gcf.protocol.http') and name of the implementation
 * class must be PushValidatorImpl.
 */
final class PushValidatorLoader
{
    private static final String PROTOCOL_PACKAGE_PATH = "com.nokia.mj.impl.gcf.protocol.";
    private static final String NAME_OF_VALIDATOR_CLASS = ".PushValidatorImpl";
    private static PushValidatorLoader sValidatorLoaderObj = null;

    Hashtable mValidatorContainer = null;

    public PushValidatorLoader()
    {
        mValidatorContainer = new Hashtable();
    }

    public static synchronized PushValidatorLoader getInstance()
    {
        if (null == sValidatorLoaderObj)
            sValidatorLoaderObj = new PushValidatorLoader();
        return sValidatorLoaderObj;
    }

    /**
     * This operation parses scheme from URI, loads protocol instance of specific PushValidator interface
     * and validates URI and 'filter' arguments.
     * @param aUri  push URI.
     * @param aFilter 'filter' argument of the push registration.
     * @param aSuiteUid MIDlet suite uid.
     * @param aMidletName name of the MIDlet.
     * @param aIsStaticRegistration indicates whether static/dynamic push registration is ongoing.
     * @throws IllegalArgumentException if URI is not valid.
     *         ConnectionNotFoundException if validation class can not be loaded.
     */
    public void validatePushArguments(String aUri,String aFilter,Uid aSuiteUid,
                                      String aMidletName,
                                      boolean aIsStaticRegistration)
    throws ConnectionNotFoundException, IOException
    {
        Logger.LOG(Logger.EJavaPush,Logger.EInfo,"+ PushValidatorLoader.validatePushArguments()");

        try
        {
            if ((null == aUri) || (null == aFilter))
                throw new IllegalArgumentException("URI/filter argument is null");

            if ((true == aUri.equals("")) || (true == aFilter.equals("")))
                throw new IllegalArgumentException("URI/filter argument string is empty");

            PushValidator validatorInstance = getPushValidator(aUri);
            validatorInstance.validate(aUri,aFilter,aSuiteUid,aMidletName,aIsStaticRegistration);

            Logger.LOG(Logger.EJavaPush,Logger.EInfo,"- PushValidatorLoader.validatePushArguments()");
        }
        catch (ClassNotFoundException ex)
        {
            throw new ConnectionNotFoundException("Unsupported URI scheme");
        }
        catch (IllegalAccessException ex)
        {
            throw new ConnectionNotFoundException("Validator class of the URI is not accessible");
        }
        catch (InstantiationException ex)
        {
            throw new ConnectionNotFoundException("Validator object of the URI can not be instantiated");
        }
    }//end validatePushArguments

    /**
     *
     */
    private PushValidator getPushValidator(String aUri)
    throws ClassNotFoundException, IllegalAccessException, InstantiationException
    {
        //Resolve scheme from the URI.
        int index = aUri.indexOf(':');
        if (-1 == index)
            throw new IllegalArgumentException("Uri is not valid");
        String schemeStr = aUri.substring(0,index);
        if (true == schemeStr.equals("secure-element"))
            schemeStr = "secureelement";

        PushValidator validatorObj = (PushValidator) mValidatorContainer.get(schemeStr);
        if (null != validatorObj)
            return validatorObj;

        StringBuffer nameOfValidatorClass = new StringBuffer(PROTOCOL_PACKAGE_PATH.length()
                + schemeStr.length()
                + NAME_OF_VALIDATOR_CLASS.length());
        nameOfValidatorClass.append(PROTOCOL_PACKAGE_PATH);
        nameOfValidatorClass.append(schemeStr);
        nameOfValidatorClass.append(NAME_OF_VALIDATOR_CLASS);
        Class clazz = Class.forName(nameOfValidatorClass.toString());
        validatorObj = (PushValidator) clazz.newInstance();

        mValidatorContainer.put(schemeStr,validatorObj);
        return validatorObj;
    }//end getPushValidator


}//end PushValidatorLoader

