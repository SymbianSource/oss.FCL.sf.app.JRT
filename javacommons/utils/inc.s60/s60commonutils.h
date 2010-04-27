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
* Description:  ?Description
*
*/


#ifndef S60COMMONUTILS_H
#define S60COMMONUTILS_H


#include "jni.h"
#include <e32def.h>
#include <e32cmn.h>
#include <wchar.h>
#include "javaosheaders.h"

namespace java
{
namespace util
{

class S60CommonUtils
{
public:
    OS_IMPORT static TInt CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer, TInt aOffset, TInt aLength, TDes8& aNativeBuffer);

    OS_IMPORT static TInt CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer, jbyteArray aJavaBuffer, TInt aOffset, TInt aLength);

    OS_IMPORT static jobjectArray NativeToJavaStringArray(JNIEnv& aJni, const RPointerArray<HBufC>& aNativeArray);


    OS_IMPORT static TTime JavaTimeToTTime(jlong aJavaTime);

    OS_IMPORT static jlong TTimeToJavaTime(TTime aEpocTime);

    OS_IMPORT static jstring NativeToJavaString(JNIEnv& aJni, const TDesC16& aNativeString);


    OS_IMPORT static const TUint JavaUpperEpocTime = 14474675;
    OS_IMPORT static const TUint JavaLowerEpocTime = 254771200;
    OS_IMPORT static HBufC* wstringToDes(const wchar_t* cStr);

    /**
     * Create a new TDesC8 from TDesC16.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt ConvertWiderToNarrowL(const TDesC& aSource,
            TDesC8*& aDestination);
    /**
     * Create a new TDesC8 from TDesC16 and leave it on the cleanup stack.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt ConvertWiderToNarrowLC(const TDesC& aSource,
            TDesC8*& aDestination);
    /**
     * Create a new TDesC16 from TDesC8.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt ConvertNarrowToWiderL(const TDesC8& aSource,
            TDesC*& aDestination);
    /**
     * Create a new TDesC16 from TDesC8 and leave it on the cleanup stack.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt ConvertNarrowToWiderLC(const TDesC8& aSource,
            TDesC*& aDestination);
    /**
     * Create the copy of TDesC8 and leave it on cleanup stack.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt CopyNarrowLC(const TDesC8& aSource,
                                       TDesC8*& aDestination);
    /**
     * Create the copy of TDesC8.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt CopyNarrowL(const TDesC8& aSource,
                                      TDesC8*& aDestination);
    /**
     * Create the copy of TDesC16 and leave it on cleanup stack.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt CopyWiderLC(const TDesC16& aSource,
                                      TDesC16*& aDestination);
    /**
     * Create the copy of TDesC16.
     * Transfer the owner ship to the caller
     * @param aSource: original descriptor
     * @param aDestination: newly created descriptor
     *
     * @return TInt (length of the newly created descriptor)
     */
    OS_IMPORT static TInt CopyWiderL(const TDesC16& aSource,
                                     TDesC16*& aDestination);

    /**
     * Searches for the most appropriate variant of the resource corresponding
     * to the device's language in use. The variant is searched on following
     * drives:
     * - on drive from where the calling dll is loaded (might be c: drive)
     * - on ROM drive (z:)
     *
     * @param aResourceFileName Path name of the resource file to be matched.
     *                          The path name is relative to 'resource/java' directory.
     *                          E.g. providing a path name 'lcdui.rsc' the
     *                          method would return either 'c:/resource/java/lcdui.rsc'
     *                          or 'z:/resource/java/lcdui.rsc'
     * @return                  The full name (including path) of the most appropriate
     *                          variant of the resource
     */
    OS_IMPORT static TFileName ResourceLanguageFileNameL(const TDesC& aResourceFileName);


    /**
     * Searches for the specified file from several drives
     * - on EDefaultSystem (usually C:)
     * - on ROM drive (Z:)
     *
     * @param aFileName The path name (with or without drive) of the file to be searched,
     *  for example '/resource/java/java_app.mif' or 'Z:/resource/java/version.txt'
     * @return The full name (including drive) of the first found version of the file.
     * @throws Leaves with KErrNotFound if file does not exist in any drive
     */
    OS_IMPORT static TFileName VerifiedFileNameL(const TDesC& aFileName);

};


} //end namespace util
} //end namespace java

#endif // S60COMMONUTILS_H
