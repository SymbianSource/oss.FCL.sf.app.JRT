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
* Description:
*
*/
#include "trustrootpolicy.h"

_LIT8(KXmlMimeType, "text/xml");

// tag names
_LIT8(KTrustRoot, "trustroot");
_LIT8(KName, "name");
_LIT8(KDomain, "domain");
_LIT8(KMcc, "mcc");
_LIT8(KMnc, "mnc");
_LIT8(KDisable, "disable");
_LIT8(KDelete, "delete");

CTrustRootPolicy* CTrustRootPolicy::NewL()
{
    CTrustRootPolicy* self = new(ELeave) CTrustRootPolicy();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CTrustRootPolicy::~CTrustRootPolicy()
{
    iRfs.Close();
    if (iParser)
    {
        delete iParser;
        iParser = NULL;
    }
}

CTrustRootPolicy::CTrustRootPolicy()
{
}

void CTrustRootPolicy::ConstructL()
{
    User::LeaveIfError(iRfs.Connect());
    iParser = CParser::NewL(KXmlMimeType, *this);
}

void CTrustRootPolicy::ReadFromFileL(const TDesC& aPolicyFileName, vector<TrustRoot>& aTrustRoots)
{

    RFile file;
    TInt size;
    User::LeaveIfError(file.Open(iRfs, aPolicyFileName, EFileRead));
    User::LeaveIfError(file.Size(size));

    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 bufferPtr(buffer->Des());
    User::LeaveIfError(file.Read(bufferPtr));

    iTrustRoots = &aTrustRoots;
    iParser->ParseBeginL();
    iParser->ParseL(*buffer);
    iParser->ParseEndL();

    CleanupStack::PopAndDestroy(); // buffer
}

void CTrustRootPolicy::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnEndDocumentL(TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/, TInt /*aErrorCode*/)
{
    iCurrentContent.clear();
    const TDesC8& tagName = aElement.LocalName().DesC();
    if (tagName.Compare(KTrustRoot) == 0)
    {
        iTrustRoot = TrustRoot();
    }
}

void CTrustRootPolicy::OnEndElementL(const RTagInfo& aElement, TInt /*aErrorCode*/)
{
    const TDesC8& tagName = aElement.LocalName().DesC();
    if (tagName.Compare(KTrustRoot) == 0)
    {
        iTrustRoots->push_back(iTrustRoot);
    }
    else if (tagName.Compare(KName) == 0)
    {
        iTrustRoot.iName = iCurrentContent;
    }
    else if (tagName.Compare(KDomain) == 0)
    {
        iTrustRoot.iDomain = iCurrentContent;
    }
    else if (tagName.Compare(KMcc) == 0)
    {
        iTrustRoot.iMcc = iCurrentContent;
    }
    else if (tagName.Compare(KMnc) == 0)
    {
        iTrustRoot.iMnc = iCurrentContent;
    }
    else if (tagName.Compare(KDisable) == 0)
    {
        iTrustRoot.iCanDisable = true;
    }
    else if (tagName.Compare(KDelete) == 0)
    {
        iTrustRoot.iCanDelete = true;
    }
}

void CTrustRootPolicy::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
{
    // append aBytes to iCurrentContent
    iCurrentContent.append((const char*)aBytes.Ptr(), aBytes.Length());
}

void CTrustRootPolicy::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/,TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnSkippedEntityL(const RString& /*aName*/,TInt /*aErrorCode*/)
{
}
void CTrustRootPolicy::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/)
{
}

void CTrustRootPolicy::OnError(TInt /*aErrorCode*/)
{
}

TAny* CTrustRootPolicy::GetExtendedInterface(const TInt32 /*aUid*/)
{
    return 0;
}

TrustRoot::TrustRoot()
{
    iName = "";
    iDomain = "";
    iMcc = "";
    iMnc = "";
    iCanDelete = false;
    iCanDisable = false;
}
