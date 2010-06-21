/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  formatternative
*
*/


//#include <avkon.rsg>
#include <memory>
#include <stringresourcereader.h>
#include <aknutils.h>

#include "com_nokia_mj_impl_utils_Formatter.h"
#include "com_nokia_mj_impl_utils_ResourceLoader.h"
#include "javajniutils.h"
#include "s60commonutils.h"
#include "logger.h"

enum EDateTimeFormat
{
    DATE_LONG       = 1,
    DATE_SHORT      = 2,
    /*TIME_SHORT      = 3,
    TIME_LONG       = 4,
    DATETIME_SHORT  = 5,
    DATETIME_LONG   = 6*/
};

const TUint JavaUpperTimeFor1970 = 14474675;
const TUint JavaLowerTimeFor1970 = 254771200;

// const TInt KMaxDateTimeStringSize = 50;
const TInt KMaxDateFormatSize = 30;
const TInt KMaxNumberFormatSize = 40;
using namespace java::util;

// _LIT( KAvkonResFile, "z:\\resource\\avkon.rsc" );

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_Formatter__1formatInteger
(JNIEnv *aJni, jobject, jint aNumber)
{
    JELOG2(EUtils);
    TReal64 realNumber = aNumber;
    std::auto_ptr<HBufC> numberString(HBufC::New(KMaxNumberFormatSize));
    if (numberString.get() == 0)
    {
        return 0;
    }
    TPtr numberPtr = numberString->Des();

    TRealFormat numberFormat;

    TInt error = numberPtr.Num(realNumber, numberFormat);
    if (error < KErrNone)
    {
        WLOG2(EUtils,
              "Cannot format %d to current locale. Error: %d", aNumber, error);
    }

    AknTextUtils::LanguageSpecificNumberConversion( numberPtr );
    return aJni->NewString(
               (const jchar*)numberPtr.Ptr(), numberPtr.Length());
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_Formatter__1formatDate
(JNIEnv * aJni, jobject, jlong timeInMillis)
{
    std::auto_ptr<HBufC> dateString(HBufC::New(KMaxDateFormatSize));
    if (dateString.get() == 0)
    {
        return 0;
    }
    TPtr datePtr(dateString->Des());
    TBuf<KMaxDateFormatSize> dateStringBuf;

    // Java Date object is calculated by millisecs from 1.1.1970 0:00:00 GMT
    // Need conversion for Symbian TTime
    TInt64 timeNum = *reinterpret_cast<TInt64*>(&timeInMillis);
    TInt64 timeBeginNum =
        MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);

    TTime timeBegin(timeBeginNum);
    TTimeIntervalMicroSeconds delta(timeNum * 1000);
    TTime time = timeBegin + delta;

    _LIT(KTestFormat, "%/0%1%/1%2%/2%3%/3");
    TRAP_IGNORE(time.FormatL(dateStringBuf, KTestFormat));
    datePtr.Append(dateStringBuf);

    return aJni->NewString(
               (const jchar*)datePtr.Ptr(), datePtr.Length());
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_utils_ResourceLoader__1getLocaleId
(JNIEnv *, jobject)

{
    return (jint)User::Language();
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_Formatter__1formatDigits
  (JNIEnv * aEnv, jclass, jstring str)
{
    jstring ret = str;
    std::wstring wstr = JniUtils::jstringToWstring(aEnv, str);
    HBufC* buf = S60CommonUtils::wstringToDes(wstr.c_str());
    TPtr ptr(buf->Des());
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    ret = S60CommonUtils::NativeToJavaString(*aEnv, ptr);
    delete buf; buf = NULL;
    return ret;
}
