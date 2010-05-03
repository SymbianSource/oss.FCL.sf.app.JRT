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

#ifndef MMSSERVERCONNECTION_H
#define MMSSERVERCONNECTION_H

#include "serverconnection.h"

using namespace java::push;

namespace java
{
namespace wma
{
/**
 * The Os agnostic interface through which MMSServerConnectionImpl is used
 *
 */
class MmsServerConnection: public ServerConnection
{

public:

    static ServerConnection* getServerConnection(const std::wstring& aUri,
            const std::wstring& aFilter);

    virtual ~MmsServerConnection()
    {

    }

    virtual void setOpen() = 0;
};

}//end of namespace wma
}//end of namespace java
#endif // MMSSERVERCONNECTION_H

