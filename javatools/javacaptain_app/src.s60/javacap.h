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

#ifndef JAVACAP_H
#define JAVACAP_H

#include <aknapp.h>
#include <aknappui.h>
#include <AknDoc.h>
#include <memory>
#include <f32file.h>
#include "javastorage.h"
#include "javadiagnostic.h"
using namespace java::debug;

const TUid KUidJavaCaptain={ 0x20022D3C };
class LogRedirector;

class CJavaCaptainContainer : public CCoeControl
{
public:
    void ConstructL(const TRect& aRect);
    ~CJavaCaptainContainer();
    static void ThreadMain(CJavaCaptainContainer*);
    void updateStates();
    void setCaptainState(bool running);
    void handleStateChangeCmd(int cmd);

private:
    void StartCaptain();
    void Draw(const TRect& aRect) const;
    std::wstring GetSecurityWarningsMode();
    void SetSecurityWarningsMode(const std::wstring& aSecurityWarningsMode);
    int GetSecurityPolicy();
    void SetSecurityPolicy(int);
    void findColumn(const java::storage::JavaStorageApplicationEntry_t& aEntry, const std::wstring& aColName, std::wstring& aColValue);


private:
    TBuf<20>        mHeader;
    TBuf<40>        mCaptainState;
    TBuf<40>        mPrewarmState;
    TBuf<40>        mPrewarmUiState;
    TBuf<40>        mDebugState;
    TBuf<40>        mSecurityMode;
    TBuf<40>        mSecurityPolicy;
    TBuf<40>        mRedirectState;

    bool            mCaptainRunning;
    bool            mPrewarmEnabled;
    bool            mPrewarmUiEnabled;
    bool            mDebugEnabled;
    bool            mUserSecurityModeEnabled;
    int             mSecurityPolicyId;
    bool            mRedirectEnabled;

    std::auto_ptr<java::storage::JavaStorage> iStorage;
    std::auto_ptr<LogRedirector> mRedirector;
};

class CJavaCaptainEikDocument : public CAknDocument
{
public:
    void ConstructL();
    CJavaCaptainEikDocument(CEikApplication& aApp): CAknDocument(aApp) { }
    ~CJavaCaptainEikDocument() {}
private:
    // from CAknDocument
    CEikAppUi* CreateAppUiL();
};

class CJavaCaptainAppUi : public CAknAppUi
{
public:
    void ConstructL();
    ~CJavaCaptainAppUi();

public:
    static TInt tick(TAny* splash);

private:
    void HandleCommandL(TInt aCommand);
    CJavaCaptainContainer* iAppView;
    bool                   mActive;
    CPeriodic*             mPeriodic;
};

class CJavaCaptainApplication : public CAknApplication
{
private:
    // from CApaApplication
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
};

class LogRedirector : public DiagnosticListener
{
public:
    LogRedirector();
    ~LogRedirector();

    void start();
    void stop();

    // DiagnosticListener methods
    virtual void systemOut(const TDesC8& aData);
    virtual void systemErr(const TDesC8& aData);
    virtual void log(const TDesC8& aData);

private:
    void write(const TDesC8& aData);
    std::auto_ptr<java::debug::JavaDiagnostic> iJavaDiagnostic;

    int mFd;
};

#endif // JAVACAP_H

