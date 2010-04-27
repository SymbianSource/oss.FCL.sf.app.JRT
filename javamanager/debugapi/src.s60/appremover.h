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
* Description: AppRemover uninstalls Java application
*
*/


#ifndef APPREMOVER_H
#define APPREMOVER_H

#include "installoperation.h"
#include "javaosheaders.h"
#include "javauid.h"
using java::util::Uid;

namespace java
{
namespace debug
{
OS_NONSHARABLE_CLASS(AppRemover) : public InstallOperation
{
public:
    AppRemover(const Uid& aSuiteUid);
    virtual ~AppRemover();

    int uninstall();

protected:
    virtual CommsMessage getNextOperation();
    virtual void handleOperationResult(CommsMessage& aMessage);

private:
    const Uid mSuiteUid;

    bool mUninstallDone;
    int mUninstallResult;
};

} // end namespace debug
} // end namespace java


#endif // APPREMOVER_H
