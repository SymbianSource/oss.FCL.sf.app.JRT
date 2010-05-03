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
* Description:  Java Sid Checker is an ECOM plugin for Symbian AppArc
 *                (database that contains info on all applications installed
 *                 to the device)
 *                It tells AppArc whether Java application installed
 *                to a removable storage media is present and can be executed.
 *
*/


#ifndef JAVASIDCHECKER_H
#define JAVASIDCHECKER_H

#include <apsidchecker.h>
#include <javaregistryincludes.h>

NONSHARABLE_CLASS(CJavaSidChecker) : public CAppSidChecker
        /** Java Sid checker.

        This is an implementation of CAppSidChecker for checking native application Sids

        This instance of the CAppSidChecker is used to verify that a native application
        installed and can be launched.
        */
{
public:
    static CJavaSidChecker* NewL();
    ~CJavaSidChecker();

    // From CAppSidChecker
    TBool AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive);

    void SetRescanCallBackL(const TCallBack &aCallback);

private:
    CJavaSidChecker();
    TBool AppRegisteredAtL(const TUid& aSid, TDriveUnit aDrive);

    Java::CJavaRegistry* iJavaRegistry;
};

#endif // JAVASIDCHECKER_H
