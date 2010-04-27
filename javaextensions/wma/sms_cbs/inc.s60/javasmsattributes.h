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

#ifndef JAVASMSATTRIBUTES_H
#define JAVASMSATTRIBUTES_H

#include <gsmuelem.h>
#include <es_sock.h>
#include <etelmm.h>

#define MESSAGE_TEXT 0L
#define MESSAGE_BINARY 1L


_LIT(KSmsProtocol, "sms");
_LIT(KCbsProtocol, "cbs");
_LIT(KCBSEmptyAddress, "");
_LIT(KUriColonAndSlashes, "://");

const int KMaxUriSize = 287;
const int KPortFieldStartIndex = 7;
const int KSmsSendAndReceiveTotal = 10;
const int KCharacterConversionBufferSize = 100;
const bool KUse16BitPortAddressing = ETrue;
const int KSmsPayloadSizeInBytesWithPortAddressing = 133;
const int KSmsMaxDataSize = KSmsSendAndReceiveTotal
                            * KSmsPayloadSizeInBytesWithPortAddressing;

class TCBSParameters
{
public:
    TBuf<TGsmSmsTelNumberMaxLen> mAddress;
    TUint16 mPort;
    TSmsDataCodingScheme::TSmsAlphabet mEncoding;
    TInt mDataSize;
};

const TInt KCBSMaxDataSize =
    (RMobileBroadcastMessaging::KGsmBroadcastDataSize * 8) / 7;

typedef TBuf16<KCBSMaxDataSize> TCBSDataBuf;

typedef TPckgBuf<TCBSParameters> TCBSParametersBuf;

class TSMSParameters
{
public:
    TBuf<TGsmSmsTelNumberMaxLen> mAddress;
    TUint16 mPort;
    TSmsDataCodingScheme::TSmsAlphabet mEncoding;
    TInt mDataSize;
    TTime mTimestamp;
};

typedef TPckgBuf<TSMSParameters> TSMSParametersBuf;

typedef TBuf16<KSmsMaxDataSize> TSmsDataBuf;

class TJavaMessageParameters
{
public:
    TBuf<TGsmSmsTelNumberMaxLen> mAddress;
    TUint16 mPort;
    TSmsDataCodingScheme::TSmsAlphabet mEncoding;
    TInt mDataSize;
    TTime mTimestamp;
    TBuf16<KSmsMaxDataSize> mData;
};

typedef TPckgBuf<TJavaMessageParameters> TJavaMessageParametersBuf;


#endif // JAVASMSATTRIBUTES_H
