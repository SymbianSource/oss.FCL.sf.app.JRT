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
* Description: A CoreUi avkon implementation.
*
*/

#ifndef COREUIAVKONIMPL_H
#define COREUIAVKONIMPL_H

#include "jni.h"

#include "javacoreui.h"
#include "javasymbianoslayer.h"

#include "coreuiavkonlcdui.h"
#include "coreuiavkoneswt.h"

#include "coreuiavkonlcduisupportimpl.h"
#include "coreuiavkoneswtsupportimpl.h"

/**
 * For accessing the static methods using dynamic dll loading.
 * @param funcName The name of the method to be searched.
 * @return a pointer to found function, 0 if not found.
 */
IMPORT_C FuncPtr findDllMethod(const char* funcName);

/**
 * For getting a sinleton instance of the CoreUi.
 */
java::ui::CoreUi& getUiInstance();

/**
 * Releases the ui. This means that the UI will be closed (if not done yet)
 * and freese all the resources.
 */
void releaseUi();

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * The implementation of the CoreUiAvkon. This class provides CoreUi services
 * to CoreUi users.
 */
NONSHARABLE_CLASS(CoreUiAvkonImpl) : public CoreUi, public CoreUiAvkonLcdui,
        public CoreUiAvkonEswt
{
public:
    /**
     * CoreUiAvkonImpl default constructor.
     */
    CoreUiAvkonImpl();

    /**
     * CoreUiAvkonImpl destructor.
     */
    virtual ~CoreUiAvkonImpl();

    /**
    * A static getter for the singleton CoreUiAvkonImpl instance.
    * @return A reference to CoreUiAvkonImpl instance.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    static CoreUiAvkonImpl& getInstanceImpl();

public: // From CoreUi
    /**
    * This method will ask the UI to start the main loop
    * @param appUid The Uid of the application.
    * @param uiParams Ui configuration parameters. If it
    *        is null, the default arguments will be used.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    virtual void start(const java::util::Uid& midletUid,
                       CoreUiParams* uiParams);

    // From CoreUiAvkon
    /**
     * Ensures that the UI has been created. After returning from this method
     * the user can be sure that the UI is available.
     * @throws std::exception on error cases.
     */
    virtual void ensureInitialized(const TUid& appUid, CoreUiParams* params = 0);

    /**
     * A getter for the CoreUiAvkonAppUi instance.
     * @return A pointer to CoreUiAvkonAppUi.
     */
    virtual CoreUiAvkonAppUi* getJavaUiAppUi() const;

    /**
     * A getter for the CAknAppUi instance.
     * @return A pointer to CAknAppUi.
     */
    virtual CAknAppUi* getJavaAknAppUi() const;

    /**
     * A getter for the CApaWindowGroupName instance.
     * @return A pointer to CApaWindowGroupName.
     */
    virtual CApaWindowGroupName* getWindowGroupName() const;

    /**
     * A request for disposing the UI. The UI will be disposed if
     * all the childs has been unregistered. If this leads to disposing
     * the UI it should be noted, that after returning from this method
     * there has been sent a request to UI thread, that it should shut down.
     * There might be a small gap until the UI is really closed. This method
     * will delete all the resources including the object itself.
     */
    virtual void dispose();

    /**
     * For informing the coreUI that a shutdown event from the window server
     * has been received. The coreUi informs the corresponding JavaRuntime
     * about the shutdown event an ensures that the application is eventually
     * closed.
     */
    virtual void shutDownRequestFromWindowServer();

    /**
     * Gets the heap of the process. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    virtual RHeap* getProcessHeap() const;

    /**
     * Gets the heap of the UI thread. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    virtual RHeap* getUiThreadHeap() const;


public: // From CoreUiAvkonLcdui
    /**
     * For getting the CoreUiAvkonLcduiSupport instance, which
     * holds all the necessary resources of the core LCDUI.
     * @return a reference to CoreUiAvkonLcduiSupport
     */
    virtual CoreUiAvkonLcduiSupport& getLcduiSupport();

public: // From CoreUiAvkonEwst
    /**
     * For getting the CoreUiAvkonEswtSupport instance, which
     * holds all the necessary resources of the core eSWT.
     * @return a reference to CoreUiAvkonEswtSupport
     */
    virtual CoreUiAvkonEswtSupport& getEswtSupport();

public:
    /**
     * Creates avkon UI environment into context of the calling
     * thread. It only initilaizes the environment, doesn't start
     * the scheduler.
     */
    void createUi();

    /**
     * Starts the active scheduler of the UI.
     */
    void startScheduler();

    /**
     * For getting the CoreUi construction parameter set by the application.
     * @return a reference to CoreUiParams.
     */
    const CoreUiParams& getCoreUiParams() const
    {
        return mCoreUiParams;
    }

    /**
     * Stores the JavaUiAvkonAppUi pointer. It is stored in order to provide
     * access to it for the toolkits.
     * @param appUi pointer to JavaUiAvkonAppUi object.
     */
    void setJavaUiAppUi(JavaUiAvkonAppUi* appUi);

    /**
     * Creates a JavaVm pointer for doing up calls in forceful shutdown.
     */
    bool setJavaVm(JNIEnv* env);

    /**
     * For asking should the start screen been created at all.
     */
    bool showStartScreen() const;

    TUid getMidletTUid() const
    {
        return mMidletTUid;
    }

    const java::util::Uid& getMidletUid() const
    {
        return mMidletUid;
    }

protected: //Members


private: //Methods
    /**
     * A private helper method for initializing the Avkon UI.
     */
    void initAvkonUiL();

    /**
     * A private helper method for setting the shutdown timer.
     */
    void setShutdownTimerL();

    /**
     * A private helper method for cancelling the shutdown timer.
     */
    void cancelShutdownTimer();

    /**
     * Call back for the timer in case where the timer elapses.
     */
    static TInt shutdownTimerCallback(TAny*);

    CoreUiAvkonImpl(const CoreUiAvkonImpl&);               //No copy constructor allowed
    CoreUiAvkonImpl& operator= (const CoreUiAvkonImpl&);   //No Assignment operator allowed

    /**
     * A private helper method for calling static java method.
     */
    bool callStaticVoidJavaMethod(const char* className,
                                  const char* methodName,
                                  const char* methodSignature,
                                  ...);

private: //Members
    enum State // codescanner::enummembers::enumnames
    {
        Constructed,
        CoreUiCreated,
        ShuttingDown,
        Destroyed
    };

    CEikonEnv*                          mJavaUiAvkonEnv; // ExecuteD wiil take the ownership
    CApaWindowGroupName*                mWgName;

    JavaUiAvkonAppUi*                   mJavaUiAppUi; // Not owned

    java::util::Uid                     mMidletUid;
    TUid                                mMidletTUid;
    CoreUiAvkonLcduiSupportImpl         mLcduiSupport;
    CoreUiAvkonEswtSupportImpl          mEswtSupport;
    State                               mState;
    bool                                mShowStartScreen;

    JavaVM*                             mJavaVm; // Not owned
    bool                                mShutDownPending;
    RFastLock                           mLock;
    CPeriodic*                          mShutdownTimer;
    CoreUiParams                        mCoreUiParams;
};


} //end namespace ui
} //end namespace java


#endif // COREUIAVKONIMPL_H
