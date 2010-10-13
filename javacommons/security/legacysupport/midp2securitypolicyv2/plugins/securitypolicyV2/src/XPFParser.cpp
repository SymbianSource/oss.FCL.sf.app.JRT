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
 * @file    XPFParser.cpp
 *
 * @internal
 *
 */

#include "j2me/midp2/security/MPermission.h"
#include "ConnectionFilterType.h"
#include "FunctionGroup.h"
#include "FunctionGroupBinding.h"
#include "FileSystemAccessPolicy.h"
#include "DriveAccessPolicy.h"
#include "PathAccessPolicy.h"
#include "ProtectionDomain.h"
#include "SecurityPolicy.h"
#include "XPFParser.h"
#include "BlanketPermissionPolicy.h"
#include "BlanketPermissionPolicyElement.h"
#include <j2me/jdebug.h>

#define MAX_LENGTH_DG_POLICY 40

namespace MIDP
{

CXPFParser* CXPFParser::NewLC(RFile& aFile, CSecurityPolicy& aPolicy)
{
    CXPFParser* xpfp = new(ELeave) CXPFParser(aFile, aPolicy);

    CleanupStack::PushL(xpfp);
    xpfp->ConstructL();
    return xpfp;
}

CXPFParser::CXPFParser(RFile& aFile, CSecurityPolicy& aPolicy):
        iFile(aFile),
        iPolicy(aPolicy)
{
}

CXPFParser::~CXPFParser()
{
    iFile.Close();
    iFunctionGroups.Close();
    delete iFileBuffer;
    delete iBuffer;
    delete iFunctionGroupName;
}

const TUint32 KFileBufferSize = 8192;

const TUint32 KLineBufferSize = 256;

void CXPFParser::ConstructL(void)
{
    iFileBuffer = HBufC8::NewL(KFileBufferSize);
    iBuffer = HBufC::NewL(KLineBufferSize);
}

_LIT(KFormatVersion, "FormatVersion: 1.0");
_LIT(KBlanket,  "Blanket");
_LIT(KOneshot,  "Oneshot");
_LIT(KSession,  "Session");
_LIT(KNo,       "No");

// Policy := Misc* FormatVersion Domains FunctionGroups Bindings Misc*

void CXPFParser::ParseL(void)
{
    MiscL();
    EnsureL(*iBuffer, KFormatVersion);
    DomainsL();
    FunctionGroupsL();
    BindingsL();
    while (ReadLineL() != -1)
    {
        if ((iLineLength > 0) && ((*iBuffer)[0] != '#'))
        {
            UnexpectedL(ECommentOrNewline, *iBuffer);
        }
    }
}

//
// Domains := Misc* ^'Domains' NewLine (Misc* Domain NewLine+)+  ^'EndDomain' NewLine
//

_LIT(KDomains,    "Domains");
_LIT(KEndDomains, "EndDomains");

void CXPFParser::DomainsL(void)
{
    MiscL();
    EnsureL(*iBuffer, KDomains);
    for (;;)
    {
        MiscL();
        DomainL();
        if (*iBuffer == KEndDomains)
        {
            break;
        }
        iNextLine = ETrue;
    }
}

//
// Domain := WS* 'Domain:' WS+ DomainName (NewLine ConnectionFilters 'EndDomain')? NewLine
//
// ConnectionFilters := WS* 'ConnectionFilters' NewLine
//                                (Misc* ConnectionFilter NewLine+)+
//                      WS* 'EndConnectionFilters' NewLine
//

_LIT(KDomain,               "Domain:");
_LIT(KEndDomain,            "EndDomain");
_LIT(KUntrusted,            "[UNTRUSTED]");
_LIT(KConnectionFilters,    "ConnectionFilters");
_LIT(KEndConnectionFilters, "EndConnectionFilters");
_LIT(KFileAccess,           "FileAccess");
_LIT(KEndFileAccess,        "EndFileAccess");
_LIT(KBlanketPermission,    "BlanketPermission");
_LIT(KEndBlanketPermission, "EndBlanketPermission");

void CXPFParser::DomainL(void)
{
    EnsureTokenL(KDomain);
    switch (TokenL())
    {
    case EString:

        if (iPolicy.FindProtectionDomain(iToken) != NULL)
        {
            DuplicateDeclarationL(EDomain, iToken);
        }
        iCurrentDomain = iPolicy.AddProtectionDomainL(iToken);
        break;

    case ESymbol:

        if (iToken == KUntrusted)
        {
            if (iUntrustedDomain != NULL)
            {
                DuplicateDeclarationL(EUntrustedDomain, iToken);
            }
            iUntrustedDomain = iPolicy.AddUntrustedProtectionDomainL();
            iCurrentDomain   = iUntrustedDomain;
            break;
        }

        // Fall-through

    default:

        UnexpectedL(EDomainName, iToken);
    }
    NewLinePlusL();
    TokenL();

    TBool endDomainExpected = EFalse;

    if (iToken == KConnectionFilters)
    {
        ConnectionFiltersL();
        endDomainExpected = ETrue;
        TokenL();
    }

    if (iToken == KFileAccess)
    {
        FileAccessL();
        endDomainExpected = ETrue;
        TokenL();
    }

    if (iToken == KBlanketPermission)
    {
        BlanketPermissionL();
        endDomainExpected = ETrue;
        TokenL();
    }

    if (endDomainExpected)
    {
        EnsureL(iToken, KEndDomain);
        NewLinePlusL();
    }
    else
    {
        iLineOffset = 0;
    }
}

void CXPFParser::ConnectionFiltersL(void)
{
    NewLineL();
    for (;;)
    {
        MiscL();
        ConnectionFilterL();
        NewLinePlusL();
        TokenL();
        if (iToken == KEndConnectionFilters)
        {
            break;
        }
        iNextLine = ETrue;
        iLineOffset = 0;
    }
    NewLinePlusL();
}

_LIT(KNegativePortFilter,       "NegativePortFilter:");
_LIT(KEndNegativePortFilter,    "EndNegativePortFilter");
_LIT(KPort,                     "Port:");

//
// ConnectionFilter   := NegativePortFilter
//
// NegativePortFilter := 'NegativePortFilter' Ports 'EndNegativePortFilter' NewLine
//
// Ports              := (Misc* 'Port:' port NewLine+)+
//

_LIT(KNegativePortFilterTypeName, "NegativePortFilter");

void CXPFParser::ConnectionFilterL(void)
{
    EnsureTokenL(KNegativePortFilter);

    HBufC* scheme = StringLC();

    NewLineL();
    for (;;)
    {
        MiscL();
        EnsureTokenL(KPort);

        HBufC* port = StringLC();

        User::LeaveIfError(iArgs.Append(port));
        CleanupStack::Pop(port);
        NewLinePlusL();
        TokenL();
        if (iToken == KEndNegativePortFilter)
        {
            break;
        }
        iNextLine   = ETrue;
        iLineOffset = 0;
    }
    NewLineL();
    iCurrentDomain->AddConnectionFilterL(
        KNegativePortFilterTypeName,
        ENegativePortFilter,
        *scheme,
        iArgs);
    iArgs.ResetAndDestroy();
    CleanupStack::PopAndDestroy(scheme);
}

_LIT(KDriveList,        "DriveList:");
_LIT(KEndDriveList,     "EndDriveList");

void CXPFParser::FileAccessL(void)
{
    CFileSystemAccessPolicy* fsap = CFileSystemAccessPolicy::NewL();
    iCurrentDomain->SetFileSystemAccessPolicy(fsap);

    for (;;)
    {
        NewLinePlusL();
        TokenL();
        if (iToken == KDriveList)
        {
            DriveListL(*fsap);
        }
        else if (iToken == KEndFileAccess)
        {
            break;
        }
    }
    NewLinePlusL();
}


_LIT(KMutualExclusionSet,       "MutualExclusionSet");
_LIT(KEndMutualExclusionSet,    "EndMutualExclusionSet");
_LIT(KRiskConfirmationSet,      "RiskConfirmationSet");
_LIT(KEndRiskConfirmationSet,   "EndRiskConfirmationSet");

void CXPFParser::BlanketPermissionL(void)
{
    DEBUG("CXPFParser::BlanketPermissionL() - Enter");
    CBlanketPermissionPolicy* bpp = CBlanketPermissionPolicy::NewL();
    CleanupStack::PushL(bpp);

    for (;;)
    {
        NewLinePlusL();
        TokenL();
        if (iToken == KMutualExclusionSet || iToken == KRiskConfirmationSet)
        {
            ExclusionSetL(bpp);
        }
        else if (iToken == KEndBlanketPermission)
        {
            break;
        }
    }
    NewLinePlusL();
    CleanupStack::Pop(bpp);
    iCurrentDomain->SetBlanketPermissionPolicy(bpp);
    DEBUG("CXPFParser::BlanketPermissionL() - Exit");
}


void CXPFParser::ExclusionSetL(CBlanketPermissionPolicy* aBlanketPermissionPolicy)
{
    //Parse mutually excusive blanket permission sets and the
    //downgrade policy for each such pair of sets
    DEBUG("CXPFParser::ExclusionSetL() - Enter");
    TBool endOfSetA= EFalse;

    CBlanketPermissionPolicyElement* pe = CBlanketPermissionPolicyElement::NewL();
    CleanupStack::PushL(pe);

    //Sets clash type in policy element to either KMutualExclusionSet or KRiskConfirmationSet
    pe->AppendDowngradeFunctionGroupListL(iToken.AllocL());

    NewLinePlusL();
    // Parse mutually excusive blanket permission sets
    for (;;)
    {
        TToken type = TokenL();
        if (type==EString)
        {
            if (!endOfSetA)
            {//Add token to set A
                pe->AppendFunctionGroupAL(iToken.AllocL());
            }
            else
            {//Add token to set B
                pe->AppendFunctionGroupBL(iToken.AllocL());
            }

        }
        else if ((type==ESymbol) && (iToken.Length() == 1))   // ':'
        {
            //create setB now
            endOfSetA=ETrue;
        }
        else if (iToken==KEndMutualExclusionSet || iToken==KEndRiskConfirmationSet)
            break;

        if (iToken.Length() == 0)
        {
            NewLinePlusL();
        }

    }

    //Add the sets and permission downgrade policy to the
    //CBlanketPermissionPolicy object for this domain
    CleanupStack::Pop(pe);
    aBlanketPermissionPolicy->Add(pe);
    DEBUG("CXPFParser::ExclusionSetL() - Exit");
}


_LIT(KPathAccess,       "PathAccess:");
_LIT(KEndPathAccess,    "EndPathAccess");
_LIT(KPathMapping,      "PathMapping:");
_LIT(KEndPathMapping,   "EndPathMapping");

//
// DriveList := 'DriveList:' ( PathAccess | PathMapping )+ 'EndDriveList'
//

void CXPFParser::DriveListL(CFileSystemAccessPolicy& aFileSystemAccessPolicy)
{
    CPathAccessPolicy* pap = CPathAccessPolicy::NewL();
    CleanupReleasePushL(*pap);

    // Parse drive letters
    for (;;)
    {
        TokenL();

        if (iToken.Length() == 1)
        {
            TDriveNumber drive = TDriveNumber(DriveTokenL());
            CDriveAccessPolicy& dap =
                aFileSystemAccessPolicy.GetDriveAccessPolicyL(drive);
            dap.SetPathAccessPolicy(pap);
        }
        else
        {
            // Anything other that a single character moves on to the paths.
            break;
        }
    }

    if (iToken.Length() == 0)
    {
        NewLinePlusL();
        TokenL();
    }

    for (;;)
    {
        if (iToken == KPathAccess)
        {
            PathAccessL(*pap);
        }
        else if (iToken == KPathMapping)
        {
            PathMappingL(*pap);
        }
        else if (iToken == KEndDriveList)
        {
            break;
        }
        NewLinePlusL();
        TokenL();
    }

    CleanupStack::PopAndDestroy(pap);    // Only actually deleted if not used.
}

//
// Drive := <Single character between A and Z inclusive>
//

_LIT(KDriveLetter, "drive letter");

TInt CXPFParser::DriveTokenL()
{
    TUint letter = iToken[0];

    if (letter >= 'A' && letter <= 'Z')
    {
        return (letter - 'A' + EDriveA);
    }

    if (letter >= 'a' && letter <= 'z')
    {
        return (letter - 'a' + EDriveA);
    }

    UnexpectedL(KDriveLetter, iToken);
    return -1;  // Keep compiler happy.
}


_LIT(KRead,             "Read");
_LIT(KWrite,            "Write");

//
// PathAccess := ‘PathAccess:’ ( ‘Read’ | ‘Write’ )* Path+ ‘EndPathAccess’
//
// Path := <String containing one or more path elements>
//

void CXPFParser::PathAccessL(CPathAccessPolicy& aPathAccessPolicy)
{
    TInt modeBits = 0;

    for (;;)
    {
        TokenL();
        if (iToken == KRead)
        {
            modeBits |= 1;
        }
        else if (iToken == KWrite)
        {
            modeBits |= 2;
        }
        else if (iToken.Length() == 0)
        {
            break;
        }
        else
        {
            UnexpectedL(EPathAccessMode, iToken);
        }
    }

    MDriveAccessPolicy::TAccessMode mode;
    switch (modeBits)
    {
    case 1:
        mode = MDriveAccessPolicy::ERead;
        break;
    case 2:
        mode = MDriveAccessPolicy::EWrite;
        break;
    case 3:
        mode = MDriveAccessPolicy::EReadWrite;
        break;
    default:
        mode = MDriveAccessPolicy::ENone;
    }

    // Now the paths, ending with 'EndPathAccess'
    for (;;)
    {
        NewLinePlusL();
        TokenL();

        if (iToken == KEndPathAccess)
        {
            break;
        }

        aPathAccessPolicy.AddPathAccessL(iToken, mode);
    }
}


//
// PathMapping := 'PathMapping' ( From To )+ 'EndPathMapping'
//
// From := Dir
//
// Dir := <String containing a single directory name>
//
// To := Path | ( ‘MIDLET_HOME’ Path )
//

_LIT(KMidletHome,       "MIDLET_HOME");

void CXPFParser::PathMappingL(CPathAccessPolicy& aPathAccessPolicy)
{
    for (;;)
    {
        NewLinePlusL();
        TokenL();

        if (iToken == KEndPathMapping)
        {
            break;
        }

        TBuf<KMaxFileName> from = iToken;
        TBool inMidletHome = EFalse;

        if (TokenL() == ESymbol && iToken == KMidletHome)
        {
            inMidletHome = ETrue;
            TokenL();
        }

        aPathAccessPolicy.AddPathMappingL(from, iToken, inMidletHome);
    }
}



//
// FunctionGroups := Misc*
//                    'FunctionGroups' NewLine
//                         (Misc* FunctionGroup NewLine+)+
//                    'EndFunctionGroups' NewLine
//

_LIT(KFunctionGroups,       "FunctionGroups");
_LIT(KEndFunctionGroups,    "EndFunctionGroups");

void CXPFParser::FunctionGroupsL(void)
{
    MiscL();
    EnsureL(*iBuffer, KFunctionGroups);
    for (;;)
    {
        MiscL();
        FunctionGroupL();
        NewLinePlusL();
        if (*iBuffer == KEndFunctionGroups)
        {
            break;
        }
        iNextLine = ETrue;
    }
}

//
// FunctionGroup := 'FunctionGroup:' WS+ String NewLine
//                       Permissions
//                  'EndFunctionGroup' NewLine
//
// Permissions   := Misc*
//                   WS* 'Permissions' NewLine
//                        (PermissionName NewLine+)+
//                   WS* 'EndPermisions' NewLine
//

_LIT(KFunctionGroup,    "FunctionGroup:");
_LIT(KPermissions,      "Permissions");
_LIT(KEndPermissions,   "EndPermissions");
_LIT(KEndFunctionGroup, "EndFunctionGroup");


void CXPFParser::FunctionGroupL(void)
{
    EnsureTokenL(KFunctionGroup);

    HBufC* functionGroupName = StringLC();

    if (FindFunctionGroup(*functionGroupName) != NULL)
    {
        DuplicateDeclarationL(EFunctionGroup, *functionGroupName);
    }

    CFunctionGroup* functionGroup = iPolicy.AddFunctionGroupL(*functionGroupName);

    CleanupStack::PopAndDestroy(functionGroupName);
    User::LeaveIfError(iFunctionGroups.Append(functionGroup));
    MiscL();
    EnsureTokenL(KPermissions);
    NewLineL();
    for (;;)
    {
        MiscL();
        if (TokenL() != ESymbol)
        {
            UnexpectedL(EPermissionName, iToken);
        }
        iPolicy.AddPermissionL(*functionGroup, iToken);
        NewLinePlusL();
        TokenL();
        if (iToken == KEndPermissions)
        {
            break;
        }
        iNextLine = ETrue;
        iLineOffset = 0;
    }
    NewLinePlusL();
    EnsureTokenL(KEndFunctionGroup);
    NewLineL();

}

_LIT(KBindings,     "Bindings");
_LIT(KEndBindings,  "EndBindings");

//
// Bindings := Misc* 'Bindings' NewLine (Misc* DomainBindings NewLine+)+ 'EndBindings' NewLine
//

void CXPFParser::BindingsL(void)
{
    MiscL();
    EnsureL(*iBuffer, KBindings);
    for (;;)
    {
        MiscL();
        DomainBindingsL();
        NewLinePlusL();
        if (*iBuffer == KEndBindings)
        {
            break;
        }
        iNextLine = ETrue;
    }
}

_LIT(KDomainBindings,           "DomainBindings:");
_LIT(KEndDomainBindings,        "EndDomainBindings");

//
// DomainBindings := 'DomainBindings:' WS+ DomainName NewLine
//                         FunctionGroupBindings
//                   'EndDomainBindings' NewLine
//

void CXPFParser::DomainBindingsL(void)
{
    EnsureTokenL(KDomainBindings);
    switch (TokenL())
    {
    case EString:

        iCurrentDomain = iPolicy.FindProtectionDomain(iToken);
        if (iCurrentDomain == NULL)
        {
            UndeclaredL(EDomain, iToken);
        }
        break;

    case ESymbol:

        if (iToken == KUntrusted)
        {
            iCurrentDomain = iUntrustedDomain;
            if (iCurrentDomain == NULL)
            {
                UndeclaredL(EUntrustedDomain, iToken);
            }
            break;
        }
        // Fall-through

    default:

        UnexpectedL(EDomainName, iToken);
    }
    for (;;)
    {
        MiscL();

        TBool wildcardBinding = FunctionGroupBindingL();

        NewLinePlusL();
        TokenL();
        if (iToken == KEndDomainBindings)
        {
            break;
        }
        if (wildcardBinding)
        {
            UnexpectedL(KEndDomainBindings, iToken);
        }
        iNextLine   = ETrue;
        iLineOffset = 0;
    }
    NewLineL();
    iFunctionGroupBindings.Reset();
}

_LIT(KFunctionGroupBinding,     "FunctionGroupBinding:");
_LIT(KWildcard,                 "[*]");
_LIT(KPermission,               "Permission:");
_LIT(KAllowed,                  "Allowed");
_LIT(KUser,                     "User");
_LIT(KDefaultMode,              "DefaultMode:");
_LIT(KMaximumMode,              "MaximumMode:");
_LIT(KRules,                    "Rules");
_LIT(KEndRules,                 "EndRules");
_LIT(KEndFunctionGroupBinding,  "EndFunctionGroupBinding");

_LIT(KFunctionGroupNameOrWildcard, "FunctionGroupName or Wildcard");

#if !defined (NO_SYNTAX_EXTENSIONS)
_LIT(KInteractionModes,     "InteractionModes");
_LIT(KEndInteractionModes,  "EndInteractionModes");
#endif

TBool CXPFParser::FunctionGroupBindingL(void)
{
    TBool wildcardBinding = EFalse;

    EnsureTokenL(KFunctionGroupBinding);
    switch (TokenL())
    {
    case EString:
        delete iFunctionGroupName;
        iFunctionGroupName = NULL;
        iFunctionGroupName = iToken.AllocL();
        if (FindFunctionGroup(*iFunctionGroupName) == NULL)
        {
            UndeclaredL(EFunctionGroup, *iFunctionGroupName);
        }
        if (FindFunctionGroupBinding(*iFunctionGroupName) != NULL)
        {
            DuplicateDeclarationL(EFunctionGroupBinding, *iFunctionGroupName);
        }
        break;

    case ESymbol:

        if (iToken == KWildcard)
        {
            wildcardBinding = ETrue;
            break;
        }

        // Fall-through;

    default:

        UnexpectedL(KFunctionGroupNameOrWildcard, iToken);
    }
    NewLinePlusL();
    EnsureTokenL(KPermission);
    TokenL();

    MPermission::TMode defaultMode = MPermission::EAllow;
    MPermission::TMode maximumMode = MPermission::EAllow;

    CFunctionGroupBinding* binding;

    if (iToken == KAllowed)
    {
        NewLinePlusL();
        if (!wildcardBinding)
        {
            binding = iCurrentDomain->AddFunctionGroupBindingL(
                          *iFunctionGroupName,
                          MPermission::EAllowed,
                          defaultMode,
                          maximumMode);
            User::LeaveIfError(iFunctionGroupBindings.Append(binding));
        }
        else
        {
            AddDefaultBindingsL(MPermission::EAllowed, defaultMode, maximumMode);
        }
    }
    else if (iToken == KUser)
    {
#if defined (NO_SYNTAX_EXTENSIONS)
        NewLinePlusL();
        EnsureTokenL(KDefaultMode);
        defaultMode = InteractionModeL();
        NewLinePlusL();
        EnsureTokenL(KMaximumMode);
        maximumMode = InteractionModeL();
        NewLinePlusL();
        if (!wildcardBinding)
        {
            binding = iCurrentDomain->AddFunctionGroupBindingL(
                          *iFunctionGroupName,
                          MPermission::EUser,
                          defaultMode,
                          maximumMode);
            User::LeaveIfError(iFunctionGroupBindings.Append(binding));
            TokenL();
            if (iToken == KRules)
            {
                for (;;)
                {
                    MiscL();
                    RuleL(*binding);
                    NewLinePlusL();
                    TokenL();
                    if (iToken == KEndRules)
                    {
                        break;
                    }
                    iNextLine   = ETrue;
                    iLineOffset = 0;
                }
                NewLinePlusL();
            }
            else
            {
                iLineOffset = 0;
            }
        }
        else
        {
            AddDefaultBindingsL(MPermission::EUser, defaultMode, maximumMode);
        }
#else
        NewLinePlusL();
        EnsureTokenL(KDefaultMode);
        defaultMode = InteractionModeL();
        NewLinePlusL();
        TokenL();
        if (iToken == KMaximumMode)
        {
            maximumMode = InteractionModeL();
            NewLinePlusL();
            if (!wildcardBinding)
            {
                binding = iCurrentDomain->AddFunctionGroupBindingL(
                              *iFunctionGroupName,
                              MPermission::EUser,
                              defaultMode,
                              maximumMode);
                User::LeaveIfError(iFunctionGroupBindings.Append(binding));
                TokenL();
                if (iToken == KRules)
                {
                    for (;;)
                    {
                        MiscL();
                        RuleL(*binding);
                        NewLinePlusL();
                        TokenL();
                        if (iToken == KEndRules)
                        {
                            break;
                        }
                        iNextLine   = ETrue;
                        iLineOffset = 0;
                    }
                    NewLinePlusL();
                }
                else
                {
                    iLineOffset = 0;
                }
            }
            else
            {
                AddDefaultBindingsL(MPermission::EUser, defaultMode, maximumMode);
            }
        }
        else if (iToken == KInteractionModes)
        {
            TUint32 interactionModes = 0;
            TUint32 interactionMode  = 0;

            for (;;)
            {
                NewLinePlusL();
                TokenL();
                if (iToken == KEndInteractionModes)
                {
                    break;
                }
                else if (iToken == KNo)
                {
                    interactionMode = 1 << MPermission::EDenied;
                }
                else if (iToken == KOneshot)
                {
                    interactionMode = 1 << MPermission::EOneshot;
                }
                else if (iToken == KSession)
                {
                    interactionMode = 1 << MPermission::ESession;
                }
                else if (iToken == KBlanket)
                {
                    interactionMode = 1 << MPermission::EBlanket;
                }
                else
                {
                    UnexpectedL(EInteractionMode, iToken);
                }
                if (interactionModes & interactionMode)
                {
                    User::Leave(-1);
                }
                interactionModes |= interactionMode;
            }
            NewLinePlusL();
            if (!wildcardBinding)
            {
                iCurrentDomain->AddFunctionGroupBindingL(
                    *iFunctionGroupName,
                    MPermission::EUser,
                    defaultMode,
                    interactionModes);
            }
            else
            {
                AddDefaultBindingsL(MPermission::EAllowed, defaultMode, interactionModes);
            }
        }
        else
        {
            UnexpectedL(EPermissionType, iToken);
        }
#endif
    }
    else
    {
        UnexpectedL(EPermissionType, iToken);
    }
    EnsureTokenL(KEndFunctionGroupBinding);
    NewLineL();
    return wildcardBinding;
}

_LIT(KSimpleMapping,        "SimpleMapping:");
_LIT(KFunctionGroupMode,    "FunctionGroupMode:");
_LIT(KPermissionMode,       "PermissionMode:");
_LIT(KEndSimpleMapping,     "EndSimpleMapping");

void CXPFParser::RuleL(CFunctionGroupBinding& aBinding)
{
    EnsureTokenL(KSimpleMapping);
    TokenL();

    HBufC* permission = iToken.AllocLC();

    NewLinePlusL();
    for (;;)
    {
        EnsureTokenL(KFunctionGroupMode);
        User::LeaveIfError(iMappings.Append(InteractionModeL()));
        NewLinePlusL();
        EnsureTokenL(KPermissionMode);
        User::LeaveIfError(iMappings.Append(InteractionModeL()));
        NewLinePlusL();
        TokenL();
        if (iToken == KEndSimpleMapping)
        {
            break;
        }
        //iNextLine = ETrue; "JCF commented out to allow grouped simplemappings to work"
        iLineOffset = 0;
    }
    NewLineL();
    aBinding.AddSimpleMappingL(*permission, iMappings);
    CleanupStack::PopAndDestroy(permission);
    iMappings.Reset();
}

_LIT(KDenied,   "Denied");

MPermission::TMode CXPFParser::InteractionModeL(void)
{
    MPermission::TMode mode = MPermission::EInvalid;

    if (TokenL() != ESymbol)
    {
        UnexpectedL(ESymbol, iToken);
    }
    if (iToken == KBlanket)
    {
        mode = MPermission::EBlanket;
    }
    else if (iToken == KOneshot)
    {
        mode = MPermission::EOneshot;
    }
    else if (iToken == KSession)
    {
        mode = MPermission::ESession;
    }
    else if (iToken == KDenied)
    {
        mode = MPermission::EDenied;
    }
    else
    {
        UnexpectedL(EInteractionMode, iToken);
    }
    return mode;
}


void CXPFParser::MiscL(void)
{
    for (;;)
    {
        if (ReadLineL() == -1)
        {
            //UnexpectedL(EEof, ????);
        }
        if ((iBuffer->Length() > 0) && ((*iBuffer)[0] != '#'))
        {
            break;
        }
    }
}

void CXPFParser::NewLineL(void)
{
    SkipWhitespace();
    if (iLineOffset != iLineLength)
    {
        UnexpectedL(EEndOfLine, EAny);
    }
}

void CXPFParser::NewLinePlusL(void)
{
    NewLineL();
    for (;;)
    {
        if (ReadLineL() != 0)
        {
            break;
        }
    }
}

void CXPFParser::SkipWhitespace(void)
{
    while (iLineOffset < iLineLength)
    {
        TUint16 c = (*iBuffer)[iLineOffset];

        if ((c != 0x09) && (c != 0x20))
        {
            break;
        }
        ++iLineOffset;
    }
}

void CXPFParser::EnsureL(const TDesC& aActual, const TDesC& aExpected)
{
    if (aActual != aExpected)
    {
        UnexpectedL(aExpected, aActual);
    }
}

void CXPFParser::EnsureTokenL(const TDesC& aExpectedToken)
{
    TokenL();
    EnsureL(iToken, aExpectedToken);
}

CXPFParser::TToken CXPFParser::TokenL(TToken aExpected)
{
    TPtrC   buffer = *iBuffer;
    TToken  token  = ENoToken;

    while (iLineOffset < iLineLength)
    {
        TUint16 c = buffer[iLineOffset];

        if (c == '#')
        {
            iLineOffset = iLineLength;
            break;
        }
        else if ((c != 0x09) && (c != 0x20))
        {
            break;
        }
        ++iLineOffset;
    }

    TUint32 length = 0;
    TUint32 start  = iLineOffset;

    if (iLineOffset < iLineLength)
    {
        TBool stringp = buffer[iLineOffset] == '"';

        if (stringp)
        {
            ++start;
            ++iLineOffset;
        }
        while (iLineOffset < iLineLength)
        {
            TUint16 c = buffer[iLineOffset];

            if (stringp)
            {
                if (buffer[iLineOffset] == '"')
                {
                    ++iLineOffset;
                    break;
                }
            }
            else if ((c == 0x09) || (c == 0x20))
            {
                break;
            }
            ++iLineOffset;
            ++length;
        }
        if (stringp)
        {
            token = EString;
            if (iLineOffset == iLineLength)
            {
                //UnexpectedL(EString, EEndOfLine);
            }
        }
        else
        {
            token = ESymbol;
        }
    }
    iToken.Set(iBuffer->Ptr() + start, length);
    if ((aExpected != EAny) && (aExpected != token))
    {
        UnexpectedL(aExpected, iToken);
    }
    return token;
}

HBufC* CXPFParser::StringLC(void)
{
    TokenL(EString);
    return iToken.AllocLC();
}

TInt CXPFParser::ReadLineL(void)
{
    if (iNextLine)
    {
        iNextLine = EFalse;
    }
    else if (ReadLineL(iBuffer) != -1)
    {
        iLineLength = iBuffer->Length();
        ++iLineNumber;
        iLineOffset = 0;
    }
    else
    {
        iLineLength = -1;
    }
    return iLineLength;
}

const TUint16 CR = 13;
const TUint16 LF = 10;

TInt CXPFParser::ReadLineL(HBufC*& aBuffer)
{
    TBool   eof    = EFalse;
    TPtr    buffer = aBuffer->Des();
    TInt    length = buffer.MaxLength();
    TInt    offset = 0;

    buffer.SetLength(0);
    for (;;)
    {
        TUint16 c;
        TInt    n;

        n = ReadChar(c);
        if (n < 0)
        {
            User::Leave(n);
        }
        else if (n == 0)
        {
            eof = ETrue;
            break;
        }
        if (c == CR)
        {
            if (ReadChar(c) != KErrNone)
            {
            }
            if (c != LF)
            {
            }
            break;
        }
        else if (c == LF)
        {
            break;
        }
        if (offset == length)
        {
            length += length;
            iBuffer = iBuffer->ReAllocL(length);
        }
        buffer.Append(c);
        ++offset;
    }
    return buffer.Length() > 0 ? buffer.Length() : (eof ? -1 : 0);
}

TInt CXPFParser::ReadChar(TUint16& aChar)
{
    TInt   status;
    TUint8 a;

    status = Read(a);
    if (status == KErrEof)
    {
        return 0;
    }
    else if (status != KErrNone)
    {
        return status;
    }
    if (a <= 0x7F)
    {
        aChar = a;
    }
    else
    {
        TUint8 b;

        status = Read(b);
        if (status != KErrNone)
        {
            return status;
        }
        if ((b & 0xC0) != 0x80)
        {
            return KErrCorrupt;
        }
        switch (a & 0xF0)
        {
        case 0xC0:
        case 0xD0:

            aChar = (TUint16)((((TUint16)a & 0x1F) << 6) + (((TUint16)b) & 0x3F));
            break;

        case 0xE0:
        {
            TUint8  c;

            status = Read(c);
            if (status != KErrNone)
            {
                return status;
            }
            if ((c & 0xC0) != 0x80)
            {
                return KErrCorrupt;
            }
            aChar = (char)(((a & 0x1F) << 6) + ((b & 0x3F) << 6) + (b & 0x3F));
        }
        break;

        default:
            // No support for 4-byte encoding
            return KErrCorrupt;
        }
    }
    return 1;
}

TInt CXPFParser::Read(TUint8& aByte)
{
    if (iFileBufferOffset == iFileBufferLength)
    {
        TPtr8 ptr    = iFileBuffer->Des();
        TInt  status = iFile.Read(ptr);

        if (status != KErrNone)
        {
            return status;
        }
        iFileBufferOffset = 0;
        iFileBufferLength = iFileBuffer->Length();
        if (iFileBufferLength == 0)
        {
            return KErrEof;
        }
    }
    aByte = (*iFileBuffer)[iFileBufferOffset++];
    return KErrNone;
}

CFunctionGroup* CXPFParser::FindFunctionGroup(const TDesC& aName)
{
    TInt functionGroupCount = iFunctionGroups.Count();

    for (TInt i = 0; i < functionGroupCount; i++)
    {
        if (iFunctionGroups[i]->Name() == aName)
        {
            return iFunctionGroups[i];
        }
    }
    return NULL;
}

CFunctionGroupBinding* CXPFParser::FindFunctionGroupBinding(const TDesC& aName)
{
    TInt bindingCount = iFunctionGroupBindings.Count();

    for (TInt i = 0; i < bindingCount; i++)
    {
        if (iFunctionGroupBindings[i]->FunctionGroup().Name() == aName)
        {
            return iFunctionGroupBindings[i];
        }
    }
    return NULL;
}

void CXPFParser::AddDefaultBindingsL(
    MPermission::TType aType,
    MPermission::TMode aDefaultMode,
    MPermission::TMode aMaximumMode)
{
    TInt fgCount = iFunctionGroups.Count();

    for (TInt i = 0; i < fgCount; i++)
    {
        CFunctionGroup* fg = iFunctionGroups[i];

        if (FindFunctionGroupBinding(fg->Name()) == NULL)
        {
            iCurrentDomain->AddFunctionGroupBindingL(
                fg->Name(),
                aType,
                aDefaultMode,
                aMaximumMode);
        }
    }
}

void CXPFParser::AddDefaultBindingsL(
    MPermission::TType aType,
    MPermission::TMode aDefaultMode,
    TUint32            aInteractionModeMap)
{
    TInt fgCount = iFunctionGroups.Count();

    for (TInt i = 0; i < fgCount; i++)
    {
        CFunctionGroup* fg = iFunctionGroups[i];

        if (FindFunctionGroupBinding(fg->Name()) == NULL)
        {
            iCurrentDomain->AddFunctionGroupBindingL(
                fg->Name(),
                aType,
                aDefaultMode,
                aInteractionModeMap);
        }
    }
}

#if !defined(_DEBUG)

void CXPFParser::DuplicateDeclarationL(TDeclarationType, const TDesC&)
{
    User::Leave(KErrCorrupt);
}

void CXPFParser::UndeclaredL(TDeclarationType, const TDesC&)
{
    User::Leave(KErrCorrupt);
}

void CXPFParser::UnexpectedL(const TDesC&, const TDesC&)
{
    User::Leave(KErrCorrupt);
}

void CXPFParser::UnexpectedL(TToken /*aExpected*/, const TDesC& /*aActual*/)
{
    User::Leave(KErrCorrupt);
}

void CXPFParser::UnexpectedL(TToken /*aExpected*/, TToken /*aActual*/)
{
    User::Leave(KErrCorrupt);
}

#else

}   // End namespace MIDP around include
#include "j2me/jdebug.h"
namespace MIDP      // Reopen it again
{

_LIT(KDuplicateDeclarationTemplate, "Error at line %d: The %S %S has already been declared");

void CXPFParser::DuplicateDeclarationL(TDeclarationType aType, const TDesC& aName)
{
    ErrorL(KDuplicateDeclarationTemplate, ToString(aType), aName);
}

_LIT(KUndeclaredTemplate, "Error at line %d: The %S %S has not been declared");

void CXPFParser::UndeclaredL(TDeclarationType aType, const TDesC& aName)
{
    ErrorL(KUndeclaredTemplate, ToString(aType), aName);
}

_LIT(KUnexpectedTemplate, "Error at line %d: Expected %S but got %S");

void CXPFParser::UnexpectedL(const TDesC& aExpected, const TDesC& aActual)
{
    ErrorL(KUnexpectedTemplate, aExpected, aActual);
}

void CXPFParser::UnexpectedL(TToken aExpectedToken, const TDesC& aActual)
{
    UnexpectedL(ToString(aExpectedToken), aActual);
}

void CXPFParser::UnexpectedL(TToken aExpectedToken, TToken aActualToken)
{
    UnexpectedL(ToString(aExpectedToken), ToString(aActualToken));
}

void CXPFParser::ErrorL(const TDesC& aTemplate, const TDesC& aArg1, const TDesC& aArg2)
{
    HBufC* buffer    = HBufC::NewLC(aTemplate.Length() + aArg1.Length() + aArg2.Length());
    TPtr   bufferPtr = buffer->Des();

    bufferPtr.Format(aTemplate, iLineNumber, &aArg1, &aArg2);
    DEBUG_STR("%S", *buffer);
    delete buffer;
    User::Leave(KErrCorrupt);
}

_LIT(KSayWhat, "????");

_LIT(KStringToken,              "a string");
_LIT(KSymbolToken,              "a symbol");
_LIT(KCommentToken,             "a comment");
_LIT(KCommentOrNewlineToken,    "a comment or newline");

const TDesC& CXPFParser::ToString(TToken aToken)
{
    switch (aToken)
    {
    case EString:

        return KStringToken;

    case ESymbol:

        return KSymbolToken;

    case EComment:

        return KCommentToken;

    case ECommentOrNewline:

        return KCommentOrNewlineToken;

    default:

        return KSayWhat;
    }
}

_LIT(KDomainDecl,               "domain");
_LIT(KUntrustedDomainDecl,      "untrusted domain");
_LIT(KFunctionGroupDecl,        "function group");
_LIT(KPermissionDecl,           "permission");
_LIT(KFunctionGroupBindingDecl, "function group binding");


const TDesC& CXPFParser::ToString(TDeclarationType aDeclarationType)
{
    switch (aDeclarationType)
    {
    case EDomain:

        return KDomainDecl;

    case EUntrustedDomain:

        return KUntrustedDomainDecl;

    case EFunctionGroup:

        return KFunctionGroupDecl;

    case EPermission:

        return KPermissionDecl;

    case EFunctionGroupBinding:

        return KFunctionGroupBinding;

    default:

        return KSayWhat;
    }
}


#endif /* !defined(_DEBUG) */

} // end of namespace MIDP
