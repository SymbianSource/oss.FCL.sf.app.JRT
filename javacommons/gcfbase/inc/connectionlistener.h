/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CONNECTIONLISTENER_H
#define CONNECTIONLISTENER_H

#include <string>

namespace java
{
namespace push
{
/**
 * Server Connection Plugin informs Push Controller about arrived message and error
 * via this interface.
 */

class ConnectionListener
{
public:

    virtual ~ConnectionListener() {}

    /**
     * This operation must be called when message arrives to the connection.
     * @param aInfo Argument to pass extra information from active connection
     *              to runtime. Currently only bluetooth uses this argument. Other
     *              protocols MUST NOT USED this argument.
     */
    virtual void msgArrived(const std::wstring& aInfo=L"") = 0;

    /**
     * This operation can be called if critical error occurs in the listening a push connection.
     * This operation does not throw an exception.
     * @param aUri      URI of the failed connection.
     * @param aErrCode  Error code defined by user.
     * @param aErrText  Error text defined by user.
     */
    virtual void error(const std::wstring& aUri,int aErrCode, const std::string& aErrText) = 0;

};


}//end namespace push
}//end namespace java

#endif // CONNECTIONLISTENER_H
