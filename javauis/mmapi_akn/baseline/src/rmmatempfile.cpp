/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//  INCLUDE FILES

#include "rmmatempfile.h"

RMMATempFile::RMMATempFile()
{
    iFileHandle = KErrNone;
    iMidletSuiteID = KErrNotFound;
}

void RMMATempFile::ConnectL()
{
    User::Leave(KErrNotSupported);
}

void RMMATempFile::CreateL()
{
    User::Leave(KErrNotSupported);
}

RFile& RMMATempFile::File()
{
    return iFile;
}

void RMMATempFile::SetMIDletSuiteID(TInt aMidletSuiteID)
{
    iMidletSuiteID = aMidletSuiteID;
}

void RMMATempFile::Close()
{
}

//  END OF FILE
