/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "mifconverter.h"


CMifConverter* CMifConverter::NewL(RFs& aFs, const TDesC& aMifFile)
{
    CMifConverter* self = new(ELeave) CMifConverter(aFs);
    CleanupStack::PushL(self);
    self->ConstructL(aMifFile);
    CleanupStack::Pop(self);
    return self;
}

CMifConverter::~CMifConverter()
{
    iMifWriteStream.Close();
}


void CMifConverter::WriteMifFileHeaderL(const TMifFileHeader& aMifFileHeader)
{
    // write file header
    iMifWriteStream.WriteInt32L(aMifFileHeader.iUid);
    iMifWriteStream.WriteInt32L(aMifFileHeader.iVersion);
    iMifWriteStream.WriteInt32L(aMifFileHeader.iOffset);
    iMifWriteStream.WriteInt32L(aMifFileHeader.iLength);
    iMifWriteStream.CommitL();
}


void CMifConverter::WriteMifIconOffsetL(const TMifIconOffset& aMifIconOffset)
{
    // write icon offset
    iMifWriteStream.WriteInt32L(aMifIconOffset.iIconOffset);
    iMifWriteStream.WriteInt32L(aMifIconOffset.iIconLength);
    iMifWriteStream.CommitL();
}


void CMifConverter::WriteMifIconHeaderL(const TMifIconHeader& aMifIconHeader)
{
    // write icon header
    iMifWriteStream.WriteInt32L(aMifIconHeader.iUid);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iVersion);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iOffset);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iLength);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iType);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iDepth);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iAnimated);
    iMifWriteStream.WriteInt32L(aMifIconHeader.iMaskDepth);
    iMifWriteStream.CommitL();
}


void CMifConverter::WriteMifBodyL(const TDesC8& apIconBuf)
{
    iMifWriteStream.WriteL(apIconBuf);
    iMifWriteStream.CommitL();
}

CMifConverter::CMifConverter(RFs& aFs): iFs(aFs)
{
}

void CMifConverter::ConstructL(const TDesC& aMifFile)
{
    TInt err = iMifWriteStream.Create(iFs, aMifFile, EFileWrite);
    if (KErrAlreadyExists == err)
    {
        // Temp file has not been removed as it should,
        // must destroy it before we can reuse it.
        err = iFs.Delete(aMifFile);
        User::LeaveIfError(err);
        // Destroying the file succeeded, try again to create write stream
        err = iMifWriteStream.Create(iFs, aMifFile, EFileWrite);
    }
    User::LeaveIfError(err);
}
