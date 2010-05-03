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
* Description:  ?Description
 *
*/

#ifndef NATIVEDYNAMICPROPERTY_H
#define NATIVEDYNAMICPROPERTY_H

#include "logger.h"
#include <jni.h>

namespace java
{

/**
 * This class is used to implement the system property functionality for commconnection.
 * The DynamicPropertyHandler java class, calls the method of this class to get a list of all
 * available comm ports.
 */

class NativeDynamicProperty
{
public:

    /**
     * getavailCommPort is called by DynamicPropertyHandler java class to get the list of avail port.
     *
     * Example return value: "COM1,COM2,IR1,USB1,USB2,BT1,BT2.."
     * @return the list of comma separated logical ports.
     */
    jstring getavailCommPort(JNIEnv& aJNi);

};

} // end namespace
#endif // NATIVEDYNAMICPROPERTY_H
