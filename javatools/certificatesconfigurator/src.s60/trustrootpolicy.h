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
#include <e32base.h>
#include <xml/contenthandler.h> // for MContentHandler
#include <xml/parser.h> // for CParser
#include <f32file.h>
#include <string>
#include <vector>

using namespace Xml;
using namespace std;

class TrustRoot
{
public:
    TrustRoot();

public:

    std::string iName;
    std::string iDomain;
    std::string iMcc;
    std::string iMnc;
    bool iCanDelete;
    bool iCanDisable;
};

class CTrustRootPolicy: public CBase, MContentHandler
{

public:

    static CTrustRootPolicy* NewL();

    virtual ~CTrustRootPolicy();

public:

    void ReadFromFileL(const TDesC& aFileName, vector<TrustRoot>& aTrustRoots);

private:

    CTrustRootPolicy();

    void ConstructL();

private: // from MContentHandler

    void OnStartDocumentL(const RDocumentParameters &aDocParam, TInt aErrorCode);

    void OnEndDocumentL(TInt aErrorCode);

    void OnStartElementL(const RTagInfo &aElement, const RAttributeArray &aAttributes,
                         TInt aErrorCode);

    void OnEndElementL(const RTagInfo &aElement, TInt aErrorCode);

    void OnContentL(const TDesC8 &aBytes, TInt aErrorCode);

    void OnStartPrefixMappingL(const RString &aPrefix, const RString &aUri,
                               TInt aErrorCode);

    void OnEndPrefixMappingL(const RString &aPrefix, TInt aErrorCode);

    void OnIgnorableWhiteSpaceL(const TDesC8 &aBytes, TInt aErrorCode);

    void OnSkippedEntityL(const RString &aName, TInt aErrorCode);

    void OnProcessingInstructionL(const TDesC8 &aTarget, const TDesC8 &aData,
                                  TInt aErrorCode);

    void OnError(TInt aErrorCode);

    TAny *GetExtendedInterface(const TInt32 aUid);

private:

    CParser*             iParser;
    RFs                  iRfs;
    vector<TrustRoot>*  iTrustRoots;
    string               iCurrentContent;
    TrustRoot           iTrustRoot;
};
