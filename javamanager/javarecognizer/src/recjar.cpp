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
* Description:  This file contains an implementation of Symbian MIME type recognizer
 *                for MIME types application/java-archive (.jar) and
 *                text/vnd.sun.j2me.app-descriptor (.jad)
 *
*/


#include <apmrec.h>
#include <apmstd.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "javauids.h"
#include "recjar.h"

const TUid KJavaRecognizerTUid = { KJavaRecognizerDllUid};
const TInt KJarNumMimeTypes = 2;

const TInt KMaxBufferLength = 100;
_LIT8(KDataTypeJavaArchive, "application/java-archive");
_LIT8(KDataTypeAppDescriptor, "text/vnd.sun.j2me.app-descriptor");
// First four bytes of a ZIP file
_LIT8(KPkWareLocalFileHeader, "\x50\x4B\x03\x04");
_LIT8(KMidlet,"Midlet-");

CApaJarRecognizer::CApaJarRecognizer() :
        CApaDataRecognizerType(KJavaRecognizerTUid, CApaDataRecognizerType::EHigh)
{
    iCountDataTypes = KJarNumMimeTypes;
}

TUint CApaJarRecognizer::PreferredBufSize()
{
    return KMaxBufferLength;
}

TDataType CApaJarRecognizer::SupportedDataTypeL(TInt aIndex) const
{
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < KJarNumMimeTypes, User::Invariant());
    switch (aIndex)
    {
    case 0:
        return TDataType(KDataTypeJavaArchive);
    case 1:
        return TDataType(KDataTypeAppDescriptor);
    default:
        // Should not come here, keep GCC happy
        return TDataType(KDataTypeJavaArchive);
    }
}

void CApaJarRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
{
    if (aName.Length() > 4)
    {
        if (aName.Right(4).CompareF(_L(".jad")) == 0)
        {
            if (aBuffer.Length() > 0 && aBuffer.FindF(KMidlet())
                    != KErrNotFound)
            {
                // The file contains the text "midlet-"
                // so its almost certain to be a JAD file
                iDataType = TDataType(KDataTypeAppDescriptor);
                iConfidence = ECertain;
            }
            else
            {
                // Did not find "midlet-" in the supplied buffer so
                // try searching the rest of the file...
                //
                TBool weOwnFileHandle = EFalse;
                // The following call only Leaves if user supplied file
                // handle can not be rewound, this is not worth
                // catching with a TRAP handler.
                RFile *file = FilePassedByHandleL();
                RFile ownFile;
                RFs ownFs;
                if (!file)
                {
                    // We were not passed an RFile, so try and open the file directly.
                    User::LeaveIfError(ownFs.Connect());
                    CleanupClosePushL(ownFs);
                    User::LeaveIfError(ownFile.Open(ownFs, aName, EFileRead
                                                    | EFileShareReadersOrWriters | EFileStream));
                    CleanupClosePushL(ownFile);
                    file = &ownFile;
                    weOwnFileHandle = ETrue;
                }

                TInt fileRemaining = 0;
                User::LeaveIfError(file->Size(fileRemaining));
                if (fileRemaining > aBuffer.Length())
                {
                    // File is longer than buffer, so search remainder
                    // of file.

                    // Skip the start, which has already been read
                    TInt tmp = aBuffer.Length();
                    User::LeaveIfError(file->Seek(ESeekStart, tmp));
                    fileRemaining -= tmp;

                    // Buffer for processing the file
                    TUint8 buf[100]; // Must be at least 2*(KMidlet.Length()-1)

                    // Copy up to the last KMidlet.Length()-1 bytes
                    // from aBuffer, then append file data. This
                    // handles the case where the KMidlet string
                    // straddles the boundry by a byte (eg. buffer
                    // ends with "Midlet" and we are looking for
                    // "Midlet-").
                    TInt prefixLength = KMidlet().Length() - 1;
                    if (aBuffer.Length() < prefixLength)
                    {
                        prefixLength = aBuffer.Length();
                    }
                    TInt prefixOffset = aBuffer.Length() - prefixLength;
                    if (prefixOffset < 0)
                    {
                        prefixOffset = 0;
                    }

                    if (prefixLength)
                    {
                        // Copy prefix into buffer
                        TPtr8 prefixDes(&buf[0], prefixLength);
                        prefixDes = aBuffer.Mid(prefixOffset, prefixLength);
                    }

                    while (fileRemaining > 0)
                    {
                        // Read as much file data into the buffer as possible.
                        TPtr8 bufBodyDes(&buf[0] + prefixLength, sizeof(buf)
                                         - prefixLength);
                        User::LeaveIfError(file->Read(bufBodyDes));
                        fileRemaining -= bufBodyDes.Length();

                        TPtrC8 bufDes(&buf[0], prefixLength
                                      + bufBodyDes.Length());
                        if (bufDes.FindF(KMidlet()) != KErrNotFound)
                        {
                            // The file contains the text "midlet-"
                            // so its almost certain to be a JAD file
                            iDataType = TDataType(KDataTypeAppDescriptor);
                            iConfidence = ECertain;
                            break;
                        }
                        // Not found, copy prefix to start of buffer and try again.
                        if (bufDes.Length() == sizeof(buf))
                        {
                            // Copy prefix into buffer
                            prefixLength = KMidlet().Length() - 1;
                            TPtr8 prefixDes(buf, prefixLength);
                            prefixDes = bufDes.Mid(bufDes.Length()
                                                   - prefixLength, prefixLength);
                        }
                    }
                }

                if (weOwnFileHandle)
                {
                    CleanupStack::PopAndDestroy(&ownFile);
                    CleanupStack::PopAndDestroy(&ownFs);
                }
            }
        }
        else if (aName.Right(4).CompareF(_L(".jar")) == 0)
        {
            // if the first 4 bytes of the buffer match the first 4
            // bytes of a zip file it must be a JAR file
            if (aBuffer.Length() > KPkWareLocalFileHeader().Length()
                    && KPkWareLocalFileHeader().Compare(aBuffer.Left(4)) == 0)
            {
                iDataType = TDataType(KDataTypeJavaArchive);
                iConfidence = ECertain;
            }
        }
    }

    return;
}

CApaDataRecognizerType* CApaJarRecognizer::CreateRecognizerL()
{
    return new(ELeave) CApaJarRecognizer();
}

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KRecognizerEcomImplUid, CApaJarRecognizer::CreateRecognizerL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

