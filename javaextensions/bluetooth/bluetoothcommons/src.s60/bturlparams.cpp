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
* Description:
*
*/

#include <cctype>    // tolower
#include <locale>

#include "logger.h"
#include "bturlparams.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "exceptionbase.h"
#include "bluetoothconsts.h"

using namespace std;
using namespace java::util;
using namespace java::bluetooth;

OS_EXPORT BtUrlParams::BtUrlParams(std::wstring aUrl, std::wstring aFilter) :
        mMaster(false), mAuthorize(false), mAuthenticate(false), mEncrypt(false),
        mTransmitMtu(0), mReceiveMtu(0),mUrl(aUrl),mFilter(aFilter)
{
    JELOG2(EJavaBluetooth);

    if (!mUrl.empty())
    {
        extractUrlParams(mUrl);
    }

    if (!mFilter.empty())
    {
        extractFilterParams(mFilter);
    }
}

BtUrlParams::BtUrlParams()
{
}

OS_EXPORT BtUrlParams::~BtUrlParams()
{
    std::list<HBufC*>::const_iterator it;
    for (it = mBlockedSenders.begin(); it != mBlockedSenders.end(); it++)
    {
        delete *it;
    }
}

OS_EXPORT std::wstring BtUrlParams::getProtocol()
{
    return mProtocol;
}

OS_EXPORT std::wstring BtUrlParams::getServiceUuid()
{
    return validLongUuid(mServiceUuid);
}

OS_EXPORT std::wstring BtUrlParams::getParamName()
{
    return mName;
}

OS_EXPORT bool BtUrlParams::getParamMaster()
{
    return mMaster;
}

OS_EXPORT bool BtUrlParams::getParamAuthorize()
{
    return mAuthorize;
}

OS_EXPORT bool BtUrlParams::getParamAuthenticate()
{
    return mAuthenticate;
}

OS_EXPORT bool BtUrlParams::getParamEncrypt()
{
    return mEncrypt;
}

OS_EXPORT int BtUrlParams::getParamReceiveMtu()
{
    return mReceiveMtu;
}

OS_EXPORT int BtUrlParams::getParamTransmitMtu()
{
    return mTransmitMtu;
}

std::wstring BtUrlParams::validLongUuid(std::wstring &aStr)
{
    int length = aStr.length();
    std::wstring validUuid(aStr);

    // Only if it is shorter than 8 bytes,
    // Then it will be added with the SHORT_BASE_UUID_STRING
    // Otherwise simply prepend zeros to the given uuid.
    if (length < 8)
    {
        std::wstring zeros = L"00000000";
        validUuid = zeros.substr(length);
        validUuid += aStr;
        validUuid += SHORT_UUID_BASE_STRING;
    }
    else if (length < 32)
    {
        std::wstring zeros=L"00000000000000000000000000000000";
        validUuid = zeros.substr(length);
        validUuid += aStr;
    }
    return validUuid;
}

std::wstring &BtUrlParams::ToLower(std::wstring &aStr)
{
    JELOG2(EJavaBluetooth);
    for (size_t i = 0; i < aStr.length(); ++i)
        aStr[i] = std::tolower(aStr[i]);
    return aStr;
}

/**
 * extractUrlParams: Function to extract parameters from Server URL
 */

void BtUrlParams::extractUrlParams(std::wstring aUrl)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ BtUrlParams::extractUrlParams %S",
         aUrl.c_str());

    int index = std::string::npos;

    // Extracting protocol
    index = aUrl.find(PROTOCOL_SEPERATOR);

    if (std::string::npos != index)
    {
        mProtocol = aUrl.substr(0, index);
        aUrl = aUrl.substr(index + wcslen(PROTOCOL_SEPERATOR));
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::extractUrlParams Protocol=%S",
             mProtocol.c_str());
    }

    std::wstring tempUrl = aUrl.substr(0, wcslen(LOCALHOST));
    if (0 == ToLower(tempUrl).compare(LOCALHOST))
    {
        aUrl = aUrl.substr(wcslen(LOCALHOST));
    }

    LOG1(EJavaBluetooth, EInfo, "  BtUrlParams::extractUrlParams url=%S",
         aUrl.c_str());

    // Extracting Service UUID
    index = aUrl.find(SEMICOLON);
    if (std::string::npos != index)
    {
        mServiceUuid = aUrl.substr(0, index);
        aUrl = aUrl.substr(index);
    }
    else
    {
        mServiceUuid = aUrl;
    }

    if (aUrl.length() <= 1)
    {
        return;
    }

    // Extracting other parameters
    index = aUrl.find(SEMICOLON, 1);

    int eqLength = wcslen(EQUAL);
    while (std::string::npos != index)
    {
        int eqIndex = std::string::npos;

        eqIndex = aUrl.find(EQUAL);
        if (std::string::npos != eqIndex && eqIndex < index)
        {
            setUrlParamsValue(aUrl.substr(0, eqIndex + eqLength),
                              aUrl.substr(eqIndex + eqLength, index - (eqIndex
                                          + eqLength)));
        }
        else
        { // Actually erroneous URL !!
            break;
        }
        aUrl = aUrl.substr(index);
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::extractUrlParams In while: %S", aUrl.c_str());

        if (aUrl.length() <= 1)
        {
            break;
        }
        index = aUrl.find(SEMICOLON, 1);
    }

    // Extracting last parameter: In case URL is not ended with ";"
    if (!aUrl.empty())
    {
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::extractUrlParams After while loop: %S",
             aUrl.c_str());
        int eqIndex = std::string::npos;

        eqIndex = aUrl.find(EQUAL);
        if (std::string::npos != eqIndex)
        {
            setUrlParamsValue(aUrl.substr(0, eqIndex + eqLength),
                              aUrl.substr(eqIndex + eqLength, index - (eqIndex
                                          + eqLength)));
        }
    }
}

void BtUrlParams::setUrlParamsValue(std::wstring aParam, std::wstring aVal)
{
    JELOG2(EJavaBluetooth);
    LOG2(EJavaBluetooth, EInfo,
         "+ BtUrlParams::setUrlParamsValue Param:%S Val:%S", aParam.c_str(),
         aVal.c_str());
    ToLower(aParam);
    if (0 == aParam.compare(URL_PARAM_AUTHENTICATE))
    {
        mAuthenticate = ToLower(aVal).compare(L"false");
    }
    else if (0 == aParam.compare(URL_PARAM_AUTHORIZE))
    {
        mAuthorize = ToLower(aVal).compare(L"false");
    }
    else if (0 == aParam.compare(URL_PARAM_ENCRYPT))
    {
        mEncrypt = ToLower(aVal).compare(L"false");
    }
    else if (0 == aParam.compare(URL_PARAM_MASTER))
    {
        mMaster = ToLower(aVal).compare(L"false");
    }
    else if (0 == aParam.compare(URL_PARAM_NAME))
    {
        mName = aVal;
    }
    else if (0 == aParam.compare(URL_PARAM_RECEIVE_MTU))
    {
        try
        {
            LOG2(EJavaBluetooth, EInfo,
                 "+ BtUrlParams::setUrlParamsValue URL_PARAM_RECEIVE_MTU:%S Val:%S",
                 aParam.c_str(), aVal.c_str());
            mReceiveMtu = JavaCommonUtils::wstringToInt(aVal);
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(EJavaBluetooth,
                  "- ServiceRecord::setUrlParamsValue URL_PARAM_RECEIVE_MTU exception Caught: %S",
                  ex.toString().c_str());
        }
    }
    else if (0 == aParam.compare(URL_PARAM_TRANSMIT_MTU))
    {
        try
        {
            LOG2(EJavaBluetooth, EInfo,
                 "+ BtUrlParams::setUrlParamsValue URL_PARAM_TRANSMIT_MTU:%S Val:%S",
                 aParam.c_str(), aVal.c_str());
            mTransmitMtu = JavaCommonUtils::wstringToInt(aVal);
        }
        catch (ExceptionBase &ex)
        {
            ELOG1(EJavaBluetooth,
                  "- ServiceRecord::setUrlParamsValue URL_PARAM_TRANSMIT_MTU exception Caught: %S",
                  ex.toString().c_str());
        }
    }
    else
    {
        // Nothing to do
    }
}

/**
 * extractUrlParams: Function to extract parameters from Push Filter
 */

void BtUrlParams::extractFilterParams(std::wstring aFilter)
{
    JELOG2(EJavaBluetooth);
    LOG1(EJavaBluetooth, EInfo, "+ BtUrlParams::extractFilterParams %S",
         aFilter.c_str());
    int index = std::string::npos;

    // Extracting black list items (if any)
    index = aFilter.find(FILTER_PARAM_BLACKLIST);

    if (std::string::npos != index)
    {
        fillBlackList(aFilter.substr(index + wcslen(FILTER_PARAM_BLACKLIST)));
        aFilter = aFilter.substr(0, index);
    }

    LOG2(EJavaBluetooth, EInfo,
         "+ BtUrlParams::extractFilterParams Allowed List:(%d)%S", index,
         aFilter.c_str());

    // Extracting Allowed list
    index = aFilter.find(SEMICOLON);
    LOG2(EJavaBluetooth, EInfo,
         "+ BtUrlParams::extractFilterParams Allowed List \";\" (%d)%S",
         index, aFilter.c_str());

    if (std::string::npos != index)
    {
        mAllowedSenders.mPattern = aFilter.substr(0, index - wcslen(SEMICOLON));
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::extractFilterParams AllowedSender Pattern:%S",
             mAllowedSenders.mPattern.c_str());
    }
    else
    {
        mAllowedSenders.mPattern = aFilter;
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::extractFilterParams AllowedSender Pattern:%S",
             mAllowedSenders.mPattern.c_str());
    }

    // Extracting Authenticated
    index = aFilter.find(FILTER_PARAM_AUTHENTICATED);
    LOG2(
        EJavaBluetooth,
        EInfo,
        "+ BtUrlParams::extractFilterParams Allowed List \"authenticated\" (%d)%S",
        index, aFilter.c_str());
    if (std::string::npos != index)
    {
        mAllowedSenders.mAuthenticated = true;
        LOG(EJavaBluetooth, EInfo,
            "  BtUrlParams::extractFilterParams Authenticated");
    }
    else
    {
        mAllowedSenders.mAuthenticated = false;
    }

    // Extracting Authorized
    index = aFilter.find(FILTER_PARAM_AUTHORIZED);
    LOG2(
        EJavaBluetooth,
        EInfo,
        "+ BtUrlParams::extractFilterParams Allowed List \"authorized\" (%d)%S",
        index, aFilter.c_str());
    if (std::string::npos != index)
    {
        mAllowedSenders.mAuthorized = true;
        LOG(EJavaBluetooth, EInfo,
            "  BtUrlParams::extractFilterParams Authorized");
    }
    else
    {
        mAllowedSenders.mAuthorized = false;
    }
}

void BtUrlParams::fillBlackList(std::wstring aBlackList)
{
    JELOG2(EJavaBluetooth);
    int index = std::string::npos;

    index = aBlackList.find(SEMICOLON);

    while (std::string::npos != index)
    {
        std::wstring pattern;

        pattern = aBlackList.substr(0, index - wcslen(SEMICOLON));
        aBlackList = aBlackList.substr(index + wcslen(SEMICOLON));
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::fillBlackList Adding Pattern:%S",
             pattern.c_str());
        try
        {
            HBufC *blackPattern = S60CommonUtils::wstringToDes(
                                      pattern.c_str());
            mBlockedSenders.push_back(blackPattern);
            index = aBlackList.find(SEMICOLON);
        }
        catch (ExceptionBase &ex)
        {
            // Nothing to handle here.
        }
    }

    // Extracting last parameter: In case URL is not ended with ";"
    if (!aBlackList.empty())
    {
        LOG1(EJavaBluetooth, EInfo,
             "  BtUrlParams::fillBlackList Adding Last Pattern:%S",
             aBlackList.c_str());
        try
        {
            HBufC *blackPattern = S60CommonUtils::wstringToDes(
                                      aBlackList.c_str());
            mBlockedSenders.push_back(blackPattern);
        }
        catch (ExceptionBase &ex)
        {
            // Nothing to handle here.
        }
    }
}

OS_EXPORT bool BtUrlParams::isAllowedSender(HBufC& aDevAddr, bool &aAuthorized,
        bool &aAuthenticated)
{
    JELOG2(EJavaBluetooth);
    int matchFlag = false;

    LOG2(EJavaBluetooth, EInfo,
         "  BtUrlParams::isAllowedSender Device:%S  Pattern:%S",
         aDevAddr.Des().PtrZ(), mAllowedSenders.mPattern.c_str());

    matchFlag = matchWS(aDevAddr, mAllowedSenders.mPattern);

    if (matchFlag == true)
    {
        aAuthorized = mAllowedSenders.mAuthorized;
        aAuthenticated = mAllowedSenders.mAuthenticated;
    }
    return matchFlag;
}

OS_EXPORT bool BtUrlParams::isBlockedSender(HBufC& aDevAddr)
{
    JELOG2(EJavaBluetooth);
    int matchFlag = false;
    std::list<HBufC*>::const_iterator cii;

    for (cii = mBlockedSenders.end(); cii != mBlockedSenders.begin();)
    {
        if (matchHB(aDevAddr, **(--cii)))
            return true;
    }
    return matchFlag;
}

bool BtUrlParams::matchWS(HBufC& aDevAddr, std::wstring aPattern)
{
    JELOG2(EJavaBluetooth);
    HBufC *pattern;
    try
    {
        pattern = S60CommonUtils::wstringToDes(aPattern.c_str());
    }
    catch (ExceptionBase &ex)
    {
        return true;
    }

    bool ret = matchHB(aDevAddr, *pattern);
    delete pattern;
    return ret;
}

bool BtUrlParams::matchHB(HBufC &aDevAddr, HBufC &aPattern)
{
    JELOG2(EJavaBluetooth);

    if (aDevAddr.MatchF(aPattern) >= 0)
        return true;
    return false;
}
