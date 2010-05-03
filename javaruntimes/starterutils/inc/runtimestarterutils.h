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
* Description:  Utilities for starting the Java runtime.
*
*/


#ifndef RUNTIMESTARTERUTILS_H
#define RUNTIMESTARTERUTILS_H

#include <string>
#include "javaoslayer.h"
#include "jvmstarter.h"

namespace java // codescanner::namespace
{

namespace comms // codescanner::namespace
{
class CommsClientEndpoint;
}

namespace runtime // codescanner::namespace
{

class OsThreadSupervisor;
class DevelopmentFeaturesListener;

/**
 * This class is responsible for providing utilies related to starting
 * the javaruntime.
 */
OS_NONSHARABLE_CLASS(RuntimeStarterUtils)
{
public:

    /**
     * Constructor of the RuntimeStarterUtils.
     */
    OS_IMPORT RuntimeStarterUtils();

    /**
     * Destructor of the RuntimeStarterUtils. Will destroy the instance of
     * comms client endpoint if has been created by this object. The class
     * is not meant to be inherited, so destructor can be non virtual.
     */
    OS_IMPORT ~RuntimeStarterUtils();

    /**
     * Creates a platform dependent thread supervisor for detecting thread
     * panics. When the thread supervisioning is enabled, any panicing thread
     * belonging to this process will lead to termination of the process.
     * At least in Symbian it is important to enable this feature. Panic of
     * one thread doesn't automatically lead to termination of the whole
     * process.
     * <p>
     * The lifetime of the supervisor is tight to existence of instance of
     * this class. Once the object is destroyed, alos the supervisor is
     * terminated.
     * @param tryThreadDumping Whether to try to do thread dump in panic case.
     *                         Might cause another panic.
     * @throws std::exception on error cases.
     */
    OS_IMPORT void startThreadSupervisor(bool tryThreadDumping = false);

    /**
     * This method will enable various kind of development features - like
     * debugging, profiling and thread dumping. At the moment all these
     * development features are to some extent controlled by Java Captain.
     * This is natural to MIDP runtime but a little problematic to non-managed
     * runtimes like Installer and TCK runner. Captain is not aware of these
     * non-managed runtimes.
     * <p>
     * This method will make a comms connection to Java Captain if it doesn't.
     * exist. For determining the existence it will search if a named comms
     * client endpoint is already created with name "javacaptain". If it
     * doesn't, then a new one is created. The endpoint is used when
     * generating thread dumps. Java Captain will deliver dump message to
     * all the running Java process that it is aware of.
     * <p>
     * Internally this method will ask from Java Captain if there are some
     * extra arguments to be provided to the JVM (if onlyThreadDump is set
     * to false). Those parameters can be
     * related e.g to debugging, profiling or something else.
     * <p>
     * The method will add a new lister to listen to thread dump messages from
     * Java Captain.
     * <p>
     * Note! If this method needs to create a new instance of comms client
     * endpoint, the exitence of endpoint is tight to existence of instance
     * of RuntimeUtils object. I.e. the endpoint is destroyd when the
     * destructor is called.
     * @param jvmStarter. User must provide a valid reference to JvmStarter.
     *                    This method will append certain JVM arguments
     *                    if some development features are set on. The
     *                    same JvmStarter must be used for starting the JVM.
     * @param askForArgs. If set to false, only thread dumping listening
     *                    is enabled and no message is sent to Captain.
     * @param applicationArgs. If askForArgs is true, this paramater must
     *                         point to a wstring. The arguments for the
     *                         application to be started will be returned in
     *                         the wstring.
     */
    OS_IMPORT void enableDevelopmentFeatures(JvmStarter& jvmStarter,
            bool askForArgs = false,
            std::wstring *applicationArgs = NULL);
    /**
     * Finds the add-on JSRs from platform dependent repository.
     * @param extendedBootClassPath a refernce to wstring where the method can store
     *                              the boot classpath. If the there are no add-on JSRs,
     *                              this will be empty wstring.
     */
    OS_IMPORT void getExtBootClassPath(std::wstring& extendedBootClassPath);

private: // Classes

private: // Methods.

    /*
     * No copy constructor allowed.
     */
    RuntimeStarterUtils(const RuntimeStarterUtils&);

    /*
     * No Assignment operator allowed.
     */
    RuntimeStarterUtils& operator= (const RuntimeStarterUtils&);

    void addRuntimeArguments(const std::wstring& aRuntimeArgs, JvmStarter& jvmStarter);

private: // Member variables.
    java::comms::CommsClientEndpoint* mComms;
    OsThreadSupervisor*               mSupervisor;
    DevelopmentFeaturesListener*      mListener;
};

} // end namespace runtime
} // end namespace java


#endif // RUNTIMESTARTERUTILS_H
