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


#ifndef JAVACOMMONUTILS_H
#define JAVACOMMONUTILS_H

#include <string>
#include "javaosheaders.h"

#include <iostream>
#include <ostream>

namespace java
{
namespace util
{

class JavaCommonUtils
{
public:
    OS_IMPORT static int stringToInt(const std::string& str);
    OS_IMPORT static long long stringToLongLong(const std::string& str);
    OS_IMPORT static std::string intToString(const int& value);
    OS_IMPORT static std::string longLongToString(const long long& value);
    OS_IMPORT static int wstringToInt(const std::wstring& str);
    OS_IMPORT static std::wstring intToWstring(const int& value);
    OS_IMPORT static std::wstring longLongToWstring(const long long& value);
    OS_IMPORT static long long wstringToLongLong(const std::wstring& str);

    /**
     * Converts the null terminated UTF-8 string given as parameter
     * to UTF-16 wstring that is returned.
     * @param chr  the null terminated UTF-8 string to be converted
     * @return UTF-16 wstring
     * @throws ExceptionBase if conversion fails
     */
    OS_IMPORT static std::wstring utf8ToWstring(const char* chr);

    /**
     * Converts UTF-16 (UCS-2) wstring to null terminated UTF-8 string
     * User must note that this method allocates memory for UTF-8 content.
     * User must delete[] the memory once it is not needed any more.
     * @param str UTF-16 wstring to be converted
     * @return new null terminated UTF-8 string
     * @throws ExceptionBase if conversion fails
     */
    OS_IMPORT static char* wstringToUtf8(const std::wstring& str);

    /**
     * Strips characters at the beginning and end of the string.
     * @param aStr          Stripped string. Output argument.
     * @param aStrippedChar Character what is stripped away.
     * <p>
     * <h3>Example</h3>
     * <pre>
     * std::wstring str(L"\n EFFFFFFF \t");
     * trimWstring(str,L'\n');
     * trimWstring(str,L'\t');
     * //Note: For some reason 'space' character must be stripped last.
     * trimWstring(str,L' ');
     * //Result is 'EFFFFFFF'.
     *</pre>
     */
    OS_IMPORT static void trimWstring(std::wstring& aStr,const wchar_t& aStrippedChar);

    /**
     * Encode string to base64 string.
     *
     * @param aData to be encoded.
     * @return base64 encoded string.
     */
    OS_IMPORT static std::string base64encode(const std::string& aData);

    /**
     * Encode wstring to base64 wstring byte by byte.
     *
     * wstring aData is base64 encoded as a byte array.
     * If the wstring has been created from TDesC16 in Symbian it contains
     * UTF-16LE (UCS-2) encoded 16-bit characters.
     * The result is byte array that contains 8-bit ASCII characters.
     * Each of these 8-bit character is converted to the corresponding
     * wchar and appended to the result wstring.
     *
     * @see \sf\app\jrt\javamanager\javainstaller\installer\javasrc\com\nokia
     *  \mj\impl\installer\utils\Args.java, decodeBase64Args()
     * @param aData wstring containing the data to be encoded.
     * @return base64 encoded result wstring.
     */
    OS_IMPORT static std::wstring wbase64encode(const std::wstring& aData);

    /**
     * Decode base64 string.
     *
     * @param aData base64 encoded string.
     * @return decoded string.
     */
    OS_IMPORT static std::string base64decode(const std::string& aData);

    /**
     * Decodes percent-encoded UTF-8 characters embedded to UCS-2 string.
     * After the characters has been decoded, they are converted to UCS-2.
     * When the whole string is in UCS-2 format, it is returned.
     * @param str UTF-16 (UCS-2) wstring that contains %-encoded UTF-8 characters,
     *  for example "%C3%80%C3%80NEST%C3%80.EXE"
     * @return UTF-16 wstring, for example "ÄÄNESTÄ.EXE"
     * @throws ExceptionBase if UTF-8 to UTF-16 conversion fails
     */
    OS_IMPORT static std::wstring percentDecode(const std::wstring& str);

public:
    /**
     * Decodes one %<X><Y> sequence.
     * @param first the character <X>
     * @param sec the character <Y>
     * @return decoded char value 0-255
     * @throws ExceptionBase if <X> and <Y> are not hexadecimal characters
     */
    static char decodeOnePercentSeq(wchar_t first, wchar_t sec);
};

// A simple class for storing char array that is deleted automatically
// when going out of scope.
class ScopedCharArray
{
public:
    ScopedCharArray(int size) : mBuffer(0)
    {
        mBuffer = new char[size];
    }
    ~ScopedCharArray()
    {
        delete [] mBuffer;
    }

    char* get()
    {
        return mBuffer;
    }

private:
    ScopedCharArray();                                      //No default constructor allowed
    ScopedCharArray(const ScopedCharArray&);                //No copy constructor allowed
    ScopedCharArray& operator= (const ScopedCharArray&);    //No Assignment operator allowed

private:
    char* mBuffer;
};


} //end namespace util
} //end namespace java

#endif // JAVACOMMONUTILS_H

