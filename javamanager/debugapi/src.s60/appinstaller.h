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
* Description: AppInstaller takes care of installing Java application
*
*/


#ifndef APPINSTALLER_H
#define APPINSTALLER_H

#include <string>
#include <list>

#include "installoperation.h"
#include "javaosheaders.h"
#include "javauid.h"
using java::util::Uid;

namespace java
{
namespace debug
{
OS_NONSHARABLE_CLASS(AppInstaller) : public InstallOperation
{
public:
    AppInstaller(const std::wstring& aFilename);
    virtual ~AppInstaller();

    int install();
    Uid getSuiteUid() const;
    std::list<Uid> getAppUids() const;

protected:
    virtual CommsMessage getNextOperation();
    virtual void handleOperationResult(CommsMessage& aMessage);

private:
    const std::wstring mFilename;
    Uid mSuiteUid;
    std::list<Uid> mAppUids;

    bool mInstallDone;
    int mInstallResult;
};

} // end namespace debug
} // end namespace java


#endif // APPINSTALLER_H
