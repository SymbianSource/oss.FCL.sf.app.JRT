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
 * Class for returning the result value of dynamic system property. It is
 * possible to pass information about can the value change during the
 * application runtime. If it can, then the runtime knows that the original
 * dynamic property handler can be removed and overwritten by the static
 * value.
 */

public class DynamicPropertyInfo
{

    /**
     * Tells whether the dynamic parameter does not change during the lifetime
     * of the application.
     */
    private boolean iIsPropertyValueStatic;

    /**
     * Name of the asked dynamic property.
     */
    private String iPropertyName;

    /**
     * Value of the asked dynamic property.
     */
    private String iPropertyValue;

    /**
     * Constructor of DynamicPropertyInfo class.
     */
    public DynamicPropertyInfo(String aPropertyName)
    {
        iPropertyName = aPropertyName;
        iIsPropertyValueStatic = false;
    }

    /**
     * Getter method for the name of the system property.
     * @return The name of the system property.
     */
    public String getPropertyName()
    {
        return iPropertyName;
    }

    /**
     * Setter method for the value of the system property.
     * @param aPropertyValue the value of the system property.
     */
    public void setPropertyValue(String aPropertyValue)
    {
        iPropertyValue = aPropertyValue;
    }

    /**
     * Getter method for the value of the system property.
     * @return The value of the system property.
     */
    public String getPropertyValue()
    {
        return iPropertyValue;
    }

    /**
     * Setter method for the info whether the dynamic property does
     * not change during the lifetime of the application.
     * @param aIsPropertyValueStatic boolean info.
     */
    public void setIsPropertyValueStatic(boolean aIsPropertyValueStatic)
    {
        iIsPropertyValueStatic = aIsPropertyValueStatic;
    }

    /**
     * Getter method for the info whether the dynamic property does
     * not change during the lifetime of the application.
     * @return Boolean info.
     */
    public boolean getIsPropertyValueStatic()
    {
        return iIsPropertyValueStatic;
    }
}
