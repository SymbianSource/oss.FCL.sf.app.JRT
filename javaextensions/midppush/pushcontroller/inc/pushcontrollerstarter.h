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


#ifndef PUSHCONTROLLERSTARTER_H
#define PUSHCONTROLLERSTARTER_H

#include <memory>
#include "dynamiclibloader.h"

namespace java
{
namespace util
{
class Uid;
}
namespace runtime
{
class MidpStarterPushSupport;
}
namespace comms
{
class CommsClientEndpoint;
}
namespace push
{

/**
 * Runtime Starter uses this interface to start and stop Push Controller.
 */

class PushControllerStarter;

typedef PushControllerStarter& (*GetPushStarter)();

class PushControllerStarter
{
public:
    virtual ~PushControllerStarter() {}

    /**
     * Reads push connections from the Java Storage, loads Server Connection Plugins
     * and start to listen push connections.
     * @param aUid UID of the application.
     * @param aAppStateController PushController informs application state changes
     *        via this object. Note: Ownership of this object is not given to the user.
     * @throws  ExceptionBase exception in the following situation:
     *          - Initialization of all push connections failed.
     */
    virtual void startListen(const java::util::Uid& aUid,
                             java::runtime::MidpStarterPushSupport* aAppStateController) = 0;

    /**
     * Updates push registrations of running application. This must be called when dynamic push
     * connection has been registered/unregistered from the other MIDlet. This operation does not
     * throw exceptions.
     * @param aUid UID of the application.
           * @param aAppStateController PushController informs application state changes
           *        via this object. Note: Ownership of this object is not given to the user.
     */
    virtual void updatePushRegs(const java::util::Uid& aUid,
                                java::runtime::MidpStarterPushSupport* aAppStateController) = 0;

    /**
     * Closes all connections (push and normal server connections)
     * and unloads Server Connection Plugins. This operation does not throw exceptions.
     * Note: It is not allowed to use PushControllerStarter object after calling this
     *       operation.
     */
    virtual void close() = 0;

    /**
    * This inline method will load the shared library which contains the
    * implementation of the starting the MIDP push controller.
    *
    * It is important that the user of this method is aware that the
    * implementation is available ONLY when the instance of
    * DynamicLibLoader is not deleted. When the user is absolutely sure
    * that no code is run from the loaded shared library, user should delete
    * the instance of DynamicLibLoader. The deletion will lead to
    * unloading the shared library.
    *
    * @param[out] loader User must store this argument as long as it needs
    *                    to run any code in the shared library. No other
                    APIs of the loader is needed to use.
    * @param[in] runtimeStarter A reference to runtimeStarter. To be removed.
    * @return Instance implementing PushControllerFactory in success case, 0
         in error cases.
    */
    static PushControllerStarter&
    getPushControllerStarter(std::auto_ptr<java::util::DynamicLibLoader>& loader);

};

inline PushControllerStarter&
PushControllerStarter::getPushControllerStarter(
    std::auto_ptr<java::util::DynamicLibLoader>& loader)
{

    //Create an instance of DynamicLibLoader, load the javajvmstarter
    //shared library and locate method createVmStarter. If loadAndGetFunction
    //succeeds were are certain that loader will contain valid object
    //and also functionPtr points to valid method.
    if (0 == loader.get())
    {
        loader.reset(new java::util::DynamicLibLoader("javapushcontroller"));
    }
    //Cast the function ptr
    //reinterpret_cast<GetPushStarter> would be better, but can't be used in Maemo
    GetPushStarter getPushStarter = (GetPushStarter)
                                    (loader->getFunction("getPushControllerStarter"));

    //Call the method which will return and instace of VmStarterInterface.
    return getPushStarter();
}

}//end namespace push
}//end namespace java

#endif // PUSHCONTROLLERSTARTER_H

