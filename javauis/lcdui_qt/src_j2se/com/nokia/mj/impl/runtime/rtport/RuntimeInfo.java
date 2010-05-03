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
* Description: 
*
*/


package com.nokia.mj.impl.runtime.rtport;

/**
 * Interface for getting runtime information.
 *
 * @see RuntimeInfoFactory
 */
public interface RuntimeInfo
{
    /**
     * Defined constant for manufacturer domain 
     */       
    final String MANUFACTURER_DOMAIN = "MFD";

    /**
     * Defined constant for identified third party domain 
     */       
    final String IDENTIFIED_THIRD_PARTY_DOMAIN = "ITPD";
    /**
     * Defined constant for operator domain 
     */      
    final String OPERATOR_DOMAIN = "OPD";

    /**
     * Defined constant for unidentified third party domain 
     */      
    final String UNIDENTIFIED_THIRD_PARTY_DOMAIN = "UTPD";
    
    /**
     * Gets the UI toolkit register for this runtime environment
     */
    public UiToolkitRegister getUiToolkitRegister();

    /**
     * Returns object representing current application of the caller. The
     * meaning of "current application" is specific to the runtime used
     * (e.g. MIDlets, OSGi).
     *
     * The object has correct hashCode and equals method implementations to
     * support usage as Hashtable key. The Id returned for different
     * applications is unique within the same JVM process.
     *
     * The caller should not keep reference to the value after the application
     * has been closed.
     *
     * @return Object representing current application
     */
    public Object getApplicationId();

    /**
     * Returns the UID of the caller, or -1 if no UID is associated.
     *
     * @return UID of the caller, or -1 if no UID is associated
     */
    public int getApplicationUid();
    
    /**
     * Returns protection domain of current application as String.
     * 
     * Currently there are four defined domains
     *      
     * @see #MANUFACTURER_DOMAIN
     * @see #IDENTIFIED_THIRD_PARTY_DOMAIN
     * @see #OPERATOR_DOMAIN     
     * @see #UNIDENTIFIED_THIRD_PARTY_DOMAIN   
     *        
     * @return protection domain as String
     */
    public String getApplicationDomain();
    
    /**
     * Notifies the runtime that exit command has been received.
     * It is runtime's responsibility to ensure that the application
     * exits surely.
     *
     * @param aUid UID of the application to be closed.
     */
    public void notifyExitCmd(int aUid);

}
