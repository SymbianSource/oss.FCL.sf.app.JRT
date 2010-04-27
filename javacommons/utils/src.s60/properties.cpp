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


#include <memory>
#include <jni.h>
#include <f32file.h>
#include <sysversioninfo.h>
#include <versioninfo.h>
#include <sysutil.h>
#include <hal.h>

#ifdef RD_JAVA_UI_QT

#include <QLocale>

#else // RD_JAVA_UI_QT

#include <bautils.h>
#include <barsc.h>
#include <avkon.rsg>
#include <AknUtils.h>

#endif // RD_JAVA_UI_QT

#include "javaenvinfo.h"
#include "commonproperties.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "logger.h"
#include "s60commonutils.h"

using namespace java::util;

jstring getS60LocaleL(JNIEnv* env);

#ifndef RD_JAVA_UI_QT

HBufC* getS60LocaleTempImplL();

#endif // RD_JAVA_UI_QT

HBufC* MicroEditionPlatformL();
TInt getHardwareVersionL(TDes& aHardwareType);

const TUint KBufferSize = 32;
_LIT(KMicroeditionPlatform,                     "S60");
_LIT(KMicroeditionPlatformValueDefaultPrefix,   "Series60");
_LIT(KMicroeditionPlatformName,      "sw_platform=");
_LIT(KMicroeditionPlatformVersion,   "sw_platform_version=");
_LIT(KMicroeditionPlatformJavaVersion, "java_build_version=");

jstring java::util::getLocaleImpl(JNIEnv* env)
{
    JELOG2(EUtils);

#ifdef RD_JAVA_UI_QT

    QString localeName = QLocale::system().name();

    jstring loc = env->NewString(localeName.utf16(), localeName.size());
    if (!loc)
    {
        std::bad_alloc();
    }
    return loc;

#else // RD_JAVA_UI_QT
    jstring loc = 0;
    HBufC* buf = 0;
    bool usingTempSolution = false;
    TRAPD(err, buf = AknLangUtils::DisplayLanguageTagL());
    if (buf == 0 && err == KErrNotSupported)
    {
        //At the moment DisplayLanguageTagL must be called from
        //UI thread. Once this is fixed by the Avkon, we must
        //use temporary solution.
        usingTempSolution = true;
        TRAP(err, buf = getS60LocaleTempImplL());
    }
    if (buf == 0)
    {
        std::string errorStr = "Could not solve locale when using ";
        if (usingTempSolution)
        {
            errorStr.append("temp");
        }
        else
        {
            errorStr.append("original");
        }
        errorStr.append(" solution. Leave code = ");
        errorStr.append(JavaCommonUtils::intToString(err));
        throw ExceptionBase(errorStr,
                            __FILE__,__FUNCTION__,__LINE__);
    }
    const jchar* stringPtr = buf->Ptr();
    const jsize stringLength = buf->Length();
    loc = env->NewString(stringPtr, stringLength);
    delete buf;
    return loc;

#endif // RD_JAVA_UI_QT
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

#ifndef RD_JAVA_UI_QT

HBufC* getS60LocaleTempImplL()
{
    JELOG2(EUtils);
    _LIT(KFileName, "z:\\resource\\avkon.rsc");
    TFileName fileName(KFileName);
    RFs iRFs;
    CleanupClosePushL(iRFs);
    User::LeaveIfError(iRFs.Connect());
    BaflUtils::NearestLanguageFile(iRFs, fileName);
    RResourceFile resourceFile;
    CleanupClosePushL(resourceFile);
    resourceFile.OpenL(iRFs, fileName);
    resourceFile.ConfirmSignatureL(0);
    HBufC8* textBuf8 = resourceFile.AllocReadLC(R_QTN_LANGUAGE_RFC3066_TAG);
    const TPtrC16 ptrBuf8((TText16*) textBuf8->Ptr(), (textBuf8->Length() + 1) >> 1);
    HBufC16* tag = ptrBuf8.AllocL();
    CleanupStack::PopAndDestroy(3);
    return tag;

}

#endif // RD_JAVA_UI_QT

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
        // Assume syntax V xx.xx\ndd-mm-yy\nNHL-vvv\n(c)Vendor
        TInt NLloc = swVersion.Locate('\n');
        TPtrC verPtr = swVersion.Mid(2, NLloc-2); // skip "V " in the beginning

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

