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

#include <cstdio>

#include "metadatafilehandler.h"

namespace java
{
namespace security
{

bool JavaCertStoreMetadataFileHandler::writeState(std::string aFilePath, int aState)
{
    FILE * stateFile = fopen(aFilePath.c_str(), "w+");
    if (stateFile != NULL)
    {
        fprintf(stateFile, "%d\n", aState);
        fclose(stateFile);
        return true;
    }
    return false;
}

int JavaCertStoreMetadataFileHandler::readState(std::string aFilePath)
{
    int state = STATE_UNDEFINED;
    FILE * stateFile = fopen(aFilePath.c_str(), "r");
    if (stateFile != NULL)
    {
        int tmp = getc(stateFile);
        switch (tmp - 48)
        {
        case STATE_ENABLED:
        case STATE_DISABLED:
        case STATE_DELETED:
            state = tmp - 48;
            break;
        }
    }
    return state;
}

} //end namespace security
} //end namespace java
