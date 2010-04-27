/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <AiwGenericParam.h>
#include <DocumentHandler.h>
#include "swtlaffacade.h"
#include "swtbrowseraputils.h"
#include "swtbrowserschemehandler.h"
#include "browserconnection.h"
#include "swtbrowserspecialloadobserver.h"
#include "swtbrowser.h"
#include "swtbrowseractiveapdb.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserSpecialLoadObserver::CSwtBrowserSpecialLoadObserver(CSwtBrowser* aBrowser)
        : iBrowser(aBrowser)
{
}

void CSwtBrowserSpecialLoadObserver::ConstructL()
{
    iHandler = CDocumentHandler::NewL();
    iHandler->SetExitObserver(this);
}

CSwtBrowserSpecialLoadObserver* CSwtBrowserSpecialLoadObserver::NewL(CSwtBrowser* aBrowser)
{
    CSwtBrowserSpecialLoadObserver* self = new(ELeave)CSwtBrowserSpecialLoadObserver(aBrowser);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CSwtBrowserSpecialLoadObserver::~CSwtBrowserSpecialLoadObserver()
{
    delete iHandler;
    delete iSchemeHandler;
}

void CSwtBrowserSpecialLoadObserver::NetworkConnectionNeededL(TInt* aConnectionPtr,
        TInt* aSockSvrHandle, TBool* aNewConn, TApBearerType* aBearerType)
{
    if (iBrowser->Connection().Connected())
    {
        // Current connection is good
        // Update connection at caller side
        *aConnectionPtr = reinterpret_cast < TInt >(&iBrowser->Connection().Connection());
        *aSockSvrHandle = iBrowser->Connection().SocketServer().Handle();
        *aNewConn = EFalse;
        *(reinterpret_cast< TInt* >(aBearerType))
        = iBrowser->Connection().CurrentBearerType();
    }
    else
    {
        // Currently not connected
        *aNewConn = ETrue;
        TBool isValid(EFalse);
        TUint32 ap(0);
        TInt err(KErrNone);
        TBool wins(EFalse);

#ifdef __WINS__
        wins = ETrue;
#endif  // __WINS__

        if (wins && (iBrowser->Preferences().AccessPointSelectionMode()
                     == EWmlSettingsAccessPointSelectionModeAlwaysAsk))
        {
            // On Wins, if always ask mode is the case, start dialog
            isValid = ETrue;  // LAN connection dialog will come anyway
        }
        else
        {

            TBool continueCheck(ETrue);
            if (iBrowser->Preferences().AccessPointSelectionMode()
                    == EWmlSettingsAccessPointSelectionModeAlwaysAsk
                    || (iBrowser->RequestedAp() == KWmlNoDefaultAccessPoint
                        && iBrowser->Preferences().DefaultAccessPoint()
                        == KWmlNoDefaultAccessPoint))
            {
                iBrowser->Preferences().SetAccessPointSelectionModeL(
                    EWmlSettingsAccessPointSelectionModeAlwaysAsk);
                isValid = ETrue;
                continueCheck = EFalse;
            }


            if (continueCheck)
            {

                if (iBrowser->RequestedAp() == KWmlNoDefaultAccessPoint)
                {
                    iBrowser->SetRequestedAP(iBrowser->Preferences().DefaultAccessPoint());
                }

                // Check user defined Ap's validity convert it IAP
                ASSERT(iBrowser->RequestedAp() != KWmlNoDefaultAccessPoint);

                CCommsDatabase& db = iBrowser->ActiveApDb().CommsDb();
                CSwtBrowserApUtils* apUtils = CSwtBrowserApUtils::NewL(db);
                CleanupStack::PushL(apUtils);
                TRAP(err, ap = apUtils->IapIdFromWapIdL(iBrowser->RequestedAp()));
                CleanupStack::PopAndDestroy(apUtils);
                if (err == KErrNone)
                {
                    // user defined Ap is good, then fine
                    isValid = ETrue;
#ifdef __WINS__
                    // On WINS no need to convert the AP ID
                    // go back to unconverted ap id
                    ap = iBrowser->RequestedAp();
#endif // __WINS__
                }
                else
                {
                    // defined ap is invalid
                    // enable alwasy ask mode
                    iBrowser->SetRequestedAP(KWmlNoDefaultAccessPoint);
                    iBrowser->Preferences().SetAccessPointSelectionModeL(
                        EWmlSettingsAccessPointSelectionModeAlwaysAsk);
                    isValid = ETrue;
                }
            }
        }

        if (isValid)
        {
            // If AP is valid, start connection
            if (iBrowser->Preferences().AccessPointSelectionMode()
                    == EWmlSettingsAccessPointSelectionModeAlwaysAsk)
            {
                iBrowser->Connection().SetRequestedAP(ap);
            }
            else
            {
                iBrowser->Connection().SetRequestedAP(NULL);
            }
            err = iBrowser->Connection().StartConnectionL();

            if (err == KErrNone)
            {
                // Update connection at caller side after connection has been done
                *aConnectionPtr =
                    reinterpret_cast < TInt >(&iBrowser->Connection().Connection());
                *aSockSvrHandle = iBrowser->Connection().SocketServer().Handle();
                *(reinterpret_cast< TInt* >(aBearerType))
                = iBrowser->Connection().CurrentBearerType();

                // update ap on preferences
                if (iBrowser->Preferences().AccessPointSelectionMode()
                        == EWmlSettingsAccessPointSelectionModeAlwaysAsk)
                {
                    TUint32 IapId = iBrowser->Connection().CurrentAPId();
                    if (IapId)
                    {
                        TUint32 wapId = IapId;
#ifndef __WINS__
                        CSwtBrowserApUtils* apUtils = CSwtBrowserApUtils::NewL(iBrowser->ActiveApDb().CommsDb());
                        CleanupStack::PushL(apUtils);
                        wapId = apUtils->WapIdFromIapIdL(IapId);
                        CleanupStack::PopAndDestroy(apUtils);
#endif // __WINS__

                        if (wapId)
                        {
                            if (iBrowser->Preferences().DefaultAccessPoint()
                                    == KWmlNoDefaultAccessPoint)
                            {
                                TRAP_IGNORE(iBrowser->Preferences().SetDefaultAccessPointL(wapId));
                            }
                            if (iBrowser->Preferences().DefaultAccessPoint()
                                    == KWmlNoDefaultAccessPoint)
                            {
                                iBrowser->SetRequestedAP(wapId);
                            }
                            iBrowser->Preferences().SetAccessPointSelectionModeL(
                                EWmlSettingsAccessPointSelectionModeUserDefined);
                        }
                    }
                }
            }

            // Leave if there is still a error
            User::LeaveIfError(err);
        }
        else // Ap is not valid
        {
            // User::LeaveIfError( err );
            User::Leave(KErrCouldNotConnect);
        }
    }
}

TBool CSwtBrowserSpecialLoadObserver::HandleRequestL(RArray<TUint>* aTypeArray,
        CDesCArrayFlat* aDesArray)
{
    TInt i;
    // The 2 arrays must be in sync. Each element in iTypeArray
    // identifies the type of the corresponding element in iDesArray.
    if (aTypeArray->Count() != aDesArray->Count())
    {
        User::Leave(KErrArgument);
    }

    // Get url
    HBufC* url = NULL;
    // Search url in the param list
    for (i = 0; i < aTypeArray->Count(); i++)
    {
        if ((*aTypeArray)[i] == EParamRequestUrl)
        {
            // the url is found
            url = HBufC::NewLC((*aDesArray)[i].Length());
            url->Des().Copy((*aDesArray)[i]);
            break;
        }
    }
    if (!url)
    {
        // The url wasn't found.
        User::Leave(KErrArgument);
    }

    // Get referrer header
    HBufC* refererHeader = NULL;
    // Search url in the param list
    for (i = 0; i < aTypeArray->Count(); i++)
    {
        if ((*aTypeArray)[i] == EParamRefererHeader)
        {
            // the referer Header is found
            refererHeader = HBufC::NewLC((*aDesArray)[i].Length());
            refererHeader->Des().Copy((*aDesArray)[i]);
            break;
        }
    }

    // Here, dispatching the request to the correct component according to refererHeader

    if (refererHeader)
    {
        CleanupStack::PopAndDestroy(refererHeader);   // refererHeader
    }

    CleanupStack::PopAndDestroy(url);
    return ETrue;
}

TBool CSwtBrowserSpecialLoadObserver::HandleDownloadL(RArray<TUint>* aTypeArray,
        CDesCArrayFlat* aDesArray)
{
    TBool paramFound(EFalse);
    TPtrC fileName = ExtractBrCtlParam(
                         EParamLocalFileName
                         , aTypeArray
                         , aDesArray
                         , paramFound);

#if defined( _DEBUG )
    _LIT(KAppName, "eSwtBrowser");
#endif

    __ASSERT_DEBUG(paramFound, User::Panic(KAppName, 0));

    TPtrC contentType = ExtractBrCtlParam(
                            EParamReceivedContentType
                            , aTypeArray
                            , aDesArray
                            , paramFound);
    HBufC8* contentType8 = 0;
    if (!paramFound)
    {
        contentType8 = KNullDesC8().AllocLC();
    }
    else
    {
        contentType8 = HBufC8::NewLC(contentType.Length());
        // 16 bit buffer copied into 8 bit buffer.
        contentType8->Des().Copy(contentType);
    }
    TDataType dataType(*contentType8);

    CAiwGenericParamList* genericParamList =
        BrCtlParamList2GenericParamListL(aTypeArray, aDesArray);
    CleanupStack::PushL(genericParamList);

    // File is not yet saved to target folder.
    TBool allowSave(ETrue);
    TAiwVariant allowSaveVariant(allowSave);
    TAiwGenericParam genericParamAllowSave(
        CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamAllowSave),
        allowSaveVariant);
    genericParamList->AppendL(genericParamAllowSave);


    // Set EGenericParamAllowMove - we encourage handlers to apply
    // move instead of copy for the file.
    TBool allowMove(ETrue);
    TAiwVariant allowMoveVariant(allowMove);
    TAiwGenericParam genericParamAllowMove(
        CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamAllowMove),
        allowMoveVariant);
    genericParamList->AppendL(genericParamAllowMove);

    RFile tempFile;
    iHandler->OpenTempFileL(fileName, tempFile);
    CleanupClosePushL(tempFile);
    iHandler->OpenFileEmbeddedL(tempFile
                                , dataType, *genericParamList);
    CleanupStack::PopAndDestroy(&tempFile);   // tempFile

    CleanupStack::PopAndDestroy(genericParamList);   // genericParamList
    CleanupStack::PopAndDestroy(contentType8);   // contentType8

    return EFalse;
}

void CSwtBrowserSpecialLoadObserver::HandleServerAppExit(TInt /*aReason*/)
{
    iSchemeProcessing = EFalse;
}

TBool CSwtBrowserSpecialLoadObserver::DoHandleRequestL
(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray)
{
    if (!iSchemeProcessing)
    {
        iSchemeProcessing = ETrue;
        TBool paramFound(EFalse);
        TPtrC requestUrl = ExtractBrCtlParam(EParamRequestUrl,
                                             aTypeArray, aDesArray, paramFound);

#if defined( _DEBUG )
        _LIT(KAppName, "eSwtBrowser");
        __ASSERT_DEBUG(paramFound, User::Panic(KAppName, 0));
#endif

        _LIT(KSavedPage,  "savedpage://");
        if ((requestUrl.Find(KSavedPage) == KErrNone))
        {
            TInt len = requestUrl.Length() - KSavedPage().Length();
            HBufC* buf = HBufC::NewLC(len);
            TPtr des = buf->Des();
            des.Copy(requestUrl.Right(requestUrl.Length()-KSavedPage().Length()));
            des.TrimRight();
            TLex16 lex(des) ;
            TInt uid;
            lex.Val(uid);
            CleanupStack::PopAndDestroy(buf);
            iBrowser->FetchSavedDeckL(uid);
            iSchemeProcessing = EFalse;
        }
        else
        {
            // pass to the CSchemaHandler
            CSwtBrowserSchemeHandler* newSH = CSwtBrowserSchemeHandler::NewL(requestUrl);
            delete iSchemeHandler;
            iSchemeHandler = newSH;

            // construct the current accesspoint
            // if it's connected, always add it
            if (iBrowser->Connection().Connected())
            {
                CAiwGenericParamList* paramsList = CAiwGenericParamList::NewL();
                CleanupStack::PushL(paramsList);
                TInt32 ap = iBrowser->Connection().CurrentAPId();
                TAiwVariant paramVariant(ap);
                TAiwGenericParam genericParam(
                    CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamAccessPoint),
                    paramVariant);
                paramsList->AppendL(genericParam);
                CleanupStack::PopAndDestroy(paramsList);
                iSchemeHandler->SetParameterList(paramsList);
            }

            iSchemeHandler->Observer(this);
            iSchemeHandler->HandleUrlEmbeddedL();
        }
    }


    return ETrue;
}

CAiwGenericParamList* CSwtBrowserSpecialLoadObserver::BrCtlParamList2GenericParamListL(
    RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray) const
{

    //Creating generic param list
    CAiwGenericParamList* genericParamList = CAiwGenericParamList::NewLC();

    for (TInt j = 0; j < aTypeArray->Count(); j++)
    {
        TBool paramFound(EFalse);
        const TUint paramType = (*aTypeArray)[j];
        TPtrC param = ExtractBrCtlParam(
                          paramType
                          , aTypeArray
                          , aDesArray
                          , paramFound);
#if defined( _DEBUG )
        _LIT(KAppName, "eSwtBrowser");
#endif
        __ASSERT_DEBUG(paramFound, User::Panic(KAppName, 0));

        switch (paramType)
        {
        case EParamRequestUrl:
        {
            TAiwVariant paramVariant(param);
            TAiwGenericParam genericParam(
                CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamURL),
                paramVariant);
            genericParamList->AppendL(genericParam);

            break;
        }
        case EParamReceivedContentType:
        {
            // The expected content type should override the response header's
            // content type => don't use EParamReceivedContentType if
            // EParamExpectedContentType is set
            TBool expectedContentTypeFound(EFalse);
            ExtractBrCtlParam(
                EParamExpectedContentType
                , aTypeArray
                , aDesArray
                , expectedContentTypeFound);
            if (!expectedContentTypeFound)
            {
                // Using EParamReceivedContentType
                TAiwVariant paramVariant(param);
                TAiwGenericParam genericParam(
                    CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamMIMEType),
                    paramVariant);
                genericParamList->AppendL(genericParam);
            }
            break;
        }
        case EParamExpectedContentType:
        {
            // Overrides EParamReceivedContentType
            TAiwVariant paramVariant(param);
            TAiwGenericParam genericParam(
                CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamMIMEType),
                paramVariant);
            genericParamList->AppendL(genericParam);
            break;
        }
        case EParamLocalFileName:
        {
            TAiwVariant paramVariant(param);
            TAiwGenericParam genericParam(EGenericParamFile,
                                          paramVariant);
            genericParamList->AppendL(genericParam);
            break;
        }
        case EParamCharset:
        {
            TAiwVariant paramVariant(param);
            TAiwGenericParam genericParam(
                CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamCharSet),
                paramVariant);
            genericParamList->AppendL(genericParam);
            break;
        }
        case EParamRefererHeader:
        {
            TAiwVariant paramVariant(param);
            TAiwGenericParam genericParam(
                CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamReferringURI),
                paramVariant);
            genericParamList->AppendL(genericParam);
            break;
        }
        default:
        {
            break;
        }
        }
    }

    // Always add the access point, if currently connected
    if (iBrowser->Connection().Connected())
    {
        TInt32 ap = iBrowser->Connection().CurrentAPId();
        TAiwVariant paramVariant(ap);
        TAiwGenericParam genericParam(
            CSwtLafFacade::GetConstant(CSwtLafFacade::ESwtBrowserGenericParamAccessPoint),
            paramVariant);
        genericParamList->AppendL(genericParam);
    }

    CleanupStack::Pop(genericParamList);   // genericParamList
    return genericParamList;
}

TPtrC CSwtBrowserSpecialLoadObserver::ExtractBrCtlParam
(TUint aParamTypeToFind
 , RArray<TUint>* aTypeArray
 , CDesCArrayFlat* aDesArray
 , TBool& aParamFound) const
{

    // initialize output parameter
    aParamFound = EFalse;
    TPtrC16 retParamValue;

    for (TInt j = 0; j < aTypeArray->Count(); j++)
    {
        const TUint paramType = (*aTypeArray)[j];
        if (aParamTypeToFind == paramType)
        {
            // That's we need
            retParamValue.Set(aDesArray->MdcaPoint(j));
            aParamFound = ETrue; // Indicate it in the out param
            break; // break the loop - we found it
        }
    }

    return retParamValue;
}
