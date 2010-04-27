/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The executable that enables launching OMJ Java
*                applications in S60
*
*/

#include <apacmdln.h>
#include <bacline.h>
#include <e32cmn.h>
#include <s32mem.h>
#include <unistd.h>

#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "exceptionbase.h"
#include "javainifileutils.h"
#include "javaoslayer.h"
#include "javastorage.h"
#include "javastorageentry.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"
#include "rtcmessages.h"
#include "runtimeexception.h"


using namespace java::captain;
using namespace java::storage;
using namespace java::util;


_LIT(KSemiColon, ";");

const TInt KExtraLenForLoggingAndPrompting = 22;
const TInt KArgumentValueMaxLen = 1568;  // Support worst case % encoded args of 512 chars

// The Uid of the Web Browser process
#define KBrowserUid 0x10008D39


/**
 * Return the Uid of the application aMidletName in package aPackageId.
 * If aMidletName is empty, return the Uid of the first application in
 * package aPackageId.
 *
 * @param aJs           JavaStorage connection, must be open
 * @param aPackageId    the id of the package
 * @param aMidletName   the name of the desired application in the package, can be empty
 * @param aUid          the Uid the found application is returned in this param
 * @return  KErrNone if Uid was found, otherwise one of the Symbian error codes
 * @throws  JavaStorageException if accessing Java Storage fails
 */
static TInt getOneApplicationFromPackage(
    JavaStorage& aJs,
    const std::wstring& aPackageId,
    const std::wstring& aMidletName,
    TInt32 &aUid)
{
    JavaStorageEntry attribute;
    JavaStorageApplicationEntry_t findPattern;
    JavaStorageApplicationList_t  foundEntries;

    // Get ID from APPLICATION_TABLE based on PACKAGE_ID and NAME
    attribute.setEntry(PACKAGE_ID, aPackageId);
    findPattern.insert(attribute);
    if (aMidletName.length() > 0)
    {
        attribute.setEntry(NAME, aMidletName);
        findPattern.insert(attribute);
    }
    attribute.setEntry(ID, L"");
    findPattern.insert(attribute);

    aJs.search(APPLICATION_TABLE, findPattern, foundEntries);

    if (foundEntries.size() < 1)
    {
        return KErrNotFound;
    }

    std::wstring applicationId = foundEntries.front().begin()->entryValue();
    TUid tmp;
    TInt err = uidToTUid(applicationId, tmp);
    if (KErrNone == err)
    {
        aUid = tmp.iUid;
    }

    return err;
}


/**
 * Decode %XX encoded UTF-8 characters embedded to UCS-2
 *
 * @param aCmdLineBuf  The command line to be decoded.
 */
static void decodeCommandLineL(TPtr &aCmdLineBuf)
{
    _LIT(KPercentage, "%");
    TInt ind = aCmdLineBuf.Find(KPercentage);
    if (KErrNotFound == ind)
    {
        // nothing to decode
        return;
    }

    LOG(EJavaCaptain, EInfo,
        "JavaLauncher:decodeCommandLineL convert percent encoded UTF-8 sequences to UTF-16 ");

    try
    {
        std::wstring cmdLine = desToWstring(aCmdLineBuf);
        std::wstring res = JavaCommonUtils::percentDecode(cmdLine);

        LOG1WSTR(EJavaCaptain, EInfo,
                 "JavaLauncher:decodeCommandLineL: decoded command line is %s", res.c_str());

        aCmdLineBuf = (const TUint16 *)res.c_str();
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaCaptain,
              "JavaLauncher:decodeCommandLineL: cannot decode command line: %s", e.what());
        User::Leave(KErrGeneral);
    }
}


/**
 * Return the value of the argument specified by aArgName
 *
 * @param aMidletCmdLine command line to be parsed
 * @param aArgName the name of the argument
 * @return the value of the argument or empty wstring if no such argument
 */
static std::wstring getArgValue(const TPtrC &aMidletCmdLine, const TDesC &aArgName)
{
    TBuf<KArgumentValueMaxLen> valueBuf;
    TInt argPos = aMidletCmdLine.FindF(aArgName);
    if (argPos >= 0)
    {
        TInt semicolonPos = aMidletCmdLine.Mid(argPos).Find(KSemiColon);
        if (KErrNotFound == semicolonPos)
        {
            semicolonPos = aMidletCmdLine.Mid(argPos).Length();
        }
        TInt argLen = semicolonPos - aArgName.Length();
        if (argLen >= KArgumentValueMaxLen)
        {
            // Protect from buffer overflow.
            WLOG2(EJavaCaptain,
                  "javalauncher: argument value len too long (%d), cutting it to %d",
                  argLen, (KArgumentValueMaxLen - 1));
            argLen = KArgumentValueMaxLen - 1;
        }

        valueBuf = aMidletCmdLine.Mid(argPos + aArgName.Length(),  argLen);
    }
    valueBuf.Append('\0');

    std::wstring value = (wchar_t *)&(valueBuf[0]);
    return value;
}


/**
 * Parse the names of the MIDlet suite and MIDlet vendor from aMidletCmdLine
 * parameter and use them to find the MIDlet suite from Java Storage.
 * Then return Uid of the named MIDlet or if 'midlet-n' argument is not given
 * in command line, the Uid of the first MIDlet in the suite.
 * Return the uid of the MIDlet in aUid.
 *
 * @param aMidletCmdLine the command line that contains at least midlet-name
 *  and midlet-vendor arguments
 * @param aUid if the MIDlet is found from Java Storage, contains the Uid after return.
 * @return KErrNone if MIDlet was found. KErrPathNotFound if MIDlet is not found.
 *  Standard Symbian error codes in other cases.
 */
static TInt getUidByNames(const TPtrC &aMidletCmdLine, TInt32 &aUid)
{
    _LIT(KMidletNameArg, "midlet-name=");
    _LIT(KMidletVendorArg, "midlet-vendor=");
    _LIT(KMidletNArg, "midlet-n=");

    TInt err = aMidletCmdLine.FindF(KMidletNameArg);
    if (KErrNotFound == err)
    {
        return KErrArgument;
    }
    err = aMidletCmdLine.FindF(KMidletVendorArg);
    if (KErrNotFound == err)
    {
        return KErrArgument;
    }

    std::wstring suiteName = getArgValue(aMidletCmdLine, KMidletNameArg);
    std::wstring vendorName = getArgValue(aMidletCmdLine, KMidletVendorArg);
    std::wstring midletName = getArgValue(aMidletCmdLine, KMidletNArg);

    if (suiteName.empty() || vendorName.empty())
    {
        return KErrArgument;
    }

    // Find application uid based on names from Java Storage
    JavaStorage *js = JavaStorage::createInstance();

    try
    {
        js->open(JAVA_DATABASE_NAME);

        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t  foundEntries;

        // Search for package ID by PACKAGE_NAME and VENDOR from APPLICATION_PACKAGE_TABLE
        attribute.setEntry(PACKAGE_NAME, suiteName);
        findPattern.insert(attribute);
        attribute.setEntry(VENDOR, vendorName);
        findPattern.insert(attribute);
        attribute.setEntry(ID, L"");
        findPattern.insert(attribute);

        js->search(APPLICATION_PACKAGE_TABLE , findPattern, foundEntries);

        // Is anything found
        if (foundEntries.size() > 0)
        {
            // The application package has been found, get the ID of the package
            std::wstring value = foundEntries.front().begin()->entryValue();
            LOG1WSTR(EJavaCaptain, EInfo,
                     "JavaLauncher: getUidByNamesL: Found suite uid by name. Uid is %s",
                     value.c_str());

            // Now find the Uid of the first or specified application in the package
            err = getOneApplicationFromPackage(*js, value, midletName, aUid);
        }
        else
        {
            err = KErrPathNotFound;
        }
    }
    catch (JavaStorageException& e)
    {
        ELOG1(EJavaCaptain, "Java Storage exception %s", e.what());
        err = KErrGeneral;
    }

    try
    {
        js->close();
    }
    catch (JavaStorageException& e2)
    {
        WLOG1(EJavaCaptain, "Java Storage exception when closing storage %s", e2.what());
    }

    delete js;

    return err;
}


/**
 * Check whether the MIDlet has been installed to device.
 *
 * @param aUid the Uid of the MIDlet to be checked
 * @return KErrNone if the MIDlet has been installed,
 *         KErrNotFound if the MIDlet has not been installed,
 *         general Symbian error codes in case of error
 */
static TInt verifyAppExists(TInt32 aUid)
{
    TInt err = KErrNone;

    if (aUid == 0)
    {
        return KErrNotFound;
    }

    // Find application uid based on names from Java Storage
    JavaStorage *js = JavaStorage::createInstance();

    try
    {
        js->open(JAVA_DATABASE_NAME);

        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t  foundEntries;

        // Search for application NAME by ID from APPLICATION_TABLE
        Uid uid;
        TUid tuid;
        tuid.iUid = aUid;
        TUidToUid(tuid, uid);
        attribute.setEntry(ID, uid.toString());
        findPattern.insert(attribute);
        attribute.setEntry(NAME, L"");
        findPattern.insert(attribute);

        js->search(APPLICATION_TABLE , findPattern, foundEntries);

        // Is anything found
        if (foundEntries.size() > 0)
        {
            err = KErrNone;
        }
        else
        {
            err = KErrNotFound;
        }
    }
    catch (JavaStorageException& e)
    {
        ELOG1(EJavaCaptain, "Java Storage exception %s", e.what());
        err = KErrGeneral;
    }

    try
    {
        js->close();
    }
    catch (JavaStorageException& e2)
    {
        WLOG1(EJavaCaptain, "Java Storage exception when closing storage %s", e2.what());
    }

    delete js;

    return err;
}


/**
 * Either parse the Uid from the value of 'midlet-uid' parameter in
 * command line given in aMidletCmdLine or find the midlet
 * based on the 'midlet-name' and 'midlet-vendor' from
 * Java Storage / AppArc and return the Uid of the midlet.
 *
 * @param aMidletCmdLine  command line to be parsed, the format is
 *  [midlet-name=XXX;midlet-vendor=XXX;|midlet-uid=YYY;]midlet-args=XXX
 * @param aUid will contain the Uid parsed from command line
 * @return KErrNone if the command line specified Uid
 */
static TInt getUidFromCommandLine(const TPtrC &aMidletCmdLine, TInt32 &aUid)
{
    _LIT(KMidletUidArg, "midlet-uid=");
    _LIT(KHexValueStart, "0x");
    TInt err(KErrNone);
    TInt argPos = aMidletCmdLine.FindF(KMidletUidArg);
    if (KErrNotFound != argPos)
    {
        TPtrC uidToParse = aMidletCmdLine.Mid(argPos + KMidletUidArg.iTypeLength);
        TLex parseUid(uidToParse);
        if (uidToParse.FindF(KHexValueStart) == 0)
        {
            parseUid.Inc(2); // skip hex prefix
            TUint32 tmpValue;
            err = parseUid.Val(tmpValue, EHex);
            aUid = tmpValue;
        }
        else
        {
            err = parseUid.Val(aUid);
        }

        if (KErrNone != err)
        {
            ELOG1(EJavaCaptain,
                  "javalauncher failed parsing app Uid from cmdline midlet-uid param. Error %d",
                  err);
        }

        err = verifyAppExists(aUid);
    }
    else
    {
        err = getUidByNames(aMidletCmdLine, aUid);
        if (KErrNone != err)
        {
            if (KErrArgument == err)
            {
                // In this case the Uid is not in opaque data and there are
                // no midlet-uid or midlet-name + midlet-vendor parameters in the command line
                ELOG(EJavaCaptain,
                     "javalauncher cannot launch app because there is no info to determine Uid.");
            }
            else
            {
                ELOG1(EJavaCaptain,
                      "javalauncher failed getting app Uid based on cmdline midlet-name and "
                      "midlet-vendor params. Error %d",
                      err);
            }
        }
    }

    return err;
}


/**
 * Parse CApaCommandLine and optional normal process command line.
 * Determine the uid of the Java application to be started
 *
 * @param aCmdLine returns value of midlet-args
 * @param aUid returns Uid of the Java application to be started
 * @param aBackGroundLaunch returns info whether back ground launch is needed
 * @return KErrNone or error code
 */
TInt getCmdLineAndUidL(HBufC **aCmdLine, TInt *aUid, TBool *aBackGroundLaunch)
{
    CApaCommandLine* commandLine;

    // CApaCommandLine command line is used when AppArc is launching Java application.
    // Uid is in opaque data and possible document name is in document name field.
    TInt err = CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
    if (KErrNone != err)
    {
        return err;
    }
    CleanupStack::PushL(commandLine);

    // The following feature is not supported yet.
    // It needs changes also to Java Installer and to
    // Symbian Settings UI
    TInt maxExtraSpaceForDocumentArg = 0;
#ifdef RD_JAVA_SUPPORT_JAVA_APPS_AS_MIME_TYPE_HANDLERS
    _LIT(KDocumentOnlyCmdLine, "midlet-args=document=");
    TFullName documentName;
    documentName = commandLine->DocumentName();
    if (documentName.Length() > 0)
    {
        maxExtraSpaceForDocumentArg =
            documentName.Length() + KDocumentOnlyCmdLine.iTypeLength +
            KSemiColon.iTypeLength;

        LOG1WSTR(EJavaCaptain, EInfo,
                 "JavaLauncher: document to handle is %s",
                 (wchar_t *)(documentName.PtrZ()));
    }
#endif

    // User::CommandLine command line is used when Java application is launched
    // from native application or from javaapp: or localapp:/jam/launch/ scheme
    // handler plugin.
    HBufC *pBufCmdLine =
        HBufC::NewLC(User::CommandLineLength() +
                     maxExtraSpaceForDocumentArg +
                     KExtraLenForLoggingAndPrompting);
    TPtr cmdLineBuf = pBufCmdLine->Des();
    User::CommandLine(cmdLineBuf);

    if (pBufCmdLine->Length() > 0)
    {
        LOG1WSTR(EJavaCaptain, EInfo,
                 "JavaLauncher: command line of this process is %s",
                 (wchar_t *)(cmdLineBuf.PtrZ()));

        // If the commandline contains hex encoded UTF-8 characters %XX decode them to UCS-2 (UTF-16)
        // For example "%C3%80%C3%80nes%C3%80.txt"
        decodeCommandLineL(cmdLineBuf);
    }

    TPtrC8 data = commandLine->OpaqueData();
    TInt32 uid = 0;  // application uid
    // New OMJ Java applications have 4 bytes of opaque data.
    TBool uidInOpaqueData = (data.Length() == 4);
    TBool backGroundLaunch =
        (EApaCommandBackground == commandLine->Command()) ||
        (EApaCommandBackgroundAndWithoutViews == commandLine->Command());

    if (uidInOpaqueData)
    {
        RDesReadStream stream(data);
        // OMJ java application, read the application uid
        TRAP(err, uid = stream.ReadInt32L());
        stream.Close();
        if (KErrNone != err)
        {
            ELOG1(EJavaCaptain,
                  "javalauncher failed reading app Uid from cmdline opaque data. Error %d",
                  err);

            CleanupStack::PopAndDestroy(pBufCmdLine);
            CleanupStack::PopAndDestroy(commandLine);
            return err;
        }
    }
    else
    {
        // Can the midlet uid be determined using info in the commandline?
        err = getUidFromCommandLine(cmdLineBuf, uid);
        if (KErrNone != err)
        {
            CleanupStack::PopAndDestroy(pBufCmdLine);
            CleanupStack::PopAndDestroy(commandLine);
            return err;
        }
    }

#ifdef RD_JAVA_SUPPORT_JAVA_APPS_AS_MIME_TYPE_HANDLERS
    // Add possible document name to Java application command line.

    // javalauncher.exe can be started with CApaCommandLine coming from AppArc
    // that may specify document name but no other arguments for Java application.
    // In this case the current command line is empty and we must create a command
    // line that contains the document name.
    if (pBufCmdLine->Length() == 0)
    {
        if (documentName.Length() > 0)
        {
            cmdLineBuf.Append(KDocumentOnlyCmdLine);
            cmdLineBuf.Append(documentName);
            cmdLineBuf.Append(KSemiColon);
        }
    }
#endif

    if (cmdLineBuf.Length() > 0)
    {
        LOG1WSTR(EJavaCaptain, EInfo,
                 "JavaLauncher: full java application cmd line is : %s",
                 (wchar_t *)(cmdLineBuf.PtrZ()));
    }

    // Uid has already been determined, the whole command line is not needed
    // anymore, only the arguments for the Java application
    _LIT(KMidletArgs, "midlet-args=");
    TInt  argsPos = cmdLineBuf.FindF(KMidletArgs);
    TBool cmdLineIsNotEmpty = EFalse;
    if (argsPos >= 0)
    {
        // Pass everything that follows "midlet-args="
        cmdLineBuf.Delete(0, argsPos + KMidletArgs.iTypeLength);
        cmdLineIsNotEmpty = ETrue;
    }
    else
    {
        // No arguments for the Java application
        cmdLineBuf.Zero();
    }

    // If Browser starts Java application with
    // command line parameters, the user must be asked for a confirmation
    // before the application is started.
    // This prevents automatic starting of Java applications from a web
    // page using 'javaapp:' url and java script.
    TUid parentUid = User::CreatorIdentity();
    LOG1(EJavaCaptain, EInfo, "JavaLauncher: Parent process uid is %x", parentUid.iUid);
    if (parentUid.iUid == KBrowserUid)
    {
        // Pass information that the application start up must be prompted
        // in the java application command line
        if (cmdLineIsNotEmpty)
        {
            cmdLineBuf.Append(KSemiColon);
        }
        _LIT(KPromptStart, "PromptAppStartup");
        cmdLineBuf.Append(KPromptStart);
    }

    // Return command line, uid and info whether this is back ground launch
    *aBackGroundLaunch = backGroundLaunch;
    *aUid = uid;
    CleanupStack::Pop(pBufCmdLine);
    *aCmdLine = pBufCmdLine;
    CleanupStack::PopAndDestroy(commandLine);

    return KErrNone;
}


/**
 *  Start Java Captain process
 */
TInt startJavaCaptain()
{
    // Start 'systemams.exe' in S60 5.0 (it will start javacaptain) and
    // 'javacaptain.exe' in 9.2 and later

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    _LIT(KJavaCaptainExe, "systemams.exe");
#else
    _LIT(KJavaCaptainExe, "javacaptain.exe");
#endif
    _LIT(KJavaCaptainArg, "");
    RProcess proc;
    int err = proc.Create(KJavaCaptainExe, KJavaCaptainArg);
    if (err == KErrNone)
    {
        proc.Resume();
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        LOG(EJavaCaptain, EInfo, "javalauncher: startJavaCaptain systemams.exe was started ok");
#else
        LOG(EJavaCaptain, EInfo, "javalauncher: startJavaCaptain javacaptain.exe was started ok");
#endif

        // Wait 3 seconds so that Java Captain has time to start
        User::After(3000000);
    }
    else
    {
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        ELOG1(EJavaCaptain, "javalauncher: startJavaCaptain start systemams.exe failed: %d", err);
#else
        ELOG1(EJavaCaptain, "javalauncher: startJavaCaptain start javacaptain.exe failed: %d", err);
#endif
    }
    proc.Close();

    return err;
}


/**
 * Determine the uid of the Java application to be started and
 * ask Java Captain to launch the application.
 * Pass parsed arguments for the application in the Comms message
 * that is sent to Java Captain to start the applciation.
 *
 * @return KErrNone or error code
 */
TInt handleLaunchL(void)
{
    HBufC *pBufCmdLine = NULL;
    TInt   uid;
    TBool  backGroundLaunch = EFalse;
    TInt err = getCmdLineAndUidL(&pBufCmdLine, &uid, &backGroundLaunch);
    if (KErrNone != err)
    {
        return err;
    }
    TPtr cmdLineBuf = pBufCmdLine->Des();

    LOG1(
        EJavaCaptain,
        EInfo,
        "javalauncher launching app uid %d", uid);

    try
    {
        JavaOsLayer::startUpTrace("JavaLauncher: starting Comms", -1, -1);
        CommsMessage message;
        message.setModuleId(PLUGIN_ID_RTC_C);
        message.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
        message.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        TUid tuid = TUid::Uid(uid);
        Uid uuid;
        std::wstring applicationArguments(desToWstring(cmdLineBuf));

        setLaunchApplicationReqParams(message, TUidToUid(tuid, uuid),
                                      (backGroundLaunch ? RTC_LAUNCH_TYPE_BACKGROUND_C : RTC_LAUNCH_TYPE_NORMAL_C),
                                      RTC_LAUNCH_OPTIONS_NONE_C,
                                      RTC_LAUNCH_RUNTIME_MIDP_C,
                                      applicationArguments);

        CommsClientEndpoint comms;
        err = comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        if (KErrNotFound == err)
        {
            ELOG(EJavaCaptain,
                 "javalauncher: Java Captain was not running. Trying to restart it.");

            // Java Captain is not running, try to start it
            err = startJavaCaptain();
            if (KErrNone == err)
            {
                err = comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
            }
        }

        if (KErrNone == err)
        {
            err = comms.send(message);
        }

        if (KErrNone != err)
        {
            ELOG1(EJavaCaptain,
                  "javalauncher: OMJ app launch: Comms connect/send failed. Error %d ",
                  err);
        }

        // Ignore possible errors in disconnect
        (void)comms.disconnect();
    }
    catch (ExceptionBase& e)
    {
        ELOG1(EJavaCaptain,
              "javalauncher: OMJ app launch: ExceptionBase caught: %s ",
              e.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaCaptain,
              "javalauncher: OMJ app launch: Exception %s caught", e.what());
    }

    return err;
}


/**
 * Main function of executable javalauncher.exe.
 * Symbian AppArc starts this executable when AppArc APIs are used to
 * start a Java application that has been registered to AppArc.
 * (Because this application has been registered to AppArc as the starter
 * application for all OMJ Java applications.)
 *
 * Reads the opaque data of the CApaCommandLine command line and
 * if the opaque data contains valid OMJ Java application Uid
 * sends a COMMS message to Java Captain asking that Captain starts
 * the application specified by the Uid.
 *
 * The document field of the CApaCommandline command line may contain the full path
 * name of the document that the java application should handle. It will be passed
 * to the Java application as an argument.
 *
 * This executable can be started also directly from other native processes
 * to start a Java application with arguments specified in normal process
 * command line.
 *
 * The command line format is
 * [midlet-name=XXX;midlet-vendor=XXX;[midlet-n=XXX;]|midlet-uid=YYY;]midlet-args=XXX
 * for example
 * midlet-name=Chess;midlet-vendor=Nokia;midlet-args=startMode=playChessDemo;sound=off;
 * 'midlet-args' specifies the arguments passed to Java application.
 * 'midlet-uid' or 'midlet-name'+'midlet-vendor' specify the Java application to be started
 *
 * Sample code for starting MIDlet from native code
 * @code
    RProcess rProcess;
    TInt err = rProcess.Create(_L("javalauncher.exe"),
        _L("midlet-uid=0x10137c4d;midlet-args=startMode=startFromCmdLine;sound=ON;landscapeMode=true;"));
    if (KErrNone == err)
    {
        TRequestStatus status;
        rProcess.Logon(status);
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            // Launching midlet failed
            ...
        }
    }
    else
    {
        // Cannot start javalauncher.exe
        ...
    }
    rProcess.Close();
 * @endcode
 *
 */
TInt E32Main(void)
{
    JavaOsLayer::startUpTrace("JavaLauncher: Start", -1, -1);

    CTrapCleanup *pCleanupStack = CTrapCleanup::New();
    if (NULL == pCleanupStack)
    {
        ELOG(EJavaCaptain, "Cannot create CleanupStack in javalauncher main()");
        return KErrNoMemory;
    }

    TInt ret = KErrNone;
    TRAPD(err, ret = handleLaunchL());
    if (KErrNone != err)
    {
        ELOG1(EJavaCaptain, "javalauncher.exe: handleLaunchL leaved with err %d", err);
        delete pCleanupStack;
        return err;
    }

    delete pCleanupStack;
    return ret;
}

