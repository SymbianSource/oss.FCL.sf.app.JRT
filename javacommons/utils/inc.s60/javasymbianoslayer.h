/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef JAVASYMBIANOSLAYER_H
#define JAVASYMBIANOSLAYER_H

#include <e32base.h>
#include <appversion.h>

#include "javaosheaders.h"
#include "javauid.h"

typedef void*(* FuncPtr)();

typedef FuncPtr(*LookupFunc)(const char* methodname);

struct FuncTable
{
    const char*    mFuncName;
    unsigned int   mFuncAddr;
};

OS_IMPORT FuncPtr findMethod(const char* funcName, const FuncTable funcTable[],
                             int tableSize);

OS_IMPORT HBufC* stringToDes(const char* str);

OS_IMPORT wchar_t* desToWstring(TPtr16& aDes);

/**
 * This method is supported only in S60 environment.
 * Converts integer to Uid object.
 * @param aId Id to be converted.
 * @param aOutUid Output parameter.
 * @return filled Uid object. Return object is equal with "aOutUid" parameter.
 *         Id is converted to hexadecimal format. Length of the number is always
 *         8 digits. See examples:
 *          1234        =>  [00001234]
 *          0x123456ab  =>  [123456ab]
 *          1289        =>  [00000509]
 */
OS_IMPORT java::util::Uid& TUidToUid(const TUid& aId,java::util::Uid& aOutUid);

/**
 * This method is supported only in S60 environment.
 * Converts Uid class to int. Supported formats:
 * [123456ab]
 * 0x123456ab
 * 123456ab
 * This operation strips following characters from the beginning and end of the string:
 * " ", "\n" and "\t".
 * @param aUid Length of the actual number is assumed to be 8 digit.
 * @param aOutId Output parameter. Uid object converted to int.
 * @return KErrNone is conversion was done successfully.
 *         KErrArgument if conversion failed for following errors:
 *         - Length of the number is not 8 digits after stripping and removing angle brackets.
 *         - Conversion from wstring to TUint fails.
 *         - Converted number is bigger than 0xEFFFFFFF or less than 0.
 */
OS_IMPORT TInt uidToTUid(const java::util::Uid& aUid,TUid& aOutId);

/**
 * Convert wide string AppVersion representation to TAppVersion.
 *
 * @param aString uid as wide string.
 * @return TAppVersion.
 */
OS_IMPORT TAppVersion wstringToAppVersion(const std::wstring& aVersionString);

/**
 * Convert wide string to HBufC. Ownership is moved to caller.
 *
 * @param aString wide string.
 * @return wide string as HBufC.
 */
OS_IMPORT HBufC* wstringToBuf(const std::wstring& aString);

template <class T>
class CleanupResetAndDestroy
{
public:
    inline static void PushL(T& aRef);
private:
    static void ResetAndDestroy(TAny *aPtr);
};
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
{
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
}
template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
{
    (STATIC_CAST(T*,aPtr))->ResetAndDestroy();
}
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
{
    CleanupResetAndDestroy<T>::PushL(aRef);
}


#endif // JAVASYMBIANOSLAYER_H
