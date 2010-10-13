/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CAppMngr2MidletInfoIterator
*
*/


#include <StringLoader.h>               // StringLoader
#include <SWInstCommonUI.rsg>           // Resource IDs
#include <javaapplicationsettings.rsg>           // Midlet resource IDs
#include <AknUtils.h>                   // AknTextUtils
#include <SWInstDefs.h>                 // KJadMimeType
#include <appmngr2drmutils.h>           // TAppMngr2DRMUtils

#include "appmngr2midletinfoiterator.h" // CAppMngr2MidletInfoIterator
#include "appmngr2midletappinfo.h"      // CAppMngr2MidletAppInfo
#include "appmngr2midletpackageinfo.h"  // CAppMngr2MidletPackageInfo
#include "appmngr2midletmanifestreader.h"
#include "appmngr2midletresourcehandler.h" // CAppMngr2MidletResourceHandler

#include "logger.h"
#include "securitystoragedatadefs.h"
#include "s60commonutils.h"

using namespace std;
using namespace java::util;

_LIT(KJava, "\x202AJava\x2122\x202C"); // Trade mark sign
_LIT(KLRE, "\x202A"); // Bidirectional text control
_LIT(KPDF, "\x202C"); // Bidirectional text control
_LIT(KLeftParenthes, "(");
_LIT(KListSeparatorAndSpace, ", ");

/*void CleanUpResetAndDestroy(TAny *aArray)
{
    if (aArray)
    {
        RPointerArray<HBufC>* array=(RPointerArray<HBufC>*)aArray;
        array->ResetAndDestroy();
        array->Close();
    }
}*/


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletInfoIterator* CAppMngr2MidletInfoIterator::NewL(
    CAppMngr2InfoBase& aMidlet, TAppMngr2InfoType aInfoType, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletInfoIterator::NewL ");
    CAppMngr2MidletInfoIterator* self =
        new(ELeave) CAppMngr2MidletInfoIterator(aMidlet, aInfoType, aFs, aResourceHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletInfoIterator::NewL ");
    return self;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::~CAppMngr2MidletInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletInfoIterator::~CAppMngr2MidletInfoIterator()
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletInfoIterator::~CAppMngr2MidletInfoIterator ");
    delete iInfoUrl;
    iInfoUrl = NULL;
    delete iUpdateUrl;
    iUpdateUrl = NULL;
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::GetInfoUrlL()
// ---------------------------------------------------------
//
const TDesC& CAppMngr2MidletInfoIterator::GetInfoUrlL()
{
    if (!iInfoUrl)
    {
        return KNullDesC;
    }
    return *iInfoUrl;
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::GetUpdateUrlL()
// ---------------------------------------------------------
//
const TDesC& CAppMngr2MidletInfoIterator::GetUpdateUrlL()
{
    if (!iUpdateUrl)
    {
        return KNullDesC;
    }
    return *iUpdateUrl;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::ConstructL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletInfoIterator::ConstructL ");
    BaseConstructL();
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetAllFieldsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetAllFieldsL()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletInfoIterator::SetAllFieldsL ");
    SetFieldL(R_SWCOMMON_DETAIL_NAME, iInfo.Name());

    if (iInfoType == EAppMngr2StatusInstalled)
    {
        CAppMngr2MidletAppInfo* midletInfo =
            reinterpret_cast<CAppMngr2MidletAppInfo*>(&iInfo);
        SetInstalledAppsInfoL(*midletInfo);
    }
    else
    {
        CAppMngr2MidletPackageInfo* packageInfo =
            reinterpret_cast<CAppMngr2MidletPackageInfo*>(&iInfo);
        // Create at least base iterator
        TRAP_IGNORE(SetNotInstalledAppsInfoL(*packageInfo));
    }
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletInfoIterator::SetAllFieldsL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetInstalledAppsInfoL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetInstalledAppsInfoL(
    CAppMngr2MidletAppInfo& aMidlet)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletInfoIterator::SetInstalledAppsInfoL ");
    //Version
    SetVersionL(aMidlet.Version());
    //Supplier
    SetVendorNameL(aMidlet.Vendor());
    //Status
    SetStatusL();
    //Memory
    SetLocationL();
    //Size
    SetFieldL(R_SWCOMMON_DETAIL_APPSIZE, aMidlet.Details());
    //Domain category
    SetDomainL(aMidlet.SecurityDomainCategory(), aMidlet.SecurityDomainName());
    //Technology
    SetFieldL(R_SWCOMMON_DETAIL_TECHNOLOGY, KJava);
    //Data size
    SetDataL(aMidlet.DataSize());
    //Info URL
    SetInfoUrlL(aMidlet.InfoUrl());
    //Content
    SetContentL(aMidlet.Content());
    // Permissions
    SetPermissionsL(aMidlet.Permissions());
    //Description
    SetTypeL(aMidlet.Description());
    // Update URL
    SetUpdateUrlL(aMidlet.UpdateUrl());

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletInfoIterator::SetInstalledAppsInfoL ");
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetNotInstalledAppsInfoL()
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetNotInstalledAppsInfoL(
    CAppMngr2MidletPackageInfo& aPackage)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletInfoIterator::SetNotInstalledAppsInfoL ");
    TFileName protectedFileName = aPackage.ProtectedFileName();

    if ((aPackage.IsDRMProtected()) &&
            (TAppMngr2DRMUtils::IsDRMRightsObjectMissingL(protectedFileName)))
    {
        // Application DRM protected. Rights object is missing
        return;
    }

    std::auto_ptr<AppMngr2MidletManifestReader>reader(
        new AppMngr2MidletManifestReader(iFs));

    RPointerArray<MJavaAttribute> attributes;

    reader->ReadManifestL(aPackage.FileName(), attributes);

    if (attributes.Count() == 0)
    {
        // no midlet suite attributes
        return;
    }

    // Version
    _LIT(KMIDletVersion, "MIDlet-Version");
    TInt index = AttributeValueIndex(attributes, KMIDletVersion);
    if (KErrNotFound != index)
    {
        SetVersionL(attributes[index]->Value());
    }
    //Status
    SetStatusL();
    //Memory
    SetLocationL();
    //Size
    SetFieldL(R_SWCOMMON_DETAIL_APPSIZE, aPackage.Details());
    //Data, we do not show, if package is not installed
    //Technology
    SetFieldL(R_SWCOMMON_DETAIL_TECHNOLOGY, KJava);

    //Description
    _LIT(KMIDletDescription, "MIDlet-Description");
    index = AttributeValueIndex(attributes, KMIDletDescription);
    if (KErrNotFound != index)
    {
        SetTypeL(attributes[index]->Value());
    }

    //URL
    _LIT(KMIDletInfoURL, "MIDlet-Info-URL");
    index = AttributeValueIndex(attributes, KMIDletInfoURL);
    if (KErrNotFound != index)
    {
        SetInfoUrlL(attributes[index]->Value());
    }

    //Required permissions, we do not show, if package is not installed
    //Domain category, we do not show, if package is not installed
    //Content
    SetContentL(attributes);

    //CleanupStack::PopAndDestroy(); //suiteAttributes host
    //attributes.ResetAndDestroy();
    //CleanupStack::PopAndDestroy(&attributes);
    attributes.ResetAndDestroy();


    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletInfoIterator::SetNotInstalledAppsInfoL ");
}


// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetStatusL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetStatusL()
{
    HBufC* status = NULL;
    TInt resourceOffset = iResourceHandler.AddResourceFileL();
    if (iInfoType == EAppMngr2StatusInstalled)
    {
        status = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_INSTALLED);
    }
    else  // iInfoType is EAppMngr2StatusNotInstalled
    {
        CAppMngr2MidletPackageInfo* pckg
        = static_cast<CAppMngr2MidletPackageInfo*>(&iInfo); // Not own
        if (pckg->JadFileName() != KNullDesC()
                && pckg->JarFileName() == KNullDesC())
        {
            status = StringLoader::LoadLC(R_JAVA_INFO_STAT_NOT_LOADED);
        }
        else
        {
            status = StringLoader::LoadLC(
                         R_SWCOMMON_DETAIL_VALUE_NOT_INSTALLED);
        }
    }
    if (resourceOffset > 0)
    {
        iResourceHandler.DeleteResourceFile(resourceOffset);
    }
    SetFieldL(R_SWCOMMON_DETAIL_STATUS, *status);
    CleanupStack::PopAndDestroy(status);
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletInfoIterator::CAppMngr2MidletInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2MidletInfoIterator::CAppMngr2MidletInfoIterator(
    CAppMngr2InfoBase& aMidlet, TAppMngr2InfoType aInfoType, RFs& aFs, CAppMngr2MidletResourceHandler& aResourceHandler) :
        CAppMngr2InfoIterator(aMidlet, aInfoType), iFs(aFs), iResourceHandler(aResourceHandler)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletInfoIterator::CAppMngr2MidletInfoIterator ");
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetVersionL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetVersionL(const TDesC& aVersion)
{
    HBufC* version = aVersion.AllocLC();
    TPtr ptrVersion = version->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(ptrVersion);
    HBufC* tmpBuf = HBufC::NewLC(
                        KLRE().Length() + version->Length() + KPDF().Length());

    TInt position = version->Find(KLeftParenthes());
    if (position >= 0)
    {
        // Add bidirectional text control
        tmpBuf->Des() = version->Mid(0, position);
        tmpBuf->Des() += KLRE();
        tmpBuf->Des() += version->Mid(position, version->Length() - position);
        tmpBuf->Des() += KPDF();
    }
    else
    {
        tmpBuf->Des() = *version;
    }

    SetFieldL(R_SWCOMMON_DETAIL_VERSION, *tmpBuf);
    CleanupStack::PopAndDestroy(tmpBuf);
    CleanupStack::PopAndDestroy(version);
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetVendorNameL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetVendorNameL(const TDesC& aVendor)
{
    //Set vendor
    HBufC* vendor = NULL;
    if (aVendor.Length() > 0)
    {
        vendor = aVendor.AllocL();
    }
    else
    {
        vendor = StringLoader::LoadL(R_SWCOMMON_DETAIL_VALUE_UNKNOWN_SUPPLIER);
    }
    CleanupStack::PushL(vendor);
    SetFieldL(R_SWCOMMON_DETAIL_SUPPLIER, *vendor);
    CleanupStack::PopAndDestroy(vendor);
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetDomainL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetDomainL(const std::wstring aSecurityDomainCategory, const std::wstring /*aSecurityDomainName*/)
{
    HBufC* domainString = NULL;
    if (aSecurityDomainCategory == UNIDENTIFIED_THIRD_PARTY_DOMAIN_CATEGORY)
    {
        domainString = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_UNT_3RD_PARTY);
    }
    else if (aSecurityDomainCategory == IDENTIFIED_THIRD_PARTY_DOMAIN_CATEGORY)
    {
        domainString = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_3RD_PARTY);
    }
    else if (aSecurityDomainCategory == OPERATOR_DOMAIN_CATEGORY)
    {
        domainString = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_OPERATOR);
    }
    else if (aSecurityDomainCategory == MANUFACTURER_DOMAIN_CATEGORY)
    {
        domainString = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_MANUFACTURER);
    }
    else
    {
        domainString = S60CommonUtils::wstringToDes(aSecurityDomainCategory.c_str());
        CleanupStack::PushL(domainString);
    }
    SetFieldL(R_SWCOMMON_DETAIL_DOMAIN, *domainString);
    CleanupStack::PopAndDestroy(domainString);
}

// CAppMngr2MidletInfoIterator::SetDataL()
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetDataL(const TDesC& aDataSize)
{
    if (aDataSize != KNullDesC)
    {
        TLex numLexer(aDataSize);
        TInt32 id(0);
        User::LeaveIfError(numLexer.Val(id));
        HBufC* data = StringLoader::LoadLC(R_SWCOMMON_DETAIL_VALUE_SIZE,
                                           id / 1024);
        SetFieldL(R_SWCOMMON_DETAIL_DATASIZE, *data);
        CleanupStack::PopAndDestroy(data);
    }
    else
    {
        HBufC* unknow = StringLoader::LoadLC(
                            R_SWCOMMON_DETAIL_VALUE_UNKNOWN_DATA);
        SetFieldL(R_SWCOMMON_DETAIL_DATASIZE, *unknow);
        CleanupStack::PopAndDestroy(unknow);
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetInfoUrlL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetInfoUrlL(const TDesC& aInfoUrl)
{
    if (aInfoUrl.Length() > 0)
    {
        SetFieldL(R_SWCOMMON_DETAIL_INFO_URL, aInfoUrl);
        if (iInfoUrl)
        {
            delete iInfoUrl;
            iInfoUrl = NULL;
        }
        iInfoUrl = aInfoUrl.AllocL();
    }
    else
    {
        HBufC* unknow = StringLoader::LoadLC(
                            R_SWCOMMON_DETAIL_VALUE_UNKNOWN_URL);
        SetFieldL(R_SWCOMMON_DETAIL_INFO_URL, *unknow);
        CleanupStack::PopAndDestroy(unknow);
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetUpdateUrlL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetUpdateUrlL(const TDesC& aUpdateUrl)
{
    delete iUpdateUrl;
    iUpdateUrl = NULL;
    iUpdateUrl = aUpdateUrl.AllocL();
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetContentL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetContentL(const TDesC& aContent)
{
    SetFieldL(R_SWCOMMON_DETAIL_CONTENT, aContent);
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetPermisisonsL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetPermissionsL(const TDesC& aPermissions)
{
    if (aPermissions.Length() > 0)
    {
        SetFieldL(R_SWCOMMON_DETAIL_PERMISSIONS, aPermissions);
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetContentL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetContentL(
    const RPointerArray<Java::MJavaAttribute>& aAttributes)
{
    // Read MIdlet-x attributes until not found.
    _LIT(KMIDletIndexPrefix, "MIDlet-%d");
    _LIT(KColon, ",");

    TInt index = KErrNone;
    TInt midletIndex = 1;

    // Read all MIDlet names belogning to the suite.
    HBufC16* names = NULL;

    while (index != KErrNotFound)
    {
        TBuf16<15> midletAttrName;
        midletAttrName.Format(KMIDletIndexPrefix, midletIndex);
        index = AttributeValueIndex(aAttributes, midletAttrName);

        if (KErrNotFound != index)
        {
            std::auto_ptr<HBufC16>midletName(
                aAttributes[index]->Value().AllocL());
            TPtrC16 midletNamePtr(midletName->Des());

            TInt delimIndex = midletNamePtr.Find(KColon);

            if (KErrNotFound != delimIndex)
            {
                // Use only name.
                TPtrC16 onlyName = midletNamePtr.Left(delimIndex);

                //if (firstRound)
                if (names == NULL || names->Des() == KNullDesC)
                {
                    // Append MIDlet name to names.
                    names = HBufC16::NewL(onlyName.Length());
                    TPtr namesPtr(names->Des());
                    namesPtr.Append(onlyName);
                    CleanupStack::PushL(names);
                }
                else
                {
                    // Append MIDlet name to names.
                    HBufC16* tmp = HBufC16::NewL(names->Length()
                                                 + KListSeparatorAndSpace().Length()
                                                 + onlyName.Length());
                    TPtr tmpPtr(tmp->Des());
                    tmpPtr.Copy(*names);
                    tmpPtr.Append(KListSeparatorAndSpace);
                    tmpPtr.Append(onlyName);
                    CleanupStack::PopAndDestroy(names);
                    names = tmp;
                    CleanupStack::PushL(names);
                }
            }
            midletIndex++;
        }
    }
    SetContentL(*names);
    CleanupStack::PopAndDestroy(names);
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::SetTypeL()
// ---------------------------------------------------------
//
void CAppMngr2MidletInfoIterator::SetTypeL(const TDesC& aDescription)
{
    if (aDescription.Length() > 0)
    {
        SetFieldL(R_SWCOMMON_DETAIL_TYPE, aDescription);
    }
    else
    {
        HBufC* unknow = StringLoader::LoadLC(
                            R_SWCOMMON_DETAIL_VALUE_UNKNOWN_TYPE);
        SetFieldL(R_SWCOMMON_DETAIL_TYPE, *unknow);
        CleanupStack::PopAndDestroy(unknow);
    }
}

// ---------------------------------------------------------
// CAppMngr2MidletInfoIterator::AttributeValueIndex()
// ---------------------------------------------------------
//
TInt CAppMngr2MidletInfoIterator::AttributeValueIndex(
    const RPointerArray<MJavaAttribute>& aAttrs, const TDesC& aName)
{
    TInt index = KErrNotFound;
    for (TInt i = 0; i < aAttrs.Count(); i++)
    {
        if (aAttrs[i]->Name() == aName)
        {
            index = i;
            break;
        }
    }
    return index;
}
