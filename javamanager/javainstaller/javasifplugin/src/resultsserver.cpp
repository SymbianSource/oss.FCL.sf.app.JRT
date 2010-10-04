/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Comms server, part of Java Sif plugin.
*               When started  in 'commsresult' mode from Java Sif plugin
*               Java Installer sends the results of the operation
*               it executes (install, uninstall or component info)
*               to this server.
*
*/


#include <iostream>
#include <unistd.h>
#include <usif/usiferror.h>

#include "comms.h"
#include "javasymbianoslayer.h"
#include "logger.h"
#include "resultsserver.h"

using namespace java::comms;
using namespace std;

ResultsServer::ResultsServer(COpaqueNamedParams& aResults, CComponentInfo& aInfo) :
        mResults(aResults), mInfo(aInfo)
{
}

ResultsServer::~ResultsServer()
{
    // Stop server if running
    stop();

    iIntPairs.clear();
    iStringPairs.clear();
}

int ResultsServer::start()
{
    // Write reasonable error codes to mResults that can be used if
    // Java Installer never returns InstallerResultMessage.
    // If InstallerResultMessage is received the values will be overwritten.
    TRAPD(err, mResults.AddIntL(KSifOutParam_ErrCategory, EUnexpectedError));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::start mResults.AddIntL ErrCategory err %d", err);
    }

    TRAP(err, mResults.AddIntL(KSifOutParam_ErrCode, KErrUnknown));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::start mResults.AddIntL ErrCode err %d", err);
    }

    // TODO: return also localized error message (KSifOutParam_ErrMessage and
    // perhaps also KSifOutParam_ErrMessageDetails) from usif
    // common localization file after the localized strings are available


    mComms.registerDefaultListener(this);
    return mComms.start(IPC_ADDRESS_JAVA_SIF_PLUGIN_C);
}

int ResultsServer::stop()
{
    mComms.unregisterDefaultListener(this);
    return mComms.stop();
}

/**
 * Communicates with Java Installer. The following messages are used.
 *
 * Message Name        Id      Contents
 *
 *
 * Installer Result    601     message := length named_int_value* length named_string_value*
 *                             length := <int telling how many entries the following table has>
 *                             named_int_value := name int_value
 *                             named_string_value := name string_value
 *                             name := <string>
 *                             int_value := <int>
 *                             string_value := <string>
 * Installer Result
 *   Response          602     int response    (always 0)
 *
 *
 */
void ResultsServer::processMessage(CommsMessage& aMessage)
{
    TInt err = KErrNone;

    switch (aMessage.getMessageId())
    {
        case INSTALLER_RESULT_MESSAGE_ID:
        {
            int int_table_len;
            int string_table_len;
            int int_value;
            std::wstring name;
            std::wstring value;

            // Read raw message data
            aMessage >> int_table_len;
            for (int idx = 0; idx < int_table_len; idx++)
            {
                aMessage >> name >> int_value;
                iIntPairs.insert(std::make_pair(name, int_value));
            }

            aMessage >> string_table_len;
            for (int idx = 0; idx < string_table_len; idx++)
            {
                aMessage >> name >> value;
                iStringPairs.insert(std::make_pair(name, value));
            }

            // Which operation the Java Installer has executed
            int operation = iIntPairs[L"operation"];
            // result is KErrNone if the operation succeeded
            int result = iIntPairs[L"result"];

            if (KErrNone != result)
            {
                // return common error information;
                setCommonErrorInfo();

                if (INSTALL_OPERATION == operation)
                {
                    ELOG1(EJavaInstaller,
                        "ResultsServer::processMessage, Install failed with error %d", result);
                }
                else if (UNINSTALL_OPERATION == operation)
                {
                    ELOG1(EJavaInstaller,
                        "ResultsServer::processMessage, Uninstall failed with error %d", result);
                }
                else if (COMPONENT_INFO_OPERATION == operation)
                {
                    ELOG1(EJavaInstaller,
                        "ResultsServer::processMessage, Component info failed with error %d",
                        result);
                }
            }
            else
            {
                // Operation succeeded

                // Overwrite (reset) the default error values set for the case where no
                // InstallerResultMessage is never received
                resetDefaultErrorValues();

                if (INSTALL_OPERATION == operation)
                {
                    // Return the component ids of the installed Java application.
                    TComponentId resultComponentId = iIntPairs[L"suite-cid"];
                    TRAP(err, mResults.AddIntL(KSifOutParam_ComponentId, resultComponentId));
                    if (KErrNone != err)
                    {
                        ELOG1(EJavaInstaller,
                              "ResultsServer::processMessage mResults.AddIntL cid error %d", err);
                    }
                }
                else if (UNINSTALL_OPERATION == operation)
                {
                    // Return nothing if uninstall succeeds
                }
                else if (COMPONENT_INFO_OPERATION == operation)
                {
                    TRAP(err, setComponentInfoL());
                    if (KErrNone != err)
                    {
                        ELOG1(EJavaInstaller,
                              "ResultsServer::processMessage Cannot set CComponentInfo, error %d",
                              err);
                    }
                }
                else
                {
                    // Unknown operation
                    WLOG1(EJavaInstaller,
                        "ResultsServer::processMessage: Unknown operation (%d) executed",
                        operation);
                }
            }

            // Reply always with 'Response' message.
            CommsMessage reply;
            reply.replyTo(aMessage);
            reply.setMessageId(INSTALLER_RESULT_RESPONSE_MESSAGE_ID);
            reply << 0;

            int err = mComms.send(reply);
            if (err != 0)
            {
                ELOG1(EJavaInstaller,
                    "ResultsServer: Sending reply to Java Installer failed, err %d", err);
            }
        }
        break;

        default:
        {
            // Unknown message. Ignore it.
            WLOG1(EJavaInstaller,
                "ResultsServer: Unknown message received. Msg Id = %d",
                aMessage.getMessageId());
        }
        break;
    }

    clearData();
}


/**
 * Set common error information.
 * Note that the information is in member variables
 * iIntPairs and iStringPairs
 */
void ResultsServer::setCommonErrorInfo()
{
    // return common error information
    TRAPD(err, mResults.AddIntL(KSifOutParam_ErrCode, iIntPairs[L"error-code"]));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::setCommonErrorInfo mResults.AddIntL ErrCode err %d", err);
    }

    TRAP(err, mResults.AddIntL(
        KSifOutParam_ErrCategory, iIntPairs[L"error-category"]));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::setCommonErrorInfo mResults.AddIntL ErrCategory err %d",
            err);
    }

    HBufC *message = wstringToBuf(iStringPairs[L"error-message"]);
    if (!message)
    {
        ELOG(EJavaInstaller,
              "ResultsServer::setCommonErrorInfo mResults.wstringToBuf returned NULL ");
    }
    else
    {
        TRAP(err, mResults.AddStringL(KSifOutParam_ErrMessage, *message));
        if (KErrNone != err)
        {
            ELOG1(EJavaInstaller,
                "ResultsServer::setCommonErrorInfo mResults.AddStringL ErrMessage err %d",
                err);
        }
        delete message;
    }

    message = wstringToBuf(iStringPairs[L"error-details"]);
    if (!message)
    {
        ELOG(EJavaInstaller,
              "ResultsServer::setCommonErrorInfo mResults.wstringToBuf 2 returned NULL ");
    }
    else
    {
        TRAP(err, mResults.AddStringL(KSifOutParam_ErrMessageDetails, *message));
        if (KErrNone != err)
        {
            ELOG1(EJavaInstaller,
                "ResultsServer::setCommonErrorInfo mResults.AddStringL ErrMessageDetails "
                "err %d", err);
        }
        delete message;
    }
}


/**
 * Overwrite (reset) the default error values to 'no error'.
 * The default error values were originally set for the case
 * where no InstallerResultMessage is never received and we must
 * return sensible error information.
 */
void ResultsServer::resetDefaultErrorValues()
{
    TRAPD(err, mResults.AddIntL(KSifOutParam_ErrCategory, 0));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::resetDefaultErrorValues mResults.AddIntL ErrCategory err %d", err);
    }

    TRAP(err, mResults.AddIntL(KSifOutParam_ErrCode, 0));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
            "ResultsServer::resetDefaultErrorValues mResults.AddIntL ErrCode err %d", err);
    }

    // TODO: reset also localized error message KSifOutParam_ErrMessage and
    // perhaps also KSifOutParam_ErrMessageDetails if they have been set in start()
}


void ResultsServer::clearData()
{
    iIntPairs.clear();
    iStringPairs.clear();
}


void ResultsServer::setComponentInfoL()
{
    // Component == MIDlet Suite
    TCapabilitySet emptySet;
    emptySet.SetEmpty();

    std::wstring wSuiteName = iStringPairs[L"suite-name"];
    TPtr16 suiteName((TUint16 *)wSuiteName.c_str(), wSuiteName.size(), wSuiteName.size());

    std::wstring wVersion = iStringPairs[L"version"];
    TPtr16 version((TUint16 *)wVersion.c_str(), wVersion.size(), wVersion.size());

    std::wstring wVendor = iStringPairs[L"vendor"];
    TPtr16 vendor((TUint16 *)wVendor.c_str(), wVendor.size(), wVendor.size());

    std::wstring wSuiteGid = iStringPairs[L"suite-gid"];
    TPtr16 suiteGid((TUint16 *)wSuiteGid.c_str(), wSuiteGid.size(), wSuiteGid.size());

    RPointerArray<CComponentInfo::CApplicationInfo> applications;
    CleanupResetAndDestroyPushL(applications);
    int n = 1;
    do
    {
        // construct wstring 'midlet-uid-<n>'. e.g. 'midlet-uid-1'
        std::wstring midletUidN(L"midlet-uid-");
        std::wstringstream ss;
        ss << midletUidN;
        ss << n;
        ss >> midletUidN;

        //LOG1WSTR(EJavaInstaller, EInfo,
        //  "ResultsServer::processMessage: checking %S", midletUidN.c_str());

        int uid = iIntPairs[midletUidN];
        if (uid == 0)
        {
            // no more midlets in the suite
            break;
        }

        // construct wstring 'midlet-name-<n>'. e.g. 'midlet-name-1'
        std::wstring midletNameN(L"midlet-name-");
        std::wstringstream ss2;
        ss2 << midletNameN;
        ss2 << n;
        ss2 >> midletNameN;
        std::wstring wMidletName = iStringPairs[midletNameN];
        TPtr16 midletName((TUint16 *)wMidletName.c_str(), wMidletName.size(), wMidletName.size());

        // Construct application information and append it to applications list.
        CComponentInfo::CApplicationInfo* applicationInfo =
            CComponentInfo::CApplicationInfo::NewLC(
                TUid::Uid(uid), midletName, KNullDesC(), KNullDesC());
        applications.AppendL(applicationInfo);
        CleanupStack::Pop(applicationInfo);

        n++;
    } // sanity check: no suite can have 10000 midlets
    while (n < 10000);   // codescanner::magicnumbers

    CComponentInfo::CNode *rootNode = NULL;
    rootNode = CComponentInfo::CNode::NewLC(
                   KSoftwareTypeJava,
                   suiteName,
                   version,
                   vendor,
                   EActivated, // Java applications are always active
                   (Usif::TInstallStatus)(iIntPairs[L"install-status"]),
                   iIntPairs[L"suite-cid"],  // TComponentId
                   suiteGid,
                   (Usif::TAuthenticity)(iIntPairs[L"authenticity"]),
                   emptySet, // Java applications use J2ME permissions, not capabilities
                   iIntPairs[L"component-size"], // max installed size is given by java installer
                   ETrue, // suite has always at least one MIDlet
                   EFalse, // drive selection is not required
                   &applications
               );

    // Store whole component info tree
    mInfo.SetRootNodeL(rootNode);
    CleanupStack::Pop(rootNode);
    CleanupStack::PopAndDestroy(&applications);
}
