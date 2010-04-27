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
* Description:
 *
*/


// INCLUDE FILES
#include "cstsfiledatamanager.h"
#include <JavaUtils.h>

#include <utf.h>
#include "cstsauthtype.h"
#include "cstspinattributes.h"

namespace java
{
namespace satsa
{

// CONSTANTS
//max size got from PKCS#15 standard, TokenInfo.label field description
const TInt KSTSWIMLabelMaxLength = 255;
const TInt KSTSWIMPathMaxLength = 6;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSFileDataManager::CSTSFileDataManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSFileDataManager::CSTSFileDataManager() :
        CActive(EPriorityNormal), iWimLabelAndPathDone(EFalse)
{
    CActiveScheduler::Add(this);
    iState = ENotReady;
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSFileDataManager::ConstructL()
{
    iWait = new(ELeave) CActiveSchedulerWait;
    iMidpProv = CWimJavaUtils::NewL();
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSFileDataManager* CSTSFileDataManager::NewL()
{
    CSTSFileDataManager* self = new(ELeave) CSTSFileDataManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSFileDataManager::~CSTSFileDataManager()
{

    delete iMidpProv;
    delete iWait;

    delete iACFData;
    delete iACIFData;
    delete iWimPath;
    delete iWimLabel16;

}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::InitializeL
//
// -----------------------------------------------------------------------------
void CSTSFileDataManager::InitializeL()
{
    iMidpProv->Initialize(iStatus);
    iState = EInitializing;
    WaitAndCheckL();
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RetrieveACIFContentL
//
// -----------------------------------------------------------------------------
const TDesC8& CSTSFileDataManager::RetrieveACIFContentL()
{
    TInt fileSize = 0;
    iMidpProv->GetACIFSize(fileSize, iStatus);
    iState = EGettingACIFSize;
    WaitAndCheckL();

    //allocate buffer for file data
    HBufC8* tmp = HBufC8::NewL(fileSize);
    delete iACIFData;
    iACIFData = tmp;

    TPtr8 wholeData(iACIFData->Des());

    iMidpProv->RetrieveACIFContent(wholeData, iStatus);
    iState = ERetrievingACIF;
    WaitAndCheckL();

    return *iACIFData;

}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RetrieveACFContentL
//
// -----------------------------------------------------------------------------
const TDesC8& CSTSFileDataManager::RetrieveACFContentL(
    const TDesC8& aFileIdOrPath)
{
    TInt fileSize = 0;
    iMidpProv->GetACFSize(fileSize, aFileIdOrPath, iStatus);
    iState = EGettingACFSize;
    WaitAndCheckL();
    //allocate buffer for file data
    HBufC8* tmp = HBufC8::NewL(fileSize);
    delete iACFData;
    iACFData = tmp;

    TPtr8 wholeData(iACFData->Des());

    iMidpProv->RetrieveACFContent(wholeData, aFileIdOrPath, iStatus);
    iState = ERetrievingACF;
    WaitAndCheckL();

    return *iACFData;

}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::GetAuthObjsInfoL
//
// -----------------------------------------------------------------------------
void CSTSFileDataManager::GetAuthObjectsL(CArrayPtr<CSTSAuthType>* aAuthTypes,
        const RArray<TInt>& aAuthIdList)
{
    TInt auhtIdCount = aAuthIdList.Count();

    if (auhtIdCount > 0)
    {

        RArray<TJavaPINParams> authObjsInfoList(auhtIdCount);

        iMidpProv->GetAuthObjsInfo(aAuthIdList, authObjsInfoList, iStatus);
        iState = EGettingAuthObjsInfo;
        WaitAndCheckL();

        //make conversion to own struct
        TInt count = authObjsInfoList.Count();

        for (TInt i = 0; i < count; i++)
        {
            //creates authType and add values
            CSTSAuthType* currentObject = CSTSAuthType::NewLC();
            currentObject->SetAuthIDL(authObjsInfoList[i].iAuthId);
            currentObject->SetLabelL(authObjsInfoList[i].iPINLabel);

            CSTSPinAttributes* pinAttr = CSTSPinAttributes::NewLC(
                                             authObjsInfoList[i].iPinType,
                                             authObjsInfoList[i].iMinLength,
                                             authObjsInfoList[i].iStoredLength,
                                             authObjsInfoList[i].iMaxLength,
                                             authObjsInfoList[i].iPinReference,
                                             authObjsInfoList[i].iPadChar, authObjsInfoList[i].iFlags);

            currentObject->SetPinAttributesL(*pinAttr);

            CleanupStack::PopAndDestroy(pinAttr);

            //append authType to the parameter array (ownership transferred)
            aAuthTypes->AppendL(currentObject);
            CleanupStack::Pop(currentObject);

        }

        authObjsInfoList.Close();

    }

}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RetrieveWIMLabelL
//
// -----------------------------------------------------------------------------
const TDesC& CSTSFileDataManager::RetrieveWIMLabelL()
{
    doRetrieveWIMLabelAndPathL();
    return *iWimLabel16;

}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RetrieveWIMPathL
//
// -----------------------------------------------------------------------------
const TDesC8& CSTSFileDataManager::RetrieveWIMPathL()
{
    doRetrieveWIMLabelAndPathL();
    return *iWimPath;
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::WaitAndCheckL
//
// -----------------------------------------------------------------------------
void CSTSFileDataManager::WaitAndCheckL()
{

    SetActive();

    if (!iWait->IsStarted())
    {
        iWait->Start();
    }
    User::LeaveIfError(iError);
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::doRetrieveWIMLabelAndPathL
// Retrieves WIM label and path and saves values to member variables.
// Saves information that retrieve is done and does not do retrieve if it is
// already done.
// -----------------------------------------------------------------------------
void CSTSFileDataManager::doRetrieveWIMLabelAndPathL()
{
    if (!iWimLabelAndPathDone)
    {
        //allocate buffer for WIM label
        HBufC8* wimLabelBuf = HBufC8::NewLC(KSTSWIMLabelMaxLength);
        TPtr8 wimLabel(wimLabelBuf->Des());

        //allocate buffer for WIM path
        HBufC8* tmpPath = HBufC8::NewL(KSTSWIMPathMaxLength);
        delete iWimPath;
        iWimPath = tmpPath;
        TPtr8 wimPath(iWimPath->Des());

        iMidpProv->RetrieveWimLabelAndPath(wimLabel, wimPath, iStatus);
        iState = EGettingWimLabelAndPath;
        WaitAndCheckL();

        //convert label to TDesC format
        //allocate member buffer for WIM label
        HBufC16* tmpLabel16 = HBufC16::NewL(wimLabel.Length());
        delete iWimLabel16;
        iWimLabel16 = tmpLabel16;
        TPtr16 label16(iWimLabel16->Des());

        TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(label16, wimLabel);
        User::LeaveIfError(err);
        CleanupStack::PopAndDestroy(wimLabelBuf);
        //save information that both has retrieved
        iWimLabelAndPathDone = ETrue;
    }
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSFileDataManager::RunL()
{
    iError = iStatus.Int();
    iState = EReady;
    iWait->AsyncStop();
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::DoCancel
// Cancels all possible active actions
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSFileDataManager::DoCancel()
{
    switch (iState)
    {
    case EInitializing:
    {
        iMidpProv->CancelInitialize();
        break;
    }
    case EGettingACIFSize:
    {
        iMidpProv->CancelGetACIFSize();
        break;
    }
    case ERetrievingACIF:
    {
        iMidpProv->CancelRetrieveACIFContent();
        break;
    }
    case EGettingACFSize:
    {
        iMidpProv->CancelGetACFSize();
        break;
    }
    case ERetrievingACF:
    {
        iMidpProv->CancelRetrieveACFContent();
        break;
    }
    case EGettingAuthObjsInfo:
    {
        iMidpProv->CancelGetAuthObjsInfo();
        break;
    }
    case EGettingWimLabelAndPath:
    {
        iMidpProv->CancelRetrieveWimLabelAndPath();
        break;
    }

    default:
    {
        // nothing to cancel
    }
    }
    iState = EReady;
    iError = KErrCancel;
}

// -----------------------------------------------------------------------------
// CSTSFileDataManager::RunError
// If leave occures in RunL,this method is called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSTSFileDataManager::RunError(TInt /*aError*/)
{
    //must return KErrNone
    return KErrNone;
}

} // namespace satsa
} // namespace java
//  End of File
