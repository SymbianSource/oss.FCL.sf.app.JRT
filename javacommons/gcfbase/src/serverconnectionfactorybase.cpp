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

#include <memory>
#include <iostream>
#include <sstream>

#include "logger.h"

#include "serverconnectionfactorybase.h"
#include "runtimeexception.h"
#include "javacommonutils.h"
#include "pusherrorcodes.h"
#include "pushexception.h"
#include "pendingconnectionlistener.h"

using namespace java::push;
using namespace java::runtime;
using namespace java::util;

/**
 *
 */
OS_EXPORT ServerConnectionFactoryBase::ServerConnectionFactoryBase()
{
}

/**
 *
 */
OS_EXPORT ServerConnectionFactoryBase::~ServerConnectionFactoryBase()
{
    JELOG2(EJavaPush);
    try
    {
        for (SrvConnContainerIter_t iter = mServerConnections.begin();
                iter != mServerConnections.end(); ++iter)
        {
            delete(iter->second.getConn());
        }
        mServerConnections.clear();
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! ~ServerConnectionFactoryBase(): Unexpected exception was caught");
    }
}

/**
 *
 */
OS_EXPORT ServerConnection&
ServerConnectionFactoryBase::createPushServerConn(const std::wstring& aUri,
        const std::wstring& aFilter,
        ConnectionListener* aListener,
        PendingConnectionListener* aPendingConnListener)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter != mServerConnections.end())
    {
        if ((SrvConnContainerData::PUSH == iter->second.connType())
                || (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()))
        {
            std::string errTxt("Object already exists");
            ELOG1(EJavaPush, "ERROR: %s",errTxt.c_str());
            ELOG1WSTR(EJavaPush,"Error in createPushServerConn() URI: %s",aUri);
            throw PushException(PUSH_CONNECTION_ALREADY_EXISTS,errTxt,__FILE__,__FUNCTION__,__LINE__);
        }
        else
        {
            //Connection type is NORMAL so we have to change it to PUSH_LISTEN_BY_MIDLET.
            iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
            iter->second.setPendingConnListener(aPendingConnListener);
            iter->second.getConn()->setFilter(aFilter);
            return *(iter->second.getConn());
        }
    }
    std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri,aFilter));
    newSrvConn->open(aListener);
    SrvConnContainerData newDataObj(newSrvConn.release(),aPendingConnListener,
                                    false,SrvConnContainerData::PUSH);
    mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>(aUri,newDataObj));

    return *(newDataObj.getConn());
}

/**
 *
 */
OS_EXPORT ServerConnection* ServerConnectionFactoryBase::create(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
    {
        std::auto_ptr<ServerConnection> newSrvConn(createSrvConnObj(aUri,L""));
        SrvConnContainerData newDataObj(newSrvConn.release(),0,false,SrvConnContainerData::NORMAL);
        mServerConnections.insert(std::pair<std::wstring,SrvConnContainerData>(aUri,newDataObj));
        return newDataObj.getConn();
    }

    if (!isMultipleSrvConnAllowed())
    {
        if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()
                || SrvConnContainerData::NORMAL == iter->second.connType())
        {
            std::string errTxt("Type of the connection is: ");
            errTxt.append(java::util::JavaCommonUtils::intToString((int)iter->second.connType()));
            throw PushException(PUSH_CONNECTION_ALREADY_EXISTS,errTxt,__FILE__,__FUNCTION__,__LINE__);
        }
    }

    if (SrvConnContainerData::PUSH == iter->second.connType())
    {
        iter->second.setConnType(SrvConnContainerData::PUSH_LISTEN_BY_MIDLET);
    }

    return (*iter).second.getConn();
}

/**
 *
 */
OS_EXPORT void
ServerConnectionFactoryBase::getPushConnections(bool aAvailable,std::list<std::wstring>& aUriList)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    for (SrvConnContainerIter_t iter = mServerConnections.begin(); iter != mServerConnections.end(); ++iter)
    {
        if ((SrvConnContainerData::PUSH == iter->second.connType())
                || (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType()))
        {
            if (true == aAvailable)
            {
                if (true == iter->second.isActive())
                {
                    aUriList.push_back((iter->first));
                }
            }
            else
            {
                WLOG1WSTR(EJavaPush,"ServerConnectionFactoryBase::getPushConnections() URI: %s",(iter->first));
                aUriList.push_back((iter->first));
            }
        }//end if( (SrvConnContainerData::PUSH == iter->second.connType())...)
    }//end for
}

/**
 *
 */
OS_EXPORT void ServerConnectionFactoryBase::closeAllConnections()
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    for (SrvConnContainerIter_t iter = mServerConnections.begin(); iter != mServerConnections.end();)
    {
        if (false == deletePushConnection(iter))
            iter++;
    }//end for
}

/**
 *
 */
OS_EXPORT void ServerConnectionFactoryBase::deletePushConnection(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
        return;
    deletePushConnection(iter);
}

/**
 *
 */
bool ServerConnectionFactoryBase::deletePushConnection(SrvConnContainerIter_t& aIter)
{
    JELOG2(EJavaPush);

    if (SrvConnContainerData::PUSH == aIter->second.connType())
    {
        aIter->second.getConn()->close();
        delete aIter->second.getConn();
        mServerConnections.erase(aIter++);
        return true;
    }
    else if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == aIter->second.connType())
    {
        //MIDlet listens connection so we cannot delete it here.
        aIter->second.setConnType(SrvConnContainerData::NORMAL);
    }
    //This method does nothing for non-push server connection. Non-push
    //connections must be release by releaseConnection operation.
    return false;
}

/**
 *
 */
OS_EXPORT ServerConnection* ServerConnectionFactoryBase::getPushConnection(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
        return 0;
    if (SrvConnContainerData::NORMAL == iter->second.connType())
        return 0;
    return iter->second.getConn();
}

/**
 *
 */
OS_EXPORT void ServerConnectionFactoryBase::releaseConnection(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter == mServerConnections.end())
        return;

    if (SrvConnContainerData::NORMAL == iter->second.connType())
    {
        iter->second.getConn()->close();
        delete iter->second.getConn();
        mServerConnections.erase(iter);
        return;
    }
    else if (SrvConnContainerData::PUSH_LISTEN_BY_MIDLET == iter->second.connType())
    {
        iter->second.getConn()->close();
        iter->second.setConnType(SrvConnContainerData::PUSH);
        return;
    }
    //This operation does nothing for connection which type is PUSH.
}

/**
 *
 */
OS_EXPORT void ServerConnectionFactoryBase::setPendingMsgFlag
(const std::wstring& aUri,bool aPendingMsg)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    //LOG1WSTR(EJavaPush,EInfo,"setPendingMsgFlag() URI: %s",aUri);
    //LOG1(EJavaPush,EInfo,"setPendingMsgFlag() aPendingMsg: %d",(int)aPendingMsg);

    SrvConnContainerIter_t iter = mServerConnections.find(aUri);
    if (iter != mServerConnections.end())
    {
        iter->second.setActive(aPendingMsg);
        PendingConnectionListener* tmpListener = iter->second.getPendingConnListener();
        if (0 != tmpListener)
            tmpListener->setPendingStatus(aUri,aPendingMsg);
    }
}

