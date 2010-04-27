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


#ifndef JAVACOMMCONNECTION_H
#define JAVACOMMCONNECTION_H

#include "functionserver.h"

typedef std::pair<std::string, std::string> TPair;

namespace java
{

namespace comm
{
class NativeCommConnection;

class JavaCommConnection: public java::util::FunctionServer
{

public:

    /**
     * Constructor
     */

    JavaCommConnection();

    /**
     * Destructor
     */

    ~JavaCommConnection();

    void createConnection(int aHandle);

    NativeCommConnection* getNativeObject()
    {
        return iNativeObj;
    }

    NativeCommConnection *iNativeObj;


};

} // end comm namespace

} // java namespace

#endif // JAVACOMMCONNECTION_H

