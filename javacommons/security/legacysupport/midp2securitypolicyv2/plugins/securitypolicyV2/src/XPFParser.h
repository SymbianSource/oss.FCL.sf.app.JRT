/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 *
 * @file    XPFParser.h
 *
 * @internalComponent
 *
 */

#ifndef XPFPARSER_H
#define XPFPARSER_H

#include <e32base.h>
#include "j2me/midp2/security/MDriveAccessPolicy.h"




namespace MIDP
{
class CFileSystemAccessPolicy;
class CPathAccessPolicy;
class CFunctionGroup;
class CFunctionGroupBinding;
class CProtectionDomain;
class CSecurityPolicy;
class RUTF8File;
class CBlanketPermissionPolicy;
class CBlanketPermissionPolicyElement;

class CXPFParser: public CBase
{
public:

    static CXPFParser* NewLC(RFile&, CSecurityPolicy&);

public:

    virtual ~CXPFParser();

    void ParseL(void);

private:

    enum TToken
    {
        ENoToken = 0,
        EAny,
        EEndOfFile,
        EEndOfLine,
        EString,
        ESymbol,
        EComment,
        ECommentOrNewline,
        EDomainName,
        EFunctionGroupName,
        EInteractionMode,
        EPathAccessMode,
        EPermissionName,
        EPermissionType
    };

    enum TDeclarationType
    {
        ENoType = 0,
        EDomain,
        EUntrustedDomain,
        EFunctionGroup,
        EPermission,
        EFunctionGroupBinding
    };

private:

    CXPFParser(RFile&, CSecurityPolicy&);

    void ConstructL(void);

    void  DomainsL(void);
    void  DomainL(void);
    void  ConnectionFiltersL(void);
    void  ConnectionFilterL(void);
    void  FileAccessL();
    void  DriveListL(CFileSystemAccessPolicy&);
    TInt  DriveTokenL(void);
    void  PathAccessL(CPathAccessPolicy&);
    void  PathMappingL(CPathAccessPolicy&);
    void  FunctionGroupsL(void);
    void  FunctionGroupL(void);
    void  BindingsL(void);
    void  DomainBindingsL(void);
    TBool FunctionGroupBindingL(void);
    void  RuleL(CFunctionGroupBinding&);

    MPermission::TMode InteractionModeL();

    void MiscL(void);
    void NewLineL(void);
    void NewLinePlusL(void);

    void SkipWhitespace(void);

    void EnsureL(const TDesC&, const TDesC&);
    void EnsureTokenL(const TDesC&);

    TToken TokenL(TToken aExpected = EAny);

    HBufC* StringLC(void);

    TInt ReadLineL(void);

    TInt ReadLineL(HBufC*& aBuffer);
    TInt ReadChar(TUint16& aChar);
    TInt Read(TUint8& aByte);

    CFunctionGroup*         FindFunctionGroup(const TDesC&);
    CFunctionGroupBinding*  FindFunctionGroupBinding(const TDesC&);

    void AddDefaultBindingsL(
        MPermission::TType,
        MPermission::TMode,
        MPermission::TMode);

    void AddDefaultBindingsL(
        MPermission::TType aPermissionType,
        MPermission::TMode aDefaultModes,
        TUint32            aInteractionModeMap);

    void DuplicateDeclarationL(TDeclarationType, const TDesC&);
    void UndeclaredL(TDeclarationType, const TDesC&);
    void UnexpectedL(const TDesC& aExpected, const TDesC& aActual);
    void UnexpectedL(TToken aExpected, const TDesC& aActual);
    void UnexpectedL(TToken aExpected, TToken aActual);

    void ErrorL(const TDesC&, const TDesC&, const TDesC&);

    const TDesC& ToString(TDeclarationType);
    const TDesC& ToString(TToken);

    void BlanketPermissionL(void);
    void ExclusionSetL(CBlanketPermissionPolicy* aBpp);

private:

    RFile&                                  iFile;
    HBufC8*                                 iFileBuffer;
    TInt                                    iFileBufferLength;
    TInt                                    iFileBufferOffset;
    CSecurityPolicy&                        iPolicy;
    HBufC*                                  iBuffer;
    TPtrC                                   iPtr;
    TPtrC                                   iToken;
    TBool                                   iNextLine;
    TInt                                    iLineLength;
    TUint32                                 iLineNumber;
    TInt                                    iLineOffset;
    CProtectionDomain*                      iCurrentDomain;
    CProtectionDomain*                      iUntrustedDomain;
    RPointerArray<CFunctionGroup>           iFunctionGroups;
    RPointerArray<CFunctionGroupBinding>    iFunctionGroupBindings;
    HBufC*                                  iFunctionGroupName;
    HBufC*                                  iPermission;
    RArray<MPermission::TMode>              iMappings;
    RPointerArray<HBufC>                    iArgs;
};

} // end of namespace MIDP

#endif // XPFPARSER_H
