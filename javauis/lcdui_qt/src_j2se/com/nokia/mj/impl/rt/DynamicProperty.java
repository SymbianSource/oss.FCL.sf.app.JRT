/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt;

/**
 * Interface for getting dynamic system property.
 * Dynamic property handler must implement this interface in a package
 * com.nokia.mj.impl.rt.properties + <jsr_package>.<PropertyName> hierarchy.
 *
 */
public interface DynamicProperty
{

    /**
     * Sets the dynamic system property based on the received argument.
     *
     * @param aDynamicPropertyInfo A class containing the system property
     *        name. After returning from the method the class should contain
     *        the value of the property and the info whether the value does
     *        not change during the lifetime of the application.
     */
    public void getProperty(DynamicPropertyInfo aDynamicPropertyInfo);
}
