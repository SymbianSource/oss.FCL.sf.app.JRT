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


#ifndef BTURLPARAMS_H
#define BTURLPARAMS_H

#include <list>
#include <string>

namespace java
{
namespace bluetooth
{

// For URLs
#define SEMICOLON                   L";"
#define EQUAL                       L"="

#define PROTOCOL_SEPERATOR          L"://"
#define LOCALHOST                   L"localhost:"

#define URL_PARAM_AUTHENTICATE      L";authenticate="
#define URL_PARAM_AUTHORIZE         L";authorize="
#define URL_PARAM_ENCRYPT           L";encrypt="
#define URL_PARAM_MASTER            L";master="
#define URL_PARAM_NAME              L";name="
#define URL_PARAM_RECEIVE_MTU       L";receivemtu="
#define URL_PARAM_TRANSMIT_MTU      L";transmitmtu="

// For Filters
#define FILTER_PARAM_BLACKLIST      L";blacklist="
#define FILTER_PARAM_AUTHENTICATED  L";authenticated"
#define FILTER_PARAM_AUTHORIZED     L";authorized"

class BtUrlParams
{
public:
    OS_IMPORT BtUrlParams(std::wstring aUrl, std::wstring aFilter);
    OS_IMPORT ~BtUrlParams();

    // Getter functions
    OS_IMPORT std::wstring getProtocol();
    OS_IMPORT std::wstring getServiceUuid();
    OS_IMPORT std::wstring getParamName();
    OS_IMPORT bool getParamMaster();
    OS_IMPORT bool getParamAuthorize();
    OS_IMPORT bool getParamAuthenticate();
    OS_IMPORT bool getParamEncrypt();
    OS_IMPORT int getParamReceiveMtu();
    OS_IMPORT int getParamTransmitMtu();

    OS_IMPORT
    bool isAllowedSender(HBufC& aDevAddr, bool &aAuthorized,
                         bool &aAuthenticated);
    OS_IMPORT bool isBlockedSender(HBufC& aDevAddr);

private:
    // Methods
    BtUrlParams();
    void extractUrlParams(std::wstring aUrl);
    void extractFilterParams(std::wstring aFilter);
    void setUrlParamsValue(std::wstring aParam, std::wstring aVal);
    void fillBlackList(std::wstring aBlackList);
    std::wstring &ToLower(std::wstring &aStr);
    std::wstring BtUrlParams::validLongUuid(std::wstring &aStr);

    bool matchWS(HBufC& aDevAddr, std::wstring aPattern);
    bool matchHB(HBufC& aDevAddr, HBufC &aPattern);

private:
    // Extracted data
    class AllowedSender
    {
    public:
        std::wstring mPattern;
        bool mAuthorized;
        bool mAuthenticated;
    } mAllowedSenders;

    bool mMaster;
    bool mAuthorize;
    bool mAuthenticate;
    bool mEncrypt;
    int mTransmitMtu;
    int mReceiveMtu;
    std::wstring mName;
    std::wstring mServiceUuid;
    std::wstring mProtocol;
    std::list<HBufC*> mBlockedSenders;
    std::locale mLocale;

    // internal data
    std::wstring mUrl;
    std::wstring mFilter;
};

} //end namespace bluetooth
} //end namespace java

#endif // BTURLPARAMS_H
