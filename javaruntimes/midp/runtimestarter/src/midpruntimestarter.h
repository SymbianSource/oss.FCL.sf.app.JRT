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
* Description:  This class provides container for message.
*
*/


#ifndef MIDPRUNTIMESTARTER_H
#define MIDPRUNTIMESTARTER_H

#include <map>

#include "commslistener.h"
#include "commsmessage.h"

#include "scopedlocks.h"
#include "monitor.h"

#include "applicationinfo.h"
#include "applicationstatecontroller.h"
#include "midpruntimepushsupport.h"
#include "midpruntimeinternalsupport.h"
#include "jvmstarter.h"

#include "javastorage.h"
#include "javastorageentry.h"

#include "commsclientendpoint.h"

namespace java // codescanner::namespace
{
namespace util // codescanner::namespace
{
class DynamicLibLoader;
} // end namespace util

namespace runtime // codescanner::namespace
{

/**
 * This class is responsible for starting the MIDP runtime. It should be noted
 * that all the thread critical state changes must be done using thread safe
 * method doStateChange.
 */

OS_NONSHARABLE_CLASS(MidpRuntimeStarter) : public ApplicationInfo,
        public ApplicationStateController,
        public java::comms::CommsListener,
        public MidpStarterPushSupport,
        public MidpStarterInternalSupport
{
public:

    /**
     * Constructor of the MidpRuntimeStarter.
     */
    MidpRuntimeStarter();

    /**
     * Destructor of the MidpRuntimeStarter.
     */
    ~MidpRuntimeStarter(); // Not to be inherited, so can be non virtual.

    /**
     * Starts the MIDP runtime according to given arguments. Unknown argument
     * will lead to throwing java::runtime::RuntimeException.
     * @param argc Count of the arguments.
     * @param argv An array of arguments. The accepted arguments are in the
     *             midpruntimearguments.h file.
     * @return Status of the method call - 0 on success case.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     *
     */
    int start(int argc, char *argv[]);

    // From ApplicationStateController.

    /**
     * To be removed...
     */
    virtual void ApplicationStateChangeRequest(ApplicationState state);

    /**
     * Implementation of method startMidletRequestFromPush from interface
     * MidpStarterPushSupport
     * @see midpruntimepushsupport.h.
     */
    virtual void
    startMidletRequestFromPush(const std::wstring& pushAdditionalInfo);

    /**
     * Implementation of method closeRuntimeRequestFromPush from interface
     * MidpStarterPushSupport
     * @see midpruntimepushsupport.h.
     */
    virtual void closeRuntimeRequestFromPush();

    /**
     * Implementation of method closeRuntimeInd from interface
     * MidpStarterInternalSupport
     * @see midpruntimeinternalsupport.h.
     */
    virtual void closeRuntimeInd();

    /**
     * Implementation of method setUid from interface
     * MidpStarterInternalSupport
     * @see midpruntimeinternalsupport.h.
     */
    virtual void setUids(const java::util::Uid& midletUid,
                         const java::util::Uid& midletSuiteUid);

    /**
     * Implementation of method getRootPath from interface
     * ApplicationInfo
     * @see applicationinfo.h.
     */
    virtual const std::wstring& getRootPath() const;

    /**
     * Implementation of method getRootPath from interface
     * ApplicationInfo
     * @see applicationinfo.h.
     */
    virtual const java::util::Uid& getUid() const;

    /**
     * Implementation of method processMessage from interface
     * java::comms::CommsListener. This method will process all the
     * RTC messages received from the Java Captain.
     * @see commslistener.h
     */
    virtual void processMessage(java::comms::CommsMessage& aMessage);



private: // Definitions

    /**
     * States of the runtime starter.
     */
    enum State // codescanner::enummembers::enumnames
    {
        /**
         * The object is created and initializations have been started.
         */
        Constructed = 0x1,

        /**
         * The starter is in push listening state. The JVM has not been
         * started.
         */
        PushListen = 0x2,

        /**
         * The JVM is started and Java peer is active. This doesn't mean
         * that the MIDlet is running. The Java peer can be in pre warmed
         * state waiting for the start request from Java Captain.
         */
        Active      = 0x4,

        /**
         * The starter has received request to shut down the process.
         */
        Closing     = 0x8
    };

    /**
     * State change request to the runtime starter.
     */
    enum StateChangeRequest // codescanner::enummembers::enumnames
    {
        /**
         * Start the MIDlet
         */
        Start = 0x1,

        /**
         * Stop the MIDlet
         */
        Stop  = 0x2
    };

    /**
     * A utility class (struct) for containing MIDlet specific information.
     */
    OS_NONSHARABLE_CLASS(MidletInfo)
    {
public:
        /**
         * Constructor.
         */
        MidletInfo(): mPushStart(false),
                mPreWarmStart(false),
                mAutoInvocationRequested(false),
                mDebugRequested(false),
                mMIDletHasArgs(false),
                mBackGroundRequested(false) {}

public:
        /**
         * The UID of the MIDlet.
         */
        java::util::Uid mMIDletUid;

        /**
        * The UID of the MIDlet Suite.
        */
        java::util::Uid mMIDletSuiteUid;


        /**
         * The defined UI orientation of the MIDlet.
         */
        std::wstring mMidletSplashScreenAttribute;

        /**
          * The classpath of the MIDlet.
          */
        std::wstring    mClassPath;

        /**
         * The rootpath of the MIDlet.
         */
        std::wstring    mMIDletRootPath;

        /**
         * The recorded intial heap size of the MIDlet.
         */
        std::wstring    mMIDletHeapInitalSize;

        /**
         * Should start to push listen mode.
         */
        bool            mPushStart;

        /**
         * Should start to pre warm listen mode.
         */
        bool            mPreWarmStart;

        /**
         * Is the start result of auto invocation.
         */
        bool            mAutoInvocationRequested;

        /**
         * Should the debugging features be enabled.
         */
        bool            mDebugRequested;

        /**
         * Has the MIDlet been started with command line args
         */
        bool            mMIDletHasArgs;

        /**
         * Is the start such that request the UI to go to back ground.
         */
        bool            mBackGroundRequested;

        /**
         * The arguments of the MIDlet if any
         */
        std::wstring    mMIDletArgs;
    };

private: // Methods

    /**
     * A thread entry point for a thread that ensure that the runtime process
     * will be exited if something goes wrong in shutdown sequence.
     * @param ptr A pointer to MidpRuntimeStarter.
     */
    static void* ensureExit(void* ptr);

    /**
     * Parses the arguments provided by Java Captain when creating the
     * process. The argument interpretations are stored into a member variable
     * mMidletInfo for easy future access. Any Unknown argument
     * will lead to throwing java::runtime::RuntimeException.
     * @param argc Count of the arguments.
     * @param argv An array of arguments. The accepted arguments are in the
     *             midpruntimearguments.h file.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void parseArgs(int argc, char *argv[]);

    /**
     * Starts the core UI.
     * @param[out] coreUiLoader A dynamic lib loader for loading
     *                          dynamically the coreui lib.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void startCoreUi(std::auto_ptr<java::util::DynamicLibLoader>& coreUiLoader);

    /**
     * Initailizes comms connection to Java Captain. If it fails then
     * RuntimeException will be thrown.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void initComms();

    /**
     * Does the state change in the the MIDP runtime starter.
     * This method is thread safe.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void doStateChange(StateChangeRequest request);

    /**
     * Finds MIDlet suite UID form the JavaStorage.
     * @param storage Opened session to JavaStorage.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void getMIDletSuiteUidFromStorage(java::storage::JavaStorage& storage);

    /**
     * Finds the value of given key from APPLICATION_PACKAGE_TABLE in
     * Java Storage. In this method will call a new session to Java Storage
     * is alway created. At the end of method the session is closed. The user
     * can provide already opened storage connection. If that is not available
     * the method will create a temp connection that will be closed when
     * exiting from the method.
     * @param storage Opened session to JavaStorage.
     * @param key A key to APPLICATION_PACKAGE_TABLE. Use constants defined in
     *            javastoragenames.h.
     * @param value A reference to wstring where to store the result. If the
     *              is unknown then RuntimeException will be thrown.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void getMIDletSuiteInfoFromStorage(java::storage::JavaStorage* storage,
                                       const std::wstring& key,
                                       std::wstring& value) const;

    /**
     * Finds the value of given key from APPLICATION_PACKAGE_ATTRIBUTES_TABLE
     * in Java Storage. This table contains all the MIDlet jad and jar manifest
     * arguments
     * @param storage Opened session to JavaStorage.
     * @param searchKey The key of the attribute to search.
     * @return A pointer to value. The user is responsible for deleting the
     *         pointer. 0 if value can't be found.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    std::wstring*
    getMidletAttributeFromStorage(java::storage::JavaStorage& storage,
                                  const std::wstring& searchKey) const;

    /**
     * Utility for finding the value from the JavaStorageApplicationEntry_t
     * received from Java Storage.
     * @param entry A list of certain teble content.
     * @param key A key to table. Use constants defined in javastoragenames.h.
     * @return The value as wstring. If the key is unknown then
     *         RuntimeException will be thrown.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    std::wstring
    getDbValue(const java::storage::JavaStorageApplicationEntry_t& entry,
               const std::wstring& key) const;

    /**
     * Starts to listen push connection. The actual listenin is done by the
     * Push framework which is initialized by this method.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void handlePushStart();

    /**
     * Delivers a close message to push framework in order to ensure proper
     * closing of certain push implementations.
     * @throws std::exception, java::runtime::RuntimeException or
     *         java::util::ExceptionBase on error cases.
     */
    void closePush();

    /**
     * If the default heap size has been recorded into file named as 'heap'
     * in the root of MIDlet's root path, then this method will return
     * the stored size in B.
     * return the default heap size in kB if available, otherwise -1.
     */
    int getHeapSize() const;

    /**
     * No copy constructor allowed
     */
    MidpRuntimeStarter(const MidpRuntimeStarter& starter);

    /**
     * No copy constructor allowed
     */
    MidpRuntimeStarter& operator= (const MidpRuntimeStarter& starter);

    /**
     * Encode string to a private format that is safe to pass to Java side
     * as a Java system property.
     * Java side function com.nokia.mj.impl.rt.midp.MidletLifeCycle::decodeArgs
     * is used to decode the string
     *
     * @param args original wstring
     * @return encoded wstring
     */
    std::wstring encodeArgs(const std::wstring& args);

private: //Members
    /**
     * Comms client endpoint towards Java Captain.
     */
    std::auto_ptr<java::comms::CommsClientEndpoint> mComms;

    /**
     * For caching important MIDlet specific data.
     */
    std::auto_ptr<MidletInfo>                       mMidletInfo;

    /**
     * Utility for making methods thread safe.
     */
    java::util::ScopedMutex                         mProcessesMutex;

    /**
     * The state of the MIDP starter.
     */
    int                                             mRuntimeState;

    /**
     * A lock for push wait case.
     */
    std::auto_ptr<java::util::Monitor>              mMonitor;

    /**
     * A lock for exit case.
     */
    std::auto_ptr<java::util::Monitor>              mExitMonitor;

    /**
     * A library loader for push case.
     */
    std::auto_ptr<java::util::DynamicLibLoader>     mPushLib;

    /**
     * Utility for making methods thread safe.
     */
    std::wstring                                    mPushAdditionalInfo;

    /**
     * Did the shutdown go ok.
     */
     bool                                           mShudownOk;

};

} // end namespace runtime
} // end namespace java

#endif // MIDPRUNTIMESTARTER_H
