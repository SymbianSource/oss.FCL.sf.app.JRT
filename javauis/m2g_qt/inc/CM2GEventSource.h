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
* Description:  Event server framework's event source implementation
*
*/

#ifndef CM2GEVENTSOURCE_H
#define CM2GEVENTSOURCE_H

// INCLUDE FILES
#include <mevents.h> // CJavaEventSource, TConstructor
#include "M2GGeneral.h"

M2G_NS_START
// CONSTANTS

// DATA TYPES

// MACROS

// FORWARD DECLARATIONS

// FUNCTION PROTOTYPES

// CLASS DECLARATION
/**
 * @class CJavaM3GEventSource
 * @brief Event source
 */
class CM2GEventSource : public CJavaEventSource<CM2GEventSource>
{
public:
    /**
     * Two-phased constructor.
     * @since Series S60 3.1
     * @param aJni JNI environment
     * @param aPeer Peer object
     * @param aServer Server
     * @return Event source handle
     */
    static TInt NewL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer);

    /**
     * Dtor
     * @since Series S60 3.1
     */
    virtual ~CM2GEventSource();

private:
    /**
     * Ctor
     * @since Series S60 3.1
     */
    CM2GEventSource();

    /**
     * Constructor
     * @since Series S60 3.1
     * @param aJni JNI environment
     * @param aPeer Peer object
     * @param aServer Server
     */
    void ConstructL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer);

    /**
     * C++ copy constructor
     * @since Series S60 3.1
     */
    CM2GEventSource(const CM2GEventSource&);

    /**
     * C++ assignment operator
     * @since Series S60 3.1
     */
    CM2GEventSource& operator=(const CM2GEventSource&);

};

M2G_NS_END

#endif // CM2GEVENTSOURCE_H
