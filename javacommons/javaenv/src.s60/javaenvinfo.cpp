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
* Description:  CJavaEnvInfo implementation.
*
*/


#include <e32base.h>
#include <f32file.h>
#include <pathinfo.h>

#include <javaenvinfo.h>
#include "logger.h"

namespace Java
{

TInt ParseVersionL(TLex& aVersionText,TBool aLastVersionNumber);

// User-agent header substring
_LIT(KUserAgentJavaString, "Configuration/CLDC-1.1 Profile/MIDP-2.1");

// About application Java version text
_LIT(KAboutApplicationString, "MIDP 2.1, CLDC 1.1");

// The path to resource file with java version
_LIT(KMicroeditionJavaVersionFile, "\\resource\\versions\\java.txt");

// Length of temporary buffer for string manipulation
const TUint KBufferSize = 32;

// The min and max values for each version number
// const TUint KMajorVersionMinValue = 1;
const TUint KMajorVersionMaxValue = 127;
// const TUint KMinorVersionMaxValue = 9;
// const TUint KBuildVersionMinValue = 1;
// const TUint KBuildVersionMaxValue = 32767;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// JavaEnvInfo::Version()
//
// The format of the version text in the version file is:
//    <major> '.', <minor> '.' <build>
//
// @return Java environment version
// ---------------------------------------------------------------------------
EXPORT_C TVersion JavaEnvInfo::Version()
{
    JELOG2(EUtils);

    TVersion version(0, 0, 0);

    // the error occured during reading a version file is ignored
    TRAP_IGNORE(version = GetJavaVersionL());

    LOG3(EUtils,EInfo," JavaEnvInfo::Version: major = %d, minor = %d, build = %d",
         version.iMajor, version.iMinor, version.iBuild);

    return version;
}

// ---------------------------------------------------------------------------
// JavaEnvInfo::GetUserAgentHeaderL returns the user agent header part.
//
// @return Java related user agent header substring
// ---------------------------------------------------------------------------
EXPORT_C HBufC* JavaEnvInfo::GetUserAgentHeaderL()
{
    JELOG2(EUtils);
    HBufC* userAgentHeader = KUserAgentJavaString().AllocL();
    return userAgentHeader;
}

// ---------------------------------------------------------------------------
// JavaEnvInfo::GetUserAgentHeaderL just returns the hardcoded literal. In
// the future, this might be read from a resource file if there is need for
// localization.
//
// @return About application text
// ---------------------------------------------------------------------------
EXPORT_C HBufC* JavaEnvInfo::GetPlatformInfoL()
{
    JELOG2(EUtils);
    HBufC* aboutString = KAboutApplicationString().AllocL();
    return aboutString;
}

// ---------------------------------------------------------------------------
// JavaEnvInfo::GetJavaVersionL()
//
// The format of the version text in the version file is:
//    <major> '.', <minor> '.' <build>
//
// The version file can be either in C-drive or in ROM (Z-drive). All the
// other locations are ignored. This means that when updating the Java
// environment, the new text file must be copied to C-drive.
// ---------------------------------------------------------------------------
TVersion JavaEnvInfo::GetJavaVersionL()
{
    JELOG2(EUtils);

    TInt size;

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    RFile file;
    //first try read the version information from C: drive

    LOG(EUtils,EInfo,"GetJavaVersionL: reading file from C: drive");
    TFileName alternateFile = TParsePtrC(
                                  PathInfo::PhoneMemoryRootPath()).Drive();
    alternateFile.Append(KMicroeditionJavaVersionFile);
    TInt ret = file.Open(rfs, alternateFile , EFileRead);

    if (ret != KErrNone)
    {
        LOG(EUtils,EInfo, " GetJavaVersionL: reading file from Z: drive");

        // if the version information file is not in C:-drive (KErrPathNotFound,
        // KErrNotFound) or can not be accessed for another reason, version
        // information is read from Z:-drive
        TFileName defaultFile = TParsePtrC(
                                    PathInfo::RomRootPath()).Drive();
        defaultFile.Append(KMicroeditionJavaVersionFile);
        User::LeaveIfError(file.Open(rfs, defaultFile, EFileRead));

        LOG(EUtils,EInfo, " GetJavaVersionL: reading file from Z: drive");
    }

    CleanupClosePushL(file);

    User::LeaveIfError(file.Size(size));
    if (size > KBufferSize)
    {
        size = KBufferSize;
    }

    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 des = buffer->Des();
    User::LeaveIfError(file.Read(des, size));

    HBufC16* buffer16 = HBufC16::NewLC(size);
    buffer16->Des().Copy(des);

    // parse the version numbers (major, minor, build) from the string
    // leave if  error occurs in string or in its format (e.g.: missing dot char, wrong value)
    TLex versionText(buffer16->Des());
    TUint32 majorVersion = ParseVersionL(versionText,EFalse);
    TUint32 minorVersion = ParseVersionL(versionText,EFalse);
    TUint32 buildVersion = ParseVersionL(versionText,ETrue);

    TVersion version(majorVersion, minorVersion, buildVersion);

    CleanupStack::PopAndDestroy(buffer16);
    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&rfs);

    return version;
}

//
//
//
TInt ParseVersionL(TLex& aVersionText,TBool aLastVersionNumber)
{
    JELOG2(EUtils);

    aVersionText.Mark();
    while ((aVersionText.Peek()).IsDigit())
    {
        // move to next char
        aVersionText.Inc();
    }
    TLex partialVersionText(aVersionText.MarkedToken());
    TUint32 version(0);
    if (partialVersionText.BoundedVal(version,
                                      EDecimal,
                                      Java::KMajorVersionMaxValue) != KErrNone)
    {
        LOG(EUtils,EInfo, " GetJavaVersionL: format of value is corrupted (major version)");
        User::Leave(KErrCorrupt);
    }
    // check format
    if (!aLastVersionNumber)
    {
        if (aVersionText.Peek() == '.')
        {
            // move to the next char (should be number)
            aVersionText.Inc();
        }
        else
        {
            LOG(EUtils,EInfo, " GetJavaVersionL: format of value is corrupted");
            User::Leave(KErrCorrupt);
        }
    }
    return version;
}

}

