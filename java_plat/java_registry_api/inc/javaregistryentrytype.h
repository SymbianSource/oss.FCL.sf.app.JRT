/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java Registy API header file
*
*/


#ifndef JAVAREGISTRYENTRYTYPE_H
#define JAVAREGISTRYENTRYTYPE_H

namespace Java
{

/**
 * This enumeration lists Java Registry Entry types.
 */
enum TJavaRegistryEntryType
{
    // EGeneralPackage defines the starting value of the range including
    // the ids of entry types which are considered as package entries
    EGeneralPackage         = 100,
    EMidp2MidletSuite       = 101,
    EDeploymentPackage      = 102,    // OSGi Deployment Package
    EBundle                 = 103,
    EOsgiAwareMidletSuite   = 104,    // OSGi Aware Midlet Suite

    // EGeneralApplication defines the starting value of the range including
    // the ids of entry types which are considered as application entries
    EGeneralApplication     = 200,
    EMidp2Midlet            = 201,
    EOsgiService            = 202,
    EOsgiAwareMidlet        = 203,    // OSGi Aware Midlet Application
    EERCP                   = 204     // eRCP Application
};

}

#endif
