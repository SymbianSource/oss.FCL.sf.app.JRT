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


#include <msvstd.h>
#include <charconv.h>
#include <mmssettings.h>
#include <mmsapplicationadapter.h>

#include "logger.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include "javasymbianoslayer.h"
#include "mmspropertyretriever.h"

namespace java
{
namespace wma
{

/* -----------------------------------------------------------------------------
 * CJavaMmsPropertyRetriever::retrieveMsgCenter
 * Retrieves the MMSC URL used in
 * sending/receiving of multimedia messages.
 * -----------------------------------------------------------------------------
 */
jstring MmsPropertyRetriever::retrieveMsgCenter(JNIEnv& aJni)
{
    JELOG2(EWMA);
    jstring mmscNum = 0;
    HBufC* messageCenterNum = 0;
    CActiveScheduler* activeScheduler = 0;
    // An Active scheduler should be installed before creation of
    // CMmsApplicationAdapter . Check for activeScheduler installed
    // if not install.
    if (!CActiveScheduler::Current())
    {
        activeScheduler = new CActiveScheduler() ;
        CActiveScheduler::Install(activeScheduler);
    }
    TRAPD(error,
    {
        CMmsApplicationAdapter* mMmsApplicationAdapter =
        CMmsApplicationAdapter::NewL();
        mMmsApplicationAdapter->MmscUrlL(messageCenterNum);
        delete mMmsApplicationAdapter;
    });
    if (KErrNone == error)
    {
        if (messageCenterNum)
        {
            mmscNum = java::util::S60CommonUtils::NativeToJavaString(aJni,
                      *messageCenterNum);
        }
    }
    delete messageCenterNum;
    delete activeScheduler;
    return mmscNum;
}

/* -----------------------------------------------------------------------------
 * CJavaMmsRetriever::retrieveMaxMMSSize
 * Retrieves the maximum size of a multimedia message which can be
 * sent/received .
 * -----------------------------------------------------------------------------
 */
int MmsPropertyRetriever::retrieveMaxMMSSize()
{
    JELOG2(EWMA);
    CMmsSettings* mmsSettings = CMmsSettings::NewL();
    CleanupStack::PushL(mmsSettings);
    mmsSettings->LoadSettingsL();
    int maxMMSSize = mmsSettings->MaximumSendSize();
    CleanupStack::PopAndDestroy(mmsSettings);
    return maxMMSSize;
}

/* -----------------------------------------------------------------------------
 * CJavaMmsRetriever::isValidEncoding
 * Checks if the provided parameter points to a valid encoding which
 * could be used to encode a certain part/attachment of a multimedia
 * message. Symbians's character converter component(CCnvCharacterSetConverter)
 * is used for validating the encoding.
 * -----------------------------------------------------------------------------
 */
bool MmsPropertyRetriever::isValidEncoding(JNIEnv& aJni,
        jstring aContentEncoding)
{
    JELOG2(EWMA);
    bool isValidEncoding = false;
    TInt result = KErrNone;
    TRAP(result,
    {
        RFs fileServer;
        // initialize the handle to the file server
        User::LeaveIfError(fileServer.Connect());
        JStringUtils encoding(aJni,aContentEncoding);
        // convert the TPtrC to TPtrC8
        HBufC8* nEncoding = HBufC8::NewL(encoding.Length());
        CleanupStack::PushL(nEncoding);
        TPtr8 ptr(nEncoding->Des());
        ptr.Copy(encoding);
        CCnvCharacterSetConverter* cConv = CCnvCharacterSetConverter::NewL();
        CleanupStack::PushL(cConv);
        TUint charSetID =
            cConv->ConvertStandardNameOfCharacterSetToIdentifierL(ptr,fileServer);
        if (charSetID != 0)
        {
            // check that a corresponding MIBEnum has been found
            TUint mibEnum = cConv->ConvertCharacterSetIdentifierToMibEnumL(
                                charSetID,fileServer);
            if (mibEnum != 0)
            {
                // corresponding encoding has been found
                isValidEncoding = true;
            }
        }
        fileServer.Close();
        CleanupStack::PopAndDestroy(cConv);
        CleanupStack::PopAndDestroy(nEncoding);
    });
    return isValidEncoding;
}

} //namespace wma
} //namespace java
// end of file
