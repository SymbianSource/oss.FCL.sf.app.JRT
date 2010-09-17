/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <memory>
#include <jni.h>
#include <f32file.h>
#include <sysversioninfo.h>
#include <versioninfo.h>
#include <sysutil.h>
#include <hal.h>

#include "javaenvinfo.h"
#include "commonproperties.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "logger.h"
#include "s60commonutils.h"

using namespace java::util;

HBufC* MicroEditionPlatformL();
TInt getHardwareVersionL(TDes& aHardwareType);

const TUint KBufferSize = 32;
_LIT(KMicroeditionPlatform,                     "S60");
_LIT(KMicroeditionPlatformValueDefaultPrefix,   "Series60");
_LIT(KMicroeditionPlatformName,      "sw_platform=");
_LIT(KMicroeditionPlatformVersion,   "sw_platform_version=");
_LIT(KMicroeditionPlatformJavaVersion, "java_build_version=");

_LIT(KMicroeditionLocaleAfrikaans,              "af-ZA");
_LIT(KMicroeditionLocaleAlbanian,               "sq-SQ");
_LIT(KMicroeditionLocaleArabic,                 "ar");
_LIT(KMicroeditionLocaleBasque,                 "eu");
_LIT(KMicroeditionLocaleIndonesian,             "id-ID");
_LIT(KMicroeditionLocaleMalay,                  "ms-MY");
_LIT(KMicroeditionLocaleBulgarian,              "bg-BG");
_LIT(KMicroeditionLocaleCatalan,                "ca");
_LIT(KMicroeditionLocalePrcChinese,             "zh-CN");
_LIT(KMicroeditionLocaleHongKongChinese,        "zh-HK");
_LIT(KMicroeditionLocaleTaiwanChinese,          "zh-TW");
_LIT(KMicroeditionLocaleCroatian,               "hr-HR");
_LIT(KMicroeditionLocaleCzech,                  "cs-CZ");
_LIT(KMicroeditionLocaleDanish,                 "da-DK");
_LIT(KMicroeditionLocaleDutch,                  "nl-NL");
_LIT(KMicroeditionLocaleEnglish,                "en");
_LIT(KMicroeditionLocaleAmerican,               "en-US");
_LIT(KMicroeditionLocaleEstonian,               "et-EE");
_LIT(KMicroeditionLocaleFarsi,                  "fa");
_LIT(KMicroeditionLocaleFinnish,                "fi-FI");
_LIT(KMicroeditionLocaleFrench,                 "fr");
_LIT(KMicroeditionLocaleCanadianFrench,         "fr-CA");
_LIT(KMicroeditionLocaleGalician,               "gl");
_LIT(KMicroeditionLocaleGerman,                 "de");
_LIT(KMicroeditionLocaleGreek,                  "el-GR");
_LIT(KMicroeditionLocaleHebrew,                 "he-IL");
_LIT(KMicroeditionLocaleHindi,                  "hi-IN");
_LIT(KMicroeditionLocaleMarathi,                "mr-IN");
_LIT(KMicroeditionLocaleHungarian,              "hu-HU");
_LIT(KMicroeditionLocaleIcelandic,              "is-IS");
_LIT(KMicroeditionLocaleItalian,                "it");
_LIT(KMicroeditionLocaleJapanese,               "ja-JP");
_LIT(KMicroeditionLocaleKorean,                 "ko-KR");
_LIT(KMicroeditionLocaleLatvian,                "lv-LV");
_LIT(KMicroeditionLocaleLithuanian,             "lt-LT");
_LIT(KMicroeditionLocaleNorwegian,              "no-NO");
_LIT(KMicroeditionLocalePolish,                 "pl-PL");
_LIT(KMicroeditionLocalePortuguese,             "pt-PT");
_LIT(KMicroeditionLocaleBrazilianPortuguese,    "pt-BR");
_LIT(KMicroeditionLocaleRomanian,               "ro-RO");
_LIT(KMicroeditionLocaleRussian,                "ru-RU");
_LIT(KMicroeditionLocaleSerbian,                "sr-YU");
_LIT(KMicroeditionLocaleSlovak,                 "sk-SK");
_LIT(KMicroeditionLocaleSlovenian,              "sl-SI");
_LIT(KMicroeditionLocaleSpanish,                "es-ES");
_LIT(KMicroeditionLocaleLatinAmericanSpanish,   "es-US");
_LIT(KMicroeditionLocaleSwahili,                "sw");
_LIT(KMicroeditionLocaleSwedish,                "sv");
_LIT(KMicroeditionLocaleTagalog,                "tl-PH");
_LIT(KMicroeditionLocaleThai,                   "th-TH");
_LIT(KMicroeditionLocaleTurkish,                "tr-TR");
_LIT(KMicroeditionLocaleUkrainian,              "uk-UA");
_LIT(KMicroeditionLocaleUrdu,                   "ur");
_LIT(KMicroeditionLocaleVietnamese,             "vi-VN");
_LIT(KMicroeditionLocaleZulu,                   "zu");

jstring java::util::getLocaleImpl(JNIEnv* env)
{
    JELOG2(EUtils);

    // microedition.locale
    ILOG1(EUtils, "User::Language: %d", User::Language());
    // KDialectMask enables support for operator specific language variants
    switch (User::Language() & KDialectMask) {
        case ELangAfrikaans:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleAfrikaans);
        case ELangAlbanian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleAlbanian);

        case ELangArabic:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleArabic);

#if defined (__S60_50__)
        case 327: // Indonesian in Asia-Pacific regions = 327
#else
        case ELangIndonesian_Apac:
#endif

        case ELangIndonesian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleIndonesian);

        case ELangMalay_Apac:
        case ELangMalay:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleMalay);

        case ELangBasque:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleBasque);

        case ELangBulgarian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleBulgarian);

        case ELangCatalan:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleCatalan);

        case ELangPrcChinese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocalePrcChinese);

        case ELangHongKongChinese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleHongKongChinese);

        case ELangTaiwanChinese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleTaiwanChinese);

        case ELangCroatian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleCroatian);

        case ELangCzech:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleCzech);

        case ELangDanish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleDanish);

        case ELangDutch:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleDutch);

        case ELangEnglish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleEnglish);

        case ELangAmerican:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleAmerican);

        case ELangEstonian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleEstonian);

        case ELangFarsi:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleFarsi);

        case ELangFinnish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleFinnish);

        case ELangFrench:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleFrench);

        case ELangCanadianFrench:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleCanadianFrench);

        case ELangGalician:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleGalician);

        case ELangGerman:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleGerman);

        case ELangGreek:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleGreek);

        case ELangHebrew:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleHebrew);

        case ELangHindi:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleHindi);

        case ELangHungarian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleHungarian);

        case ELangIcelandic:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleIcelandic);

        case ELangItalian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleItalian);

        case ELangJapanese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleJapanese);

        case ELangKorean:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleKorean);

        case ELangLatvian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleLatvian);

        case ELangLithuanian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleLithuanian);

        case ELangMarathi:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleMarathi);

        case ELangNorwegian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleNorwegian);

        case ELangPolish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocalePolish);

        case ELangPortuguese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocalePortuguese);

        case ELangBrazilianPortuguese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleBrazilianPortuguese);

        case ELangRomanian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleRomanian);

        case ELangRussian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleRussian);

        case ELangSerbian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSerbian);

        case ELangSlovak:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSlovak);

        case ELangSlovenian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSlovenian);

        case ELangSpanish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSpanish);

        case ELangLatinAmericanSpanish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleLatinAmericanSpanish);

        case ELangSwahili:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSwahili);

        case ELangSwedish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleSwedish);

        case ELangTagalog:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleTagalog);

        case ELangThai:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleThai);

        case ELangTurkish:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleTurkish);

        case ELangUkrainian:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleUkrainian);

        case ELangUrdu:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleUrdu);

        case ELangVietnamese:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleVietnamese);

        case ELangZulu:
            return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleZulu);
    }

    // According to MIDP2.0 spec the locale property, if not null, MUST
    // consist of the language and MAY optionally also contain the country
    // code, and variant separated by -
    return S60CommonUtils::NativeToJavaString(*env, KMicroeditionLocaleEnglish);
}

jstring java::util::getPlatformImpl(JNIEnv* aEnv)
{
    JELOG2(EUtils);


    jstring platform = NULL;
    std::auto_ptr<HBufC> propertyValue(0);

    TRAP_IGNORE(propertyValue.reset(MicroEditionPlatformL()));
    if (propertyValue.get())
    {
        platform = S60CommonUtils::NativeToJavaString(*aEnv,*propertyValue.get());
    }
    return platform;
}

HBufC* GetPlatformVersionL()
{
    VersionInfo::TPlatformVersion platformVersion;
    User::LeaveIfError(VersionInfo::GetVersion(platformVersion));
    TBuf<KBufferSize> version;

    version.AppendNum(platformVersion.iMajorVersion);
    version.Append('.');
    version.AppendNum(platformVersion.iMinorVersion);

    return version.AllocL();
}

HBufC* MicroEditionPlatformL()
{
    // First try to get the actual device name
    // If the device name is not found the default value is used: Series60/
    HBufC *productName= NULL;

    TBuf<KBufferSize> hardwareType(0);
    int retCode = getHardwareVersionL(hardwareType);

    if (retCode == KErrNone && hardwareType.Length() > 0)
    {
        TInt slashAfterManufacturer = hardwareType.Locate('/');
        TInt spaceAfterManufacturer = hardwareType.Locate(' ');
        TPtrC productNameShort = hardwareType;

        if ((slashAfterManufacturer != KErrNotFound) ||
                (spaceAfterManufacturer != KErrNotFound))
        {
            if (slashAfterManufacturer == KErrNotFound)
            {
                productNameShort.Set(hardwareType.Left(
                                         spaceAfterManufacturer));
            }
            else if (spaceAfterManufacturer == KErrNotFound)
            {
                productNameShort.Set(hardwareType.Left(
                                         slashAfterManufacturer));
            }
            else
            {
                productNameShort.Set(hardwareType.Left(
                                         slashAfterManufacturer < spaceAfterManufacturer ?
                                         slashAfterManufacturer :
                                         spaceAfterManufacturer));
            }
        }

        productName = HBufC::NewL(productNameShort.Length());
        productName->Des().Copy(productNameShort);
    }

    if (!productName)
    {
        productName = KMicroeditionPlatformValueDefaultPrefix().AllocLC();
    }
    else
    {
        CleanupStack::PushL(productName);
    }

    HBufC* platformName = NULL;

    TBuf<KSysUtilVersionTextLength> swVersion;
    if (SysUtil::GetSWVersion(swVersion) == KErrNone && swVersion.Length()>0)
    {
        // Assume syntax xx.xx\ndd-mm-yy\nNHL-vvv\n(c)Vendor
        TInt NLloc = swVersion.Locate('\n');
        if (KErrNotFound == NLloc)
        {
            NLloc = swVersion.Length();  // no '\n' -> use whole string
        }
        TPtrC verPtr = swVersion.Left(NLloc);

        // Get platform minor and major version numbers
        HBufC* platformVersionValue = GetPlatformVersionL();
        CleanupStack::PushL(platformVersionValue);

        // Get platform properties and values
        HBufC* platformNameProperty = KMicroeditionPlatformName().AllocLC();
        HBufC* platformNameValue = KMicroeditionPlatform().AllocLC();
        HBufC* platformVersionProperty = KMicroeditionPlatformVersion().AllocLC();
        HBufC* platformJavaVersionProperty = KMicroeditionPlatformJavaVersion().AllocLC();

        // Get Java version number
        TVersion versionInfo = Java::JavaEnvInfo::Version();
        TVersionName versionName;
        versionName.AppendNum(versionInfo.iMajor);
        versionName.Append('.');
        versionName.AppendNum(versionInfo.iMinor);
        versionName.Append('.');
        versionName.AppendNum(versionInfo.iBuild);
        HBufC* platformJavaVersionValue = versionName.AllocL();
        CleanupStack::PushL(platformJavaVersionValue);

        // Calculate length of property string
        TUint propertyLength = productName->Length() + 1 + verPtr.Length() + 1;
        propertyLength += platformNameProperty->Length();
        propertyLength += platformNameValue->Length() + 1;
        propertyLength += platformVersionProperty->Length();
        propertyLength += platformVersionValue->Length() + 1;
        propertyLength += platformJavaVersionProperty->Length();
        propertyLength += platformJavaVersionValue->Length();

        // Concatenate properties
        platformName = HBufC::NewL(propertyLength);
        platformName->Des().Append(*productName);
        platformName->Des().Append('/');
        platformName->Des().Append(verPtr);
        platformName->Des().Append('/');
        platformName->Des().Append(*platformNameProperty);
        platformName->Des().Append(*platformNameValue);
        platformName->Des().Append(';');
        platformName->Des().Append(*platformVersionProperty);
        platformName->Des().Append(*platformVersionValue);
        platformName->Des().Append(';');
        platformName->Des().Append(*platformJavaVersionProperty);
        platformName->Des().Append(*platformJavaVersionValue);

        CleanupStack::PopAndDestroy(platformJavaVersionValue);
        CleanupStack::PopAndDestroy(platformJavaVersionProperty);
        CleanupStack::PopAndDestroy(platformVersionProperty);
        CleanupStack::PopAndDestroy(platformNameValue);
        CleanupStack::PopAndDestroy(platformNameProperty);
        CleanupStack::PopAndDestroy(platformVersionValue);
    }

    CleanupStack::PopAndDestroy(productName);
    return platformName;
}

jint java::util::getFreeMemoryImpl(JNIEnv*)
{
    JELOG2(EUtils);
    int freeMemory = -1;
    HAL::Get(HAL::EMemoryRAMFree, freeMemory);
    return freeMemory;
}

TInt getHardwareVersionL(TDes& aHardwareType)
{
    SysVersionInfo::TProductVersion pVersion;

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    TInt rc = SysVersionInfo::GetVersionInfo(pVersion, fs);

    if (rc == KErrNone)
    {
        aHardwareType.Append(pVersion.iManufacturer);
        aHardwareType.Append(pVersion.iModel);
        aHardwareType.Append('/');
        aHardwareType.Append(pVersion.iProduct);
        aHardwareType.Append(pVersion.iRevision);
        aHardwareType.Append(';');
    }

    CleanupStack::PopAndDestroy(&fs);
    return rc;
}

