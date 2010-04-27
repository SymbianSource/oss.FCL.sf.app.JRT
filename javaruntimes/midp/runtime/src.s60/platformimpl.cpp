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
* Description:  Platform dependent implementations.
 *
*/


#include <DRMHelper.h>
#include <drmrightsinfo.h>

#include "logger.h"
#include "platformimpl.h"
#include "javacommonutils.h"

void consumeRigthsImplL(const std::wstring& uri, std::string& status,
                        const CDRMHelper::TDRMHelperConsumeAction action,
                        int& handle)
{
    JELOG2(EJavaRuntime);
    LOG1(EJavaRuntime, EInfo, "consumeRigthsImplL. URI: %S", uri.c_str());


    // Convert wstring to 16 bit descriptor.
    TPtrC ptr((const TUint16 *)uri.c_str(), uri.length());

    CDRMHelper* drmHelper = reinterpret_cast<CDRMHelper*>(handle);

    // In case of start we need to create instance of CDRMHelper.
    if (action == CDRMHelper::EStart)
    {
        drmHelper = CDRMHelper::NewLC();
        handle = reinterpret_cast<int>(drmHelper);
    }

    int err = -1;
    if (drmHelper)
    {
        err = drmHelper->ConsumeFile2(ptr, ContentAccess::EExecute, action);
    }
    if (action == CDRMHelper::EFinish)
    {
        delete drmHelper;
        drmHelper = NULL;
    }
    else
    {
        CleanupStack::Pop(drmHelper);
    }
    if (err != KErrNone)
    {
        delete drmHelper;
        handle = 0;
    }

    switch (err)
    {
    case KErrNone:
        status.clear();
        break;

    case KErrCANoPermission:
        status = "Rights exist but the execute intent is not permitted";
        break;

    case KErrCAPendingRights:
        status = "Rights have not yet arrived but are expected soon";
        break;

    case KErrPermissionDenied:
        status = "Client is not allowed to use this content object";
        break;

    default:
        ELOG1(EJavaRuntime, "CDRMHelper::ConsumeFile2 failed: %d", err);
        User::Leave(err);
        break;

    }
}

bool checkRigthsL(const std::wstring& drmContentId, std::string& status)
{
    JELOG2(EJavaRuntime);
    LOG1(EJavaRuntime, EInfo, "checkRigthsL. drmContentId: %S",
         drmContentId.c_str());

    // Convert wstring to 16 bit descriptor.
    TPtrC ptr((const TUint16 *)drmContentId.c_str(), drmContentId.length());

    DRM::CDrmRightsInfo* ri(DRM::CDrmRightsInfo::NewLC());
    DRM::TDrmRightsInfo rightsInfo(DRM::EURightsInfoValid);

    // Check if there are still valid rigths.
    TRAPD(err, ri->CheckRightsL(ptr,
                                ContentAccess::EExecute,
                                rightsInfo)) ;

    CleanupStack::PopAndDestroy(ri);
    // If leave happened consider it fatal.
    if (err != KErrNone)
    {
        ELOG1(EJavaRuntime, "CheckRightsL failed: %d", err);
        User::Leave(err);
    }

    LOG1(EJavaRuntime, EInfo, "checkRigthsL, rightsInfo: %d", rightsInfo);


    if (rightsInfo == DRM::EURightsInfoValid)
    {
        return true;
    }

    WLOG1(EJavaRuntime, "No more rights to start MIDlet: rightsInfo = %d",
          rightsInfo);
    status = "No more rights to start MIDlet";
    return false;
}

void consumeRigthsImpl(const std::wstring& uri,
                       const std::wstring& drmContentId,
                       std::string& status,
                       bool startPhase,
                       int& handle)
{
    JELOG2(EJavaRuntime);

    bool rightsOk = true;

    int err = KErrNone;
    // Need to check only when starting the MIDlet.
    if (startPhase)
    {
        // Check if there are still valid rights.
        TRAP(err, rightsOk = checkRigthsL(drmContentId, status););
    }

    if (err == KErrNone && rightsOk)
    {
        // Rigths where still valid, so lets consume rights.
        const CDRMHelper::TDRMHelperConsumeAction action =
            startPhase ? CDRMHelper::EStart : CDRMHelper::EFinish;
        TRAP(err, consumeRigthsImplL(uri, status, action, handle););
    }
    if (err != KErrNone)
    {
        ELOG1(EJavaRuntime, "ERROR!!! consumeRigthsImpl() %d", err);
        status = "General OS error code: ";
        status += java::util::JavaCommonUtils::intToString(err);
    }
}
