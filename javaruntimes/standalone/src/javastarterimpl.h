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

#ifndef JAVASTARTERIMPL_H
#define JAVASTARTERIMPL_H

#include <memory>

#include "midpruntimeinternalsupport.h"
#include "monitor.h"

namespace java // codescanner::namespace
{

OS_NONSHARABLE_CLASS(JavaStarterImpl) :
        public java::runtime::MidpStarterInternalSupport
{
public:
    /**
     * Constructor of the JavaStarterImpl.
     */
    JavaStarterImpl(const std::list<std::wstring>& args);

    /**
     * Destructor of the JavaStarterImpl.
     */
    ~JavaStarterImpl();

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
    virtual void setUids(const java::util::Uid& uid,
                         const java::util::Uid& suiteUid);

    int start(int argc, const char** argv);
    int start();

    static void getArgsFromFile(const char* file,
                                std::list<std::wstring>& args);

private:
    static void* ensureExit(void* ptr);

    void parseFlags();
    void setConfigration(const std::wstring& conf);
    void setProfile(const std::wstring& profile);
    void setOrientation(const std::wstring& orientation);
    void setStartScreen(const std::wstring& startscreen);
    void createJvmStarter();
    void handleJadJarCpArgs();
    void setUidAndRootPath();
    void addHandle();
    void addApplicationArgs();
    void storeNextArgument(std::wstring& arg);
    std::wstring getArgValue(const std::wstring& arg);

    /**
     * No default constructor.
     */
    JavaStarterImpl();

private:
    java::runtime::JvmStarter*                  mJvmStarter;
    java::runtime::RuntimeStarterUtils*         mRuntimeStarterUtils;
    const std::list<std::wstring>&              mOriginalArgs;
    std::list<std::wstring>::const_iterator     mOriginalArgsIter;
    std::list<std::wstring>                     mFlagArgs;
    std::wstring                                mJarFile;
    std::wstring                                mJadFile;
    std::wstring                                mClassPath;
    java::ui::CoreUiParams                      mUiParams;
    std::auto_ptr<java::util::Monitor>          mMonitor;
    bool                                        mShudownOk;
    bool                                        mIsMainApp;
    java::runtime::JvmStarter::Configuration    mConfiguration;
    java::util::Uid                             mAppUid;
};

} // end namespace java

#endif // JAVASTARTERIMPL_H
