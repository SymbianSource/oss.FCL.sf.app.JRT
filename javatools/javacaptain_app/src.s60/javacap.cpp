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
* Description:  JavaCap
*
*/


#include "comms.h"
#include "rtcmessages.h"
#include "coremessages.h"

#include "logger.h"

#include "javacap.h"
#include "javacap.hrh"

#include "javainifileutils.h"

#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>

#include "javastoragenames.h"                   // Storage constants.
#include "securitystoragedatadefs.h"

#include <javacap.rsg>

_LIT(KJavaCapYes, "Yes");
_LIT(KJavaCapNo,  "No");
_LIT(KUserSecurityMode,  "User");
_LIT(KNormalSecurityMode,  "Default");
_LIT(KS60SecurityPolicyName,  "S60");
_LIT(KMSASecurityPolicyName,  "MSA");
_LIT(KATTSecurityPolicyName,  "AT&T");
_LIT(KS60SecurityPolicy,  "s60");
_LIT(KMSASecurityPolicy,  "msa");
_LIT(KATTSecurityPolicy,  "att");
// Java Security Policy Central Repository UID
static const TUid KCRUidJavaSecurity = { 0x2001B289 };
// IDs for security folder and file keys
static const TUint32 KPolicy = 0x03;
static const TUint32 KSecurityWarnings = 0x04;
const int KS60SecurityPolicyId = 1;
const int KMSASecurityPolicyId = 2;
const int KATTSecurityPolicyId = 3;

using namespace java::storage;
using namespace std;
using namespace java::util;
using namespace java::comms;
using namespace java::captain;

void CJavaCaptainContainer::ConstructL(const TRect& aRect)
{
    JELOG2(EJavaCaptain);
    mCaptainRunning = true;
    mPrewarmEnabled = false;
#if defined(RD_JAVA_PREWARM) && !defined (__WINS__)
    mPrewarmEnabled = true;
#endif // RD_JAVA_PREWARM
    mPrewarmUiEnabled = false;
    mDebugEnabled = false;
    mRedirectEnabled = false;
    iStorage.reset(JavaStorage::createInstance());
    try
    {
        iStorage->open();
    }
    catch (JavaStorageException& aJse)
    {
        User::Leave(aJse.mStatus);
    }
    mUserSecurityModeEnabled = (GetSecurityWarningsMode() == SECURITY_WARNINGS_USER_DEFINED_MODE);
    mSecurityPolicyId = GetSecurityPolicy();
    mHeader = _L("Java states:");
    updateStates();
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    mRedirector.reset(new LogRedirector());
}

CJavaCaptainContainer::~CJavaCaptainContainer()
{
    JELOG2(EJavaCaptain);
}

void CJavaCaptainContainer::Draw(const TRect& /*aRect*/) const
{
    JELOG2(EJavaCaptain);
    CWindowGc& gc = SystemGc();
    gc.Clear();

    const CFont* usedFont = CEikonEnv::Static()->NormalFont();
    gc.UseFont(usedFont);

    gc.DrawText(mHeader,TPoint(10,20));
    int pos = 50;
    gc.DrawText(mCaptainState,TPoint(20,pos));
    int gap = 25;
    pos += gap;
    gc.DrawText(mPrewarmState,TPoint(20,pos));
    pos += gap;
    gc.DrawText(mPrewarmUiState,TPoint(20,pos));
    pos += gap;
    gc.DrawText(mDebugState,TPoint(20,pos));
    pos += gap;
    gc.DrawText(mSecurityMode,TPoint(20,pos));
    pos += gap;
    gc.DrawText(mSecurityPolicy,TPoint(20,pos));
    pos += gap;
    gc.DrawText(mRedirectState,TPoint(20,pos));

    gc.DiscardFont();
}

void CJavaCaptainContainer::updateStates()
{

    int propertyVal = java::util::JavaIniFileUtils::getProperty(java::util::UI_PREWARM);
    if (propertyVal != -1)
    {
        mPrewarmUiEnabled = propertyVal;
    }

    mCaptainState = _L("Captain running:               ");
    if (mCaptainRunning)
    {
        mCaptainState.Append(KJavaCapYes);
    }
    else
    {
        mCaptainState.Append(KJavaCapNo);
    }

    mPrewarmState = _L("Prewarm enabled:           ");
    if (mPrewarmEnabled)
    {
        mPrewarmState.Append(KJavaCapYes);
    }
    else
    {
        mPrewarmState.Append(KJavaCapNo);
    }

    mPrewarmUiState = _L("Prewarm UI enabled:      ");
    if (mPrewarmUiEnabled)
    {
        mPrewarmUiState.Append(KJavaCapYes);
    }
    else
    {
        mPrewarmUiState.Append(KJavaCapNo);
    }

    mDebugState = _L("Java Debug enabled:      ");
    if (mDebugEnabled)
    {
        mDebugState.Append(KJavaCapYes);
    }
    else
    {
        mDebugState.Append(KJavaCapNo);
    }

    mSecurityMode = _L("Security warnings:      ");
    if (mUserSecurityModeEnabled)
    {
        mSecurityMode.Append(KUserSecurityMode);
    }
    else
    {
        mSecurityMode.Append(KNormalSecurityMode);
    }

    mSecurityPolicy = _L("Security policy:      ");
    switch (mSecurityPolicyId)
    {
    case KS60SecurityPolicyId:
        mSecurityPolicy.Append(KS60SecurityPolicyName);
        break;
    case KMSASecurityPolicyId:
        mSecurityPolicy.Append(KMSASecurityPolicyName);
        break;
    case KATTSecurityPolicyId:
        mSecurityPolicy.Append(KATTSecurityPolicyName);
        break;
    default:
        mSecurityPolicy.Append(KS60SecurityPolicyName);
    }

    mRedirectState = _L("Redirect logs:                   ");
    if (mRedirectEnabled)
    {
        mRedirectState.Append(KJavaCapYes);
    }
    else
    {
        mRedirectState.Append(KJavaCapNo);
    }

    DrawDeferred();
}

void CJavaCaptainContainer::setCaptainState(bool running)
{
    mCaptainRunning = running;
    updateStates();
}



#include "commsmessage.h"
#include "commsclientendpoint.h"

void CJavaCaptainContainer::handleStateChangeCmd(int cmd)
{
    switch (cmd)
    {

    case ETogglePrewarmEnable:
    {
        mPrewarmEnabled = !mPrewarmEnabled;
        CommsMessage message;
        message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
        if (mPrewarmEnabled)
        {
            message.setMessageId(CORE_MSG_ID_START_PREWARM);
        }
        else
        {
            message.setMessageId(CORE_MSG_ID_STOP_PREWARM);
        }
        CommsClientEndpoint comms;
        comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        comms.send(message);
        comms.disconnect();
        break;
    }
    case ETogglePrewarmUiEnable:
        mPrewarmUiEnabled = !mPrewarmUiEnabled;
        java::util::JavaIniFileUtils::setProperty(java::util::UI_PREWARM, mPrewarmUiEnabled);
        break;

    case EDebugMode:
    {
        mDebugEnabled = !mDebugEnabled;

        CommsMessage message;
        std::wstring vmArgs = L"";
        if (mDebugEnabled)
        {
            vmArgs = L"-Xrunjdwp:server=y,address=localhost:8000";
        }
        setSetExtraArguments(message, vmArgs, L"");

        CommsClientEndpoint comms;
        comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        comms.send(message);
        comms.disconnect();
    }
    break;

    case ESecurityMode:
    {
        mUserSecurityModeEnabled = !mUserSecurityModeEnabled;

        if (mUserSecurityModeEnabled)
        {
            SetSecurityWarningsMode(SECURITY_WARNINGS_USER_DEFINED_MODE);
        }
        else
        {
            SetSecurityWarningsMode(SECURITY_WARNINGS_DEFAULT_MODE);
        }
    }
    break;
    case ES60SecurityPolicy:
        mSecurityPolicyId = KS60SecurityPolicyId;
        SetSecurityPolicy(mSecurityPolicyId);
        break;
    case EMSASecurityPolicy:
        mSecurityPolicyId = KMSASecurityPolicyId;
        SetSecurityPolicy(mSecurityPolicyId);
        break;
    case EATTSecurityPolicy:
        mSecurityPolicyId = KATTSecurityPolicyId;
        SetSecurityPolicy(mSecurityPolicyId);
        break;

    case ERedirectMode:
        mRedirectEnabled = !mRedirectEnabled;
        if (mRedirectEnabled)
        {
            mRedirector->start();
        }
        else
        {
            mRedirector->stop();
        }
        break;

    default:
        break;
    }
    updateStates();
}

std::wstring CJavaCaptainContainer::GetSecurityWarningsMode()
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        query.insert(attr);
        attr.setEntry(SECURITY_WARNINGS, L"");
        query.insert(attr);
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t domainQueryResult;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring securityWarnings = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, SECURITY_WARNINGS, securityWarnings);
            if (securityWarnings == SECURITY_WARNINGS_USER_DEFINED_MODE)
            {
                // make them all USER
                SetSecurityWarningsMode(SECURITY_WARNINGS_USER_DEFINED_MODE);
                return SECURITY_WARNINGS_USER_DEFINED_MODE;
            }
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
    // one mroe check: central repository
    std::wstring securityWarningsMode = SECURITY_WARNINGS_DEFAULT_MODE;
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        TInt warningsMode;
        User::LeaveIfError(repository->Get(KSecurityWarnings, warningsMode));
        switch (warningsMode)
{
case 1:
    securityWarningsMode = SECURITY_WARNINGS_USER_DEFINED_MODE;
    break;
case 2:
    securityWarningsMode = SECURITY_WARNINGS_DEFAULT_MODE;
    break;
}
CleanupStack::PopAndDestroy(repository););
    return securityWarningsMode;
}

void CJavaCaptainContainer::SetSecurityWarningsMode(const std::wstring& aSecurityWarningsMode)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        query.insert(attr);
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t domainQueryResult;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring uid = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, ID, uid);
            JavaStorageApplicationEntry_t oldEntry;
            JavaStorageEntry oldAttr;
            oldAttr.setEntry(ID, uid);
            oldEntry.insert(oldAttr);
            attr.setEntry(SECURITY_WARNINGS, aSecurityWarningsMode);
            entry.clear();
            entry.insert(attr);
            iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
    // set the cenRep key as well
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        if (aSecurityWarningsMode == SECURITY_WARNINGS_USER_DEFINED_MODE)
{
    repository->Set(KSecurityWarnings, 1);
    }
    else
    {
        repository->Set(KSecurityWarnings, 2);
    }
    CleanupStack::PopAndDestroy(repository);
    );
}

void  CJavaCaptainContainer::findColumn(const JavaStorageApplicationEntry_t& aEntry,
                                        const wstring& aColName, wstring& aColValue)
{
    JavaStorageEntry findPattern;
    findPattern.setEntry(aColName, L"");
    JavaStorageApplicationEntry_t::const_iterator findIterator =
        aEntry.find(findPattern);
    if (findIterator != aEntry.end())
    {
        aColValue = findIterator->entryValue();
    }
}


int CJavaCaptainContainer::GetSecurityPolicy()
{
    int securityPolicyId =KS60SecurityPolicyId;
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        HBufC* buf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
        TPtr policy(buf->Des());
        User::LeaveIfError(repository->Get(KPolicy, policy));
        if (policy.Compare(KS60SecurityPolicy) == 0)
{
    securityPolicyId = KS60SecurityPolicyId;
}
else if (policy.Compare(KMSASecurityPolicy) == 0)
{
    securityPolicyId = KMSASecurityPolicyId;
}
else if (policy.Compare(KATTSecurityPolicy) == 0)
{
    securityPolicyId = KATTSecurityPolicyId;
}
CleanupStack::PopAndDestroy(buf);
CleanupStack::PopAndDestroy(repository););
    return securityPolicyId;
}

void CJavaCaptainContainer::SetSecurityPolicy(int aSecurityPolicyId)
{
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidJavaSecurity);
        switch (aSecurityPolicyId)
{
case KS60SecurityPolicyId:
    User::LeaveIfError(repository->Set(KPolicy, KS60SecurityPolicy));
        break;
    case KMSASecurityPolicyId:
        User::LeaveIfError(repository->Set(KPolicy, KMSASecurityPolicy));
        break;
    case KATTSecurityPolicyId:
        User::LeaveIfError(repository->Set(KPolicy, KATTSecurityPolicy));
        break;
    }
    CleanupStack::PopAndDestroy(repository););
}

//
// CJavaCaptainAppUi
//
CJavaCaptainAppUi::~CJavaCaptainAppUi()
{
    JELOG2(EJavaCaptain);
    RemoveFromStack(iAppView);
    delete iAppView;
    delete mPeriodic;
}

void CJavaCaptainAppUi::ConstructL()
{
    JELOG2(EJavaCaptain);
    BaseConstructL();
    iAppView=new(ELeave) CJavaCaptainContainer;
    iAppView->SetMopParent(this);
    iAppView->ConstructL(ClientRect());

    iCoeEnv->RootWin().EnableFocusChangeEvents();
    mPeriodic=CPeriodic::NewL(CActive::EPriorityStandard);

    AddToStackL(iAppView);

}

_LIT(KExecutableName, "javacaptain.exe");

void senMsgToSplash(int id)
{
    CommsMessage message;
    message.setModuleId(1);
    message.setMessageId(id);
    CommsClientEndpoint comms;
    comms.connect(10900);
    comms.send(message);
    comms.disconnect();
}

TInt CJavaCaptainAppUi::tick(TAny* appUi)
{
    CJavaCaptainAppUi* app = static_cast<CJavaCaptainAppUi*>(appUi);
//    ELOG1(EJavaCaptain,"mActive: %d", app->mActive);
    if (app->mActive)
    {
        senMsgToSplash(1);
    }
    else
    {
        senMsgToSplash(2);
    }
    app->mActive = !app->mActive;
    return 0;
}

void CJavaCaptainAppUi::HandleCommandL(TInt aCommand)
{
    JELOG2(EJavaCaptain);

    switch (aCommand)
    {
    case EAknSoftkeyExit:
    case EEikCmdExit:
        Exit();
        break;
    case ECaptainStart:
    {
        RProcess proc;
        int st = proc.Create(KExecutableName, KExecutableName);
        if (st == KErrNone)
        {
            proc.Resume();
            iAppView->setCaptainState(true);
        }
        proc.Close();
    }
    break;

    case ECaptainStop:
    {
        CommsMessage message;
        message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
        message.setMessageId(CORE_MSG_ID_STOP_JAVACAPTAIN);
        CommsClientEndpoint comms;
        comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        comms.send(message);
        comms.disconnect();
        iAppView->setCaptainState(false);
    }
    break;

    case EThreadDump:
    {
        CommsMessage message;
        message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
        message.setMessageId(CORE_MSG_ID_DO_THREAD_DUMP);
        CommsClientEndpoint comms;
        comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        comms.send(message);
        comms.disconnect();
    }
    break;

    case ETogglePrewarmEnable:
    case ETogglePrewarmUiEnable:
    case EDebugMode:
    case ESecurityMode:
    case ES60SecurityPolicy:
    case EMSASecurityPolicy:
    case EATTSecurityPolicy:
    case ERedirectMode:
        iAppView->handleStateChangeCmd(aCommand);
        break;

    default:
        Exit();
        break;
    }
}

CEikAppUi* CJavaCaptainEikDocument::CreateAppUiL()
{
    JELOG2(EJavaCaptain);
    return(new(ELeave) CJavaCaptainAppUi);
}

void CJavaCaptainEikDocument::ConstructL()
{
    JELOG2(EJavaCaptain);
}

TUid CJavaCaptainApplication::AppDllUid() const
{
    JELOG2(EJavaCaptain);
    return KUidJavaCaptain;
}

CApaDocument* CJavaCaptainApplication::CreateDocumentL()
{
    JELOG2(EJavaCaptain);
    CJavaCaptainEikDocument* document=new(ELeave) CJavaCaptainEikDocument(*this);
    CleanupStack::PushL(document);
    document->ConstructL();
    CleanupStack::Pop();
    return(document);
}

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
{
    JELOG2(EJavaCaptain);
    return new CJavaCaptainApplication;
}


GLDEF_C TInt E32Main()
{
    JELOG2(EJavaCaptain);
    return EikStart::RunApplication(NewApplication);
}


#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

LogRedirector::LogRedirector() : mFd(-1)
{
    iJavaDiagnostic.reset(JavaDiagnostic::createInstance());
}

LogRedirector::~LogRedirector()
{
}

void LogRedirector::start()
{
    iJavaDiagnostic->setDiagnosticListener(*this);
    mFd = open("c:\\data\\javaredirect.log", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
}

void LogRedirector::stop()
{
    iJavaDiagnostic->removeDiagnosticListener();
    close(mFd);
    mFd = -1;
}

void LogRedirector::systemOut(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::systemErr(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::log(const TDesC8& aData)
{
    write(aData);
}

void LogRedirector::write(const TDesC8& aData)
{
    if (mFd > 0)
    {
        ::write(mFd, (const char*)aData.Ptr(), aData.Size());
    }
};



// End of File
