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
* Description:  S60FileSystemUtilities
 *
*/


#ifndef S60FILESYSTEMUTILITIES_H
#define S60FILESYSTEMUTILITIES_H

#include <wchar.h>
#include <e32def.h>
#include <e32cmn.h>

#include "javaosheaders.h"

#include "jni.h"

namespace java
{
namespace fileutils
{

OS_NONSHARABLE_CLASS(S60FileSystemUtilities)
{
public:
    static void GetAvailableSizeL(TInt64& aRetSize, TChar& aDriveChar);

    static void GetUsedSizeL(TInt64& aRetSize, TChar& aDriveChar);

    static void GetTotalSizeL(TInt64& aRetSize, TChar& aDriveChar);

    static void IsHiddenL(TBool& aIsHidden, const TDesC& aFile);

    IMPORT_C static void SetHiddenL(TDesC& aFile, TBool aHide);

    static void ReplaceCharacters(TPtr& aPtr, TUint8 aReplacedChar,
                                  TUint8 aNewChar);

    static void ReplaceCharacters(TPtr& aPtr, TUint8 aReplacedChar,
                                  TUint8 aNewChar, TBool aOnlyFirstMatch);

    static TDesC16& GetSupportedDrives();

};

} // end namepsace fileutils
} // end namepsace java

#endif // S60FILESYSTEMUTILITIES_H
