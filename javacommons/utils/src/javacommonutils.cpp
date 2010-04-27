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
* Description:  JavaCommonUtils
*
*/


#include <iostream>
#include <sstream>
#include <vector>

#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "logger.h"
#include "javacommonutils.h"
#include "exceptionbase.h"
#include "convertutf.h"

using namespace java::util;

OS_EXPORT int JavaCommonUtils::stringToInt(const std::string& str)
{
//    JELOG2(EUtils);
//    value = boost::lexical_cast<int>( str );
    std::istringstream iss(str);
    int value = -1;
    if (!(iss >> value))
    {
        std::string errorStr = "Not able to convert to integer: ";
        errorStr.append(str);
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    return value;
}

OS_EXPORT long long JavaCommonUtils::stringToLongLong(const std::string& str)
{
//  JELOG2(EUtils);
    std::istringstream iss(str);
    long long value = -1LL;
    if (!(iss >> value))
    {
        std::string errorStr = "Not able to convert to 'long long': ";
        errorStr.append(str);
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    return value;
}

OS_EXPORT std::string JavaCommonUtils::intToString(const int& value)
{
//    JELOG2(EUtils);
//    str = boost::lexical_cast<std::string>( value );
    std::ostringstream oss;
    oss<<value;
    return oss.str();
}

OS_EXPORT std::string JavaCommonUtils::longLongToString(const long long& value)
{
//  JELOG2(EUtils);
    std::ostringstream oss;
    oss<<value;
    return oss.str();
}

OS_EXPORT int JavaCommonUtils::wstringToInt(const std::wstring& str)
{
//    JELOG2(EUtils);
    //throw ExceptionBase("JavaCommonUtils::wstringToInt not supported yet", __FILE__, __FUNCTION__, __LINE__);
//    value = boost::lexical_cast<int>( str );

    std::wistringstream iss(str);
    int value = -1;
    if (!(iss >> value))
    {
        std::string errorStr = "Not able to convert wstring to integer";
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    return value;
}

OS_EXPORT std::wstring JavaCommonUtils::intToWstring(const int& value)
{
//    JELOG2(EUtils);
    //throw ExceptionBase("JavaCommonUtils::wstringToInt not supported yet", __FILE__, __FUNCTION__, __LINE__);
//  return boost::lexical_cast<std::wstring>( value );
    std::wostringstream oss;
    oss<<value;
    return oss.str();
}

OS_EXPORT std::wstring JavaCommonUtils::longLongToWstring(const long long& value)
{
//  JELOG2(EUtils);
    std::wostringstream oss;
    oss<<value;
    return oss.str();
}

OS_EXPORT long long JavaCommonUtils::wstringToLongLong(const std::wstring& str)
{
//  JELOG2(EUtils);
    std::wistringstream iss(str);
    long long value;
    if (!(iss >> value))
    {
        std::string errorStr = "Not able to convert wstring to 'long long'";
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    return value;
}

OS_EXPORT std::wstring JavaCommonUtils::utf8ToWstring(const char* chr)
{
//    JELOG2(EUtils);
    if (chr==0)
    {
        throw ExceptionBase("Null argument", __FILE__, __FUNCTION__, __LINE__);
    }

    size_t widesize = strlen(chr);
    if (sizeof(wchar_t) == 2)
    {
        wchar_t* widestringnative = new wchar_t[widesize+3];
        const UTF8* sourcestart = reinterpret_cast<const UTF8*>(chr);
        const UTF8* sourceend = sourcestart + widesize;
        UTF16* targetstart = reinterpret_cast<UTF16*>(widestringnative);
        UTF16* targetend = targetstart + widesize + 3;
        ConversionResult res =
            ConvertUTF8toUTF16(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
        if (res != conversionOK)
        {
            delete [] widestringnative;
            std::string errorStr = "Not able to convert utf8 to utf16 wstring";
            throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        *targetstart = 0;  // add terminating null character
        std::wstring resultstring(widestringnative);
        delete [] widestringnative;
        return resultstring;
    }
    else if (sizeof(wchar_t) == 4)
    {
        wchar_t* widestringnative = new wchar_t[widesize+3];
        const UTF8* sourcestart = reinterpret_cast<const UTF8*>(chr);
        const UTF8* sourceend = sourcestart + widesize;
        UTF32* targetstart = reinterpret_cast<UTF32*>(widestringnative);
        UTF32* targetend = targetstart + widesize + 3;
        ConversionResult res =
            ConvertUTF8toUTF32(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
        if (res != conversionOK)
        {
            delete [] widestringnative;
            std::string errorStr = "Not able to convert utf8 to utf32 wstring";
            throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        *targetstart = 0; // add terminating null character
        std::wstring resultstring(widestringnative);
        delete [] widestringnative;
        return resultstring;
    }
    else
    {
        int len = strlen(chr);
        wchar_t* dest = new wchar_t[len+1];
        int i;
        for (i = 0; i < len; i++)
        {
            dest[i] = chr[i];
        }
        dest[i] = 0;
        std::wstring result = std::wstring(dest);
        delete [] dest;
        return result;
    }
}

OS_EXPORT char* JavaCommonUtils::wstringToUtf8(const std::wstring& str)
{
    size_t widesize = str.length();

    if (sizeof(wchar_t) == 2)
    {
        size_t utf8size = 3 * widesize + 3;
        // should be large enough to contain the conversion always
        char* utf8stringnative = new char[utf8size];
        const UTF16* sourcestart = reinterpret_cast<const UTF16*>(str.c_str());
        const UTF16* sourceend = sourcestart + widesize;
        UTF8* targetstart = reinterpret_cast<UTF8*>(utf8stringnative);
        UTF8* targetend = targetstart + utf8size;
        ConversionResult res =
            ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
        if (res != conversionOK)
        {
            delete [] utf8stringnative;
            std::string errorStr = "Not able to convert utf16 wstring to utf8";
            throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        *targetstart = 0;
        return utf8stringnative;
    }
    else if (sizeof(wchar_t) == 4)
    {
        size_t utf8size = 4 * widesize + 3;
        // should be large enough to contain the conversion always
        char* utf8stringnative = new char[utf8size];
        const UTF32* sourcestart = reinterpret_cast<const UTF32*>(str.c_str());
        const UTF32* sourceend = sourcestart + widesize;
        UTF8* targetstart = reinterpret_cast<UTF8*>(utf8stringnative);
        UTF8* targetend = targetstart + utf8size;
        ConversionResult res =
            ConvertUTF32toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
        if (res != conversionOK)
        {
            delete [] utf8stringnative;
            std::string errorStr = "Not able to convert utf32 wstring to utf8";
            throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        *targetstart = 0;
        return utf8stringnative;
    }
    else
    {
        char* chr = new char[str.size()+1];
        sprintf(chr,"%ls",str.c_str());
        return chr;
    }
}

OS_EXPORT void JavaCommonUtils::trimWstring(std::wstring& aStr,
        const wchar_t& aStrippedChar)
{
    if (aStr.empty())
        return;
    std::string::size_type const first = aStr.find_first_not_of(aStrippedChar);
    std::string::size_type const second = aStr.find_last_not_of(aStrippedChar);
    aStr = aStr.substr(first,second-first+1);
}

OS_EXPORT std::string JavaCommonUtils::base64encode(const std::string& aData)
{
    BIO* b64  = BIO_new(BIO_f_base64());
    if (NULL == b64)
    {
        return "";
    }
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new(BIO_s_mem());

    b64 = BIO_push(b64, bmem);
    BIO_write(b64, aData.c_str(), aData.length());
    BIO_flush(b64);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);

    std::string result;
    result.insert(0, bptr->data, bptr->length);
    BIO_free_all(b64);

    return result;
}

OS_EXPORT std::string JavaCommonUtils::base64decode(const std::string& aData)
{
    BIO* b64  = BIO_new(BIO_f_base64());
    if (NULL == b64)
    {
        return "";
    }
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new_mem_buf(
                    const_cast<char*>(aData.c_str()), aData.length());

    bmem = BIO_push(b64, bmem);

    char* buf = new char[aData.length()];
    int olen = BIO_read(bmem, buf, aData.length());

    std::string result;
    result.insert(0, buf, olen);

    BIO_free_all(bmem);
    delete[] buf;

    return result;
}

OS_EXPORT std::wstring JavaCommonUtils::percentDecode(const std::wstring& str)
{
    const std::wstring percent(L"%");
    std::string::size_type idx = str.find(percent);
    std::string::size_type cur = 0;

    std::wstring res;
    std::string convBuf;

    while (idx != std::string::npos)
    {
        // Add all characters before % sequence to final result string
        if ((idx - cur) > 0)
        {
            res.append(str.substr(cur, (idx - cur)));
        }

        // Decode all percent encoded chars in this sequence.
        do
        {
            char ch = decodeOnePercentSeq(str.at(idx + 1), str.at(idx + 2));
            // Add one UTF-8 value in this sequence to the buffer that will
            // be converter to UTF-16
            convBuf.append(&ch, 1);
            cur = idx + 3;
            idx = str.find(percent, cur);
        }
        while (idx == cur);

        // Now convert all UTF-8 values in buffer to UTF-16 and add result
        // to the final result string
        res.append(utf8ToWstring(convBuf.c_str()));
        convBuf.clear();
    }

    // Add characters after last % sequence if any
    res.append(str.substr(cur, str.length() - cur));

    return res;
}


char JavaCommonUtils::decodeOnePercentSeq(wchar_t first, wchar_t sec)
{
    // Handle one encoded %XY
    char decodedCh = 0;
    wchar_t ch = first;
    if ((ch >= '0') && (ch <= '9'))
    {
        decodedCh = (ch - '0') * 16;
    }
    else if ((ch >= 'A') && (ch <= 'F'))
    {
        decodedCh = ((ch - 'A') + 10) * 16;
    }
    else if ((ch >= 'a') && (ch <= 'f'))
    {
        decodedCh = ((ch - 'a') + 10) * 16;
    }
    else
    {
        std::string errorStr = "Illegal % encoding";
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }

    ch = sec;
    if ((ch >= '0') && (ch <= '9'))
    {
        decodedCh += (ch - '0');
    }
    else if ((ch >= 'A') && (ch <= 'F'))
    {
        decodedCh += (ch - 'A') + 10;
    }
    else if ((ch >= 'a') && (ch <= 'f'))
    {
        decodedCh += (ch - 'a') + 10;
    }
    else
    {
        std::string errorStr = "Illegal % encoding";
        throw ExceptionBase(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }

    return decodedCh;
}

