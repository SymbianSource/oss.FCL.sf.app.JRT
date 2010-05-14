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


#include "pushserverconnpluginmanager.h"
#include "logger.h"
#include "applicationstatecontroller.h"
#include "serverconnectionfactory.h"
#include "serverconnection.h"
#include "pusherrorcodes.h"
#include "pushexception.h"
#include "pushconstant.h"

using namespace java::push;
using namespace java::util;
using namespace java::runtime;

/**
 *
 */
PushServerConnPluginManager::PushServerConnPluginManager
(ConnectionListener* aListener,PendingConnectionListener* aPendingConnListener,
 ConnManagedInterface* aConnManagedInterface)
        : mConnListener(aListener),mPendingConnListener(aPendingConnListener),
        mConnManagedInterface(aConnManagedInterface)
{
    JELOG2(EJavaPush);
}

/**
 *
 */
PushServerConnPluginManager::~PushServerConnPluginManager()
{
    JELOG2(EJavaPush);
    try
    {
        for (SrvConnContainerIter_t iter = mSrvConnContainer.begin(); iter != mSrvConnContainer.end(); ++iter)
        {
            iter->second.mServerConnFactory.closeAllConnections();
            delete(iter->second.mSrvConnLibAccess);
            iter->second.mSrvConnLibAccess = 0;
        }
        mSrvConnContainer.clear();
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! ~PushServerConnPluginManager(): Unexpected exception was caught");
    }
}

/**
 *
 */
void PushServerConnPluginManager::startListen(UriList_t& aUriList,bool aThrowException,
        bool aCallConnectionHandled)
{
    JELOG2(EJavaPush);

    if (0 == aUriList.size())
        return;

    for (UriListIter_t iter = aUriList.begin(); iter != aUriList.end(); ++iter)
    {
        try
        {
            LOG1WSTR(EJavaPush,EInfo, "startListen() URI: %s",iter->mUri);
            std::wstring schemeStr;
            loadAndStoreDll(iter->mUri,schemeStr);
            createAndOpenConn((*iter),schemeStr);
            if (true == aCallConnectionHandled)
                mConnManagedInterface->connCompleted(iter->mUri);
        }
        catch (ExceptionBase& ex)
        {
            //std::exception is caught here to get a debug print.
            ELOG1(EJavaPush,"ERROR!!! PushServerConnPluginManager::startListen(): %s",
                  ex.toString().c_str());
            if (true == aThrowException)
                throw;
        }
        catch (...)
        {
            //Exception is not thrown from this situation. It is better just ignore unsupported
            //URI.
            ELOG(EJavaPush,
                 "ERROR!!! PushServerConnPluginManager::startListen() Cannot open plugin for push URI");
            if (true == aThrowException)
                throw;
        }
    }//end for

    if (0 == getNumOfSuccessfulPushConns())
    {
        throw PushException(INIT_OF_ALL_PUSH_CONNS_FAILED,"Creation of all push connections failed",
                            __FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushServerConnPluginManager::updatePushRegs(UriList_t& aUriList,
        PushControllerErrorHandlerInterface& aErrorHandler)
{
    JELOG2(EJavaPush);

    //First is retrieved current connections. This is needed to find out whether
    //push connection was registered or unregistered.
    std::list<std::wstring> existingUriList;
    listConnections(false,existingUriList);

    //We can stop execution of this operation if size of both list is equal.
    //This should not never happened.
    if (aUriList.size() == existingUriList.size())
        return;

    if (aUriList.size() < existingUriList.size())
    {
        //In this case one push req has been removed from other MIDlet.
        findAndDeleteUnregPushConns(existingUriList,aUriList,aErrorHandler);
    }
    else if (aUriList.size() > existingUriList.size())
    {
        //In this case push req has been added to the MIDlet from the other MIDlet.
        findAndAddNewPushConns(existingUriList,aUriList);
    }
}

/**
 *
 */
void PushServerConnPluginManager::listConnections(bool aAvailable,
        std::list<std::wstring>& aUriList)
{
    JELOG2(EJavaPush);
    for (SrvConnContainerIter_t iter = mSrvConnContainer.begin(); iter != mSrvConnContainer.end(); ++iter)
    {
        iter->second.mServerConnFactory.getPushConnections(aAvailable,aUriList);
    }//end for
}

/**
 *
 */
ServerConnection& PushServerConnPluginManager::getPushConnection(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    std::wstring schemeStr = resolveScheme(aUri);
    ServerConnectionFactory* srvConnFac = getDllFromContainer(schemeStr);
    if (0 == srvConnFac)
    {
        std::string errTxt("Server Connection Plugin by URI is not found");
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(SRV_CONN_NOT_FOUND,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    ServerConnection* srvConn = srvConnFac->getPushConnection(aUri);
    if (0 == srvConn)
    {
        std::string errTxt("ServerConnection object by URI is not found");
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(SRV_CONN_NOT_FOUND,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    return *srvConn;
}

/**
 *
 */
void PushServerConnPluginManager::unregisterDynamicPushConn(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    std::wstring schemeStr = resolveScheme(aUri);
    ServerConnectionFactory* srvConnFac = getDllFromContainer(schemeStr);
    if (0 != srvConnFac)
        srvConnFac->deletePushConnection(aUri);
    //Unregisterd connection must be removed from "is connection opened successfully"
    //list.
    mSuccessPushConnList.erase(aUri);
}

/**
 *
 */
int PushServerConnPluginManager::setConnFailed(const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    SuccessPushConnListIter_t iter = mSuccessPushConnList.find(aUri);
    if (iter != mSuccessPushConnList.end())
    {
        iter->second = false;
    }
    return getNumOfSuccessfulPushConns();
}

/**
 *
 */
void PushServerConnPluginManager::deletePushServerConn(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    try
    {
        std::wstring schemeStr = resolveScheme(aUri);
        ServerConnectionFactory* srvConnFac = getDllFromContainer(schemeStr);
        if (0 != srvConnFac)
            srvConnFac->deletePushConnection(aUri);
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! deletePushServerConn() Unexpected exception was caught");
    }
}

/**
 *
 */
bool PushServerConnPluginManager::isValidPushUri(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    ServerConnection* srvConn = 0;
    std::wstring schemeStr;
    try
    {
        ServerConnectionFactory& srvConnFac = loadAndStoreDll(aUri,schemeStr);

        //This checks that push connetion is not reserved by this MIDlet.
        srvConn = srvConnFac.getPushConnection(aUri);
        if (0 != srvConn)
            return false;

        //If creation of the server connection succeeds then URI is unique.
        //Connection is closed immediately.
        srvConn = srvConnFac.create(aUri);
        srvConn->open(mConnListener);
        srvConn->close();
        srvConnFac.releaseConnection(aUri);
        return true;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! checkUniquenessOfUri() Exception was caught");
        try
        {
            if (0 != srvConn)
            {
                srvConn->close();
                ServerConnectionFactory& srvConnFac = loadAndStoreDll(aUri,schemeStr);
                srvConnFac.releaseConnection(aUri);
            }
        }
        catch (...) {}
        return false;
    }
}

/**
 *
 */
ServerConnectionFactory* PushServerConnPluginManager::getDllFromContainer(const std::wstring& aScheme)
{
    JELOG2(EJavaPush);

    SrvConnContainerIter_t iter = mSrvConnContainer.find(aScheme);
    if (iter != mSrvConnContainer.end())
        return &(iter->second.mServerConnFactory);
    return 0;
}

/**
 *
 */
ServerConnectionFactory& PushServerConnPluginManager::loadAndStoreDll(const std::wstring& aUri,
        std::wstring& aOutScheme)
{
    JELOG2(EJavaPush);
    aOutScheme = resolveScheme(aUri);
    ServerConnectionFactory* srvConnFac = getDllFromContainer(aOutScheme);
    if (0 != srvConnFac)
        return *srvConnFac;

    std::string dllName = resolveDllName(aOutScheme);
    std::auto_ptr<java::util::DynamicLibLoader> tmpLoader(0);
    ServerConnectionFactory& conn =
        ServerConnectionFactory::getServerConnectionFactory(dllName,tmpLoader);

    ServerConnectionPluginData pluginData(tmpLoader.release(),conn);
    std::pair<std::wstring,ServerConnectionPluginData> pairObj(aOutScheme,pluginData);
    mSrvConnContainer.insert(pairObj);
    return conn;
}

/**
 *
 */
void PushServerConnPluginManager::createAndOpenConn(const PushRegData& aData,
        const std::wstring& aScheme)
{
    JELOG2(EJavaPush);

    SrvConnContainerIter_t connIter;
    try
    {
        connIter = mSrvConnContainer.find(aScheme);
        if (mSrvConnContainer.end() == connIter)
            return;

        connIter->second.mServerConnFactory.createPushServerConn
        (aData.mUri,aData.mFilter,mConnListener,mPendingConnListener);

        ScopedLock lockObj(mMutex);
        mSuccessPushConnList.insert(std::pair<std::wstring,bool>(aData.mUri,true));
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! PushServerConnPluginManager::createAndOpenConn()");
        throw;
    }
}

/**
 *
 */
std::wstring PushServerConnPluginManager::resolveScheme(const std::wstring& aUri) const
{
    JELOG2(EJavaPush);

    std::wstring::size_type schemeIndex = aUri.find(L":");
    if (std::string::npos == schemeIndex)
    {
        std::string errTxt("URI does not contain scheme");
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(INVALID_URI,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    std::wstring schemeStr = aUri.substr(0,schemeIndex);
    if (L"secure-element" == schemeStr)
        return L"secureelement";
    return schemeStr;
}

/**
 *
 */
std::string PushServerConnPluginManager::resolveDllName(const std::wstring& aScheme) const
{
    JELOG2(EJavaPush);
    std::string schemeStr(aScheme.begin(),aScheme.end());
    std::string dllNameStr(PREFIX_OF_SRV_CONN_PLUGIN);
    dllNameStr.append(schemeStr);
    dllNameStr.append(SUFFIX_OF_SRV_CONN_PLUGIN);
    ILOG1(EJavaPush,"Name of the loaded dll: %s",dllNameStr.c_str());
    return dllNameStr;
}

/**
 *
 */
int PushServerConnPluginManager::getNumOfSuccessfulPushConns()
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);

    int successConnections = 0;
    for (SuccessPushConnListIter_t iter = mSuccessPushConnList.begin();
            iter != mSuccessPushConnList.end(); ++iter)
    {
        if (true == iter->second)
            successConnections++;
    }
    return successConnections;
}

/**
 *
 */
void PushServerConnPluginManager::findAndDeleteUnregPushConns
(std::list<std::wstring>& aExistingUriList,UriList_t& aNewUriListFromStorage,
 PushControllerErrorHandlerInterface& aErrorHandler)
{
    JELOG2(EJavaPush);

    //First is removed all valid push connections(=not push regs which will be unregistered)
    //from the 'existingUriList' list.
    //After the loop 'aExistingUriList' list contains connections which will be removed.
    for (UriListIter_t iter = aNewUriListFromStorage.begin();
            iter != aNewUriListFromStorage.end(); ++iter)
    {
        aExistingUriList.remove(iter->mUri);
    }//end for

    //Unregistering connections. Actually this list should contains only one item because
    //dynamic push regs are unregistered one by one from PushRegstry class.
    for (std::list<std::wstring>::iterator iter2 = aExistingUriList.begin();
            iter2 != aExistingUriList.end(); ++iter2)
    {
        unregisterDynamicPushConn((*iter2));
        mConnManagedInterface->connCompleted((*iter2));
    }//end for

    //Next is checked whether this MIDlet has any push connections. Below
    //is also checked whether listening of all push connections are failed.
    std::list<std::wstring> existingPushConns;
    listConnections(false,existingPushConns);
    if (0 < existingPushConns.size())
    {
        //MIDlet has push connections. Next is checked whether opening
        //of all MIDlets has been done successfully.
        if (0 == getNumOfSuccessfulPushConns())
        {
            //Initialization of all connections has been failed.
            aErrorHandler.sendAllConnsFailedMsg();
            //No active connections so we can inform runtime to close a process.
            aErrorHandler.closeRuntime();
        }
    }
    else if (0 == existingPushConns.size())
    {
        //No connections so runtime can be informed to close an application.
        aErrorHandler.closeRuntime();
    }
}

/**
 *
 */
void PushServerConnPluginManager::findAndAddNewPushConns
(std::list<std::wstring>& aExistingUriList,UriList_t& aNewUriListFromStorage)
{
    JELOG2(EJavaPush);

    //First is removed all push connections from 'aNewUriListFromStorage' list which
    //already exists in the push plugins.
    for (std::list<std::wstring>::iterator iter = aExistingUriList.begin();
            iter != aExistingUriList.end(); ++iter)
    {
        PushRegData tmpObj((*iter),L"*");
        aNewUriListFromStorage.remove(tmpObj);
    }//end for

    //Now 'aNewUriListFromStorage' list contains only new dynamic push regs.
    startListen(aNewUriListFromStorage,false,true);
}

