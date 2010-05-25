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


#include "metadatafilehandler.h"
#include <f32file.h>

namespace java
{
namespace security
{

bool JavaCertStoreMetadataFileHandler::writeState(std::string aFilePath, int aState)
{
    bool res = false;
    TRAP_IGNORE(
        int len = aFilePath.size();
        HBufC* filePath = HBufC::NewL(len);
        CleanupStack::PushL(filePath);
        TPtr ptr = filePath->Des();
        TPtr8 ptr8((unsigned char *)aFilePath.c_str(),len);
        ptr8.SetLength(len);
        ptr.Copy(ptr8);
        RFs rFs;
        RFile stateFile;
        int err = rFs.Connect();
        if (err == KErrNone)
        {
            err = stateFile.Open(rFs, ptr, EFileWrite);
            if (err == KErrNotFound)
            {
                err = stateFile.Create(rFs, ptr, EFileWrite);
            }
            if (err == KErrNone)
            {
                CleanupClosePushL(stateFile);
                TPckgBuf<TUint32> state(aState);
                stateFile.Write(0, state);
                CleanupStack::PopAndDestroy();  // state file
                res = true;
            }
        }
        CleanupStack::PopAndDestroy(); // file path
    );
    return res;
}

int JavaCertStoreMetadataFileHandler::readState(std::string aFilePath)
{
    int state = STATE_UNDEFINED;
    TRAP_IGNORE(
        int len = aFilePath.size();
        HBufC* filePath = HBufC::NewL(len);
        CleanupStack::PushL(filePath);
        TPtr ptr = filePath->Des();
        TPtr8 ptr8((unsigned char *)aFilePath.c_str(),len);
        ptr8.SetLength(len);
        ptr.Copy(ptr8);
        RFs rFs;
        RFile stateFile;
        int err = rFs.Connect();
        if (err == KErrNone)
        {
            err = stateFile.Open(rFs, ptr, EFileRead);
            if (err == KErrNone)
            {
                CleanupClosePushL(stateFile);
                TPckgBuf<TUint32> tmp;
                err = stateFile.Read(0, tmp);
                CleanupStack::PopAndDestroy(); // state file
                if (err == KErrNone)
                {
                    state = tmp();
                }
            }
        }
        CleanupStack::PopAndDestroy(); // file path
    );
    return state;
}

} //end namespace security
} //end namespace java
