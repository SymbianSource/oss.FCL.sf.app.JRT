/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CManifestReader implementation file
*
*/


#include <caf/content.h>
#include <zipfile.h>
#include <utf.h>
#include <e32cmn.h>

#include <javaattribute.h>

#include "appmngr2midletmanifestreader.h"
#include "logger.h"                         // LOG

const TInt KAllRead = -1;
const TInt KAttributeMinSize = 3;
const TInt KFilePrefix = 4;

_LIT(KManifestEntryName, "META-INF/MANIFEST.MF");

const TUint32 LF  = 10;
const TUint32 CR  = 13;
const TUint32 SP  = 32;
const TUint32 COLON = 58;


AppMngr2MidletManifestReader::AppMngr2MidletManifestReader(RFs& aFs) :
        mFs(aFs), mSessionOpen(false)
{
}

AppMngr2MidletManifestReader::~AppMngr2MidletManifestReader()
{
    // Close the file session (if opened by ManifestReader)
    if (mSessionOpen)
    {
        mFs.Close();
    }
}

void AppMngr2MidletManifestReader::ReadManifestL(
    const TDesC& aManifestFile,
    RPointerArray<MJavaAttribute>& aAttributes)
{
    if (mManifestContent.get())
    {
        mManifestContent.reset(0);
    }

    ContentL(aManifestFile);

    ReadAttributesL(aAttributes);
}

void AppMngr2MidletManifestReader::ReadAttributesL(
    RPointerArray<MJavaAttribute>& aAttributes)
{
    TPtr16 contentPtr(mManifestContent->Des());

    //remove white space from the beginning of the file
    contentPtr.TrimLeft();

    TInt attributeIndex = -1;

    // Continue until new line is found and manifest content is more than 3.
    // It can't be shorter than a:b\n. Actually spec specifies that it must be 'a: b\n'
    // that makes four but for the robustness as also user inserted variables
    // are read. Parameter can be e.g. either TestCase1:myCase or TestCase1: myCase.
    while ((attributeIndex = ReadLineIndexL(EFalse)) != KAllRead)
    {
        HBufC16* attribute = contentPtr.Left(attributeIndex).AllocLC();
        TPtr16 attributePtr(attribute->Des());
        attributePtr.Trim();

        TInt colonIndex = attributePtr.Locate(COLON);

        // Invalid attribute. Name not found
        if (KErrNotFound == colonIndex)
        {
            ELOG(EJavaAppMngrPlugin, "MF parsing failed: No attribute name defined");
            //User::Leave(KJavaParseNameAttributeMissing);
            User::Leave(KErrArgument);
        }

        // Implementation expects that attribute name fits to first line.
        HBufC16* name = attributePtr.Left(colonIndex).AllocLC();
        TPtr16 namePtr(name->Des());
        namePtr.Trim();

        ValidateAttributeNameL(namePtr);

        // Rest of the manifest data can be continuational value.
        HBufC16* value = HBufC16::NewLC(contentPtr.Length());
        TPtr16 valuePtr(value->Des());

        // Skip colon
        valuePtr.Append(attributePtr.Mid(colonIndex + 1));
        valuePtr.Trim();

        ReadContinuationLineL(valuePtr, contentPtr, attributeIndex);

        // If manifest attribute value is not defined attribute is
        // skipped and it is not returned to caller.
        if (valuePtr.Length() > 0)
        {
            CJavaAttribute* javaAttribute
            = CJavaAttribute::NewL(namePtr, valuePtr, EFalse);
            aAttributes.Append(javaAttribute);
        }

        CleanupStack::PopAndDestroy(value);
        CleanupStack::PopAndDestroy(name);
        CleanupStack::PopAndDestroy(attribute);

        // Delete already handled attribute from the buffer.
        // Content is read until the enter and it must be deleted to reach next one.
        contentPtr.Delete(0, attributeIndex + 1);
        contentPtr.TrimLeft();
    }
}

void AppMngr2MidletManifestReader::ContentL(const TDesC& aManifestFileName)
{
    RFile file;
    TInt err = file.Open(mFs,
                         aManifestFileName,
                         EFileStream | EFileShareAny | EFileRead);

    User::LeaveIfError(err);
    CleanupClosePushL(file);

    std::auto_ptr<HBufC8>fileStart(HBufC8::NewL(KFilePrefix));
    TPtr8 fileStartPtr(fileStart->Des());

    std::auto_ptr<HBufC16>filePrefix(HBufC16::NewL(KFilePrefix));
    TPtr16 filePrefixPtr(filePrefix->Des());

    err = file.Read(fileStartPtr, KFilePrefix);
    User::LeaveIfError(err);

    err = CnvUtfConverter::ConvertToUnicodeFromUtf8(
              filePrefixPtr, fileStartPtr);

    if (KErrNone != err)
    {
        ELOG1(EJavaAppMngrPlugin, "MF UTF-8 to unicode conversion failed: %d", err);
        // User::Leave(KJavaErrNoneUTF8);
        User::Leave(KErrArgument);
    }

    if (fileStartPtr.Length() < KFilePrefix)
    {
        ELOG(EJavaAppMngrPlugin, "Not enough content at manifest file");
        // User::Leave(KJavaErrInvalidManifest);
        User::Leave(KErrArgument);
    }

    // Check from CAF if file is DRM protected.
    TBool drmProtected = EFalse;
    std::auto_ptr<ContentAccess::CContent>content(
        ContentAccess::CContent::NewL(file));

    err = content->GetAttribute(ContentAccess::EIsProtected, drmProtected);
    if (KErrNone != err)
    {
        ELOG1(EJavaAppMngrPlugin, "DRM protection check failed: %d", err);
        User::Leave(KErrArgument);
    }

    // Detect JAR package. JAR is either a DRM protected file or a ZIP file.
    // ZIP file has PK\003\004 prefix.
    if (drmProtected
            || (filePrefixPtr[0] == 0x50
                && filePrefixPtr[1] == 0x4B
                && filePrefixPtr[2] == 0x03
                && filePrefixPtr[3] == 0x04))
    {
        ReadManifestContentFromPackageL(file);
    }
    else
    {
        // Plain manifest file
        ReadManifestContentL(file);
    }
    CleanupStack::PopAndDestroy(&file);
}

void AppMngr2MidletManifestReader::ReadManifestContentL(RFile& aManifestFile)
{
    // Reads the entire file into a heap descriptor. Use the content
    // access framework to access the manifest.
    ContentAccess::CContent* content
    = ContentAccess::CContent::NewLC(aManifestFile);
    ContentAccess::CData* manifestFile
    = content->OpenContentL(ContentAccess::EPeek);
    CleanupStack::PushL(manifestFile);

    HBufC8* manifestFileData = ReadRawManifestFileL(*manifestFile);
    TPtr8 ptr(manifestFileData->Des());

    CleanupStack::PopAndDestroy(manifestFile);
    CleanupStack::PopAndDestroy(content);
    CleanupStack::PushL(manifestFileData);

    mManifestContent.reset(HBufC16::NewL(ptr.Size()));
    TPtr16 ucsPtr(mManifestContent->Des());
    TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(ucsPtr, ptr);

    if (KErrNone != err)
    {
        ELOG1(EJavaAppMngrPlugin, "MF UTF-8 to unicode conversion failed: %d", err);
        //User::Leave(KJavaErrNoneUTF8);
        User::Leave(KErrArgument);
    }
    CleanupStack::PopAndDestroy(manifestFileData);
}

HBufC8* AppMngr2MidletManifestReader::ReadRawManifestFileL(
    ContentAccess::CData& aManifestFile)
{
    // Reads size of file
    TInt manifestSize;
    aManifestFile.DataSizeL(manifestSize);
    // allocates buffer for file contents, if this is too big and device runs
    // out of memory function will leave. Do not really care about this as file
    // was unlikely to have been a manifest file at all.
    HBufC8* manifestFileData = HBufC8::NewMaxLC(manifestSize);
    TPtr8 manifestFileDataPtr(manifestFileData->Des());

    // Now read the manifest file
    User::LeaveIfError(aManifestFile.Read(manifestFileDataPtr, manifestSize));
    CleanupStack::Pop(manifestFileData);

    return manifestFileData;
}

void AppMngr2MidletManifestReader::ReadManifestContentFromPackageL(
    RFile& aPackageFile)
{
    CZipFile* zipFile = CZipFile::NewL(mFs, aPackageFile);
    CleanupStack::PushL(zipFile);

    // Seek manifest file
    CZipFileMember* zippedFile =
        zipFile->CaseSensitiveOrCaseInsensitiveMemberL(KManifestEntryName());

    if (!zippedFile)
    {
        ELOG(EJavaAppMngrPlugin, "Package is missing manifest");
        //User::LeaveIfError(KJavaErrMissingManifest);
        User::Leave(KErrArgument);
    }
    CleanupStack::PushL(zippedFile);

    TUint uncompressedSize = zippedFile->UncompressedSize();
    if ((TUint)uncompressedSize>=(KMaxTInt/2))
    {
        ELOG(EJavaAppMngrPlugin, "Invalid manifest");
        //User::Leave(KJavaErrInvalidManifest);
        User::Leave(KErrArgument);
    }
    HBufC8* resultData = HBufC8::NewLC(uncompressedSize);

    RZipFileMemberReaderStream* zippedStream = 0;
    TInt err = zipFile->GetInputStreamL(zippedFile, zippedStream);
    User::LeaveIfError(err);
    CleanupStack::PushL(zippedStream);

    TPtr8 ptr(resultData->Des());
    User::LeaveIfError(zippedStream->Read(ptr, uncompressedSize));

    CleanupStack::PopAndDestroy(zippedStream);

    mManifestContent.reset(HBufC16::NewL(uncompressedSize));
    TPtr16 ucsPtr(mManifestContent->Des());
    err = CnvUtfConverter::ConvertToUnicodeFromUtf8(ucsPtr, ptr);

    if (KErrNone != err)
    {
        ELOG1(EJavaAppMngrPlugin, "UTF-8 to unicode conversion failed: %d", err);
        //User::Leave(KJavaErrNoneUTF8);
        User::Leave(KErrArgument);
    }

    CleanupStack::PopAndDestroy(resultData);
    CleanupStack::PopAndDestroy(zippedFile);
    CleanupStack::PopAndDestroy(zipFile);
}

TInt AppMngr2MidletManifestReader::ReadLineIndexL(TBool aContinuation)
{
    TPtr16 contentPtr(mManifestContent->Des());

    if (contentPtr.Length() < KAttributeMinSize)
    {
        // EOF. This covers also case where all rest of the data
        // was whitespace characters trimmed were trimmed off before
        // going to next line.
        return KAllRead;
    }

    TInt attributeIndex = contentPtr.Locate(LF);
    TInt tmp = contentPtr.Locate(CR);

    // New line can be CR LF | LF | CR. Let's take
    // the first appearance of new line character.
    if ((tmp < attributeIndex && tmp != KErrNotFound)
            || attributeIndex == KErrNotFound)
    {
        // This covers also new line indicated only with CR.
        attributeIndex = tmp;
    }

    if (KErrNotFound == attributeIndex && KErrNotFound == tmp)
    {
        // Check if no enter after last line
        if (contentPtr.Length() > KAttributeMinSize)
        {
            attributeIndex = contentPtr.Length();
        }
        else
        {
            ELOG(EJavaAppMngrPlugin, "No attribute found");
            //User::Leave(KJavaErrInvalidManifest);
            User::Leave(KErrArgument);
        }
    }
    else if ((attributeIndex < KAttributeMinSize) && !aContinuation)
    {
        // Minimum parameter length is 4 e.g. 'a: b\n' or 'a:b\n'
        // This is a bit more robust than manifest specification as
        // it expects nameCOLONSPACEvalue format.
        ELOG(EJavaAppMngrPlugin, "Invalid attribute");
        //User::Leave(KJavaErrInvalidManifest);
        User::Leave(KErrArgument);
    }
    else if (attributeIndex <= 0)
    {
        // Minimum continuation parameter length is 1 e.g. 'a\n'
        ELOG(EJavaAppMngrPlugin, "Invalid continuation attribute");
        //User::Leave(KJavaErrInvalidManifest);
        User::Leave(KErrArgument);
    }

    return attributeIndex;
}

void AppMngr2MidletManifestReader::ReadContinuationLineL(TDes16& aValue,
        TDes& aContent,
        TInt& aIndex)
{
    TBool continuation = ETrue;

    while (continuation)
    {
        if (aContent.Length() <= aIndex + 2)
        {
            // Last line
            continuation = EFalse;
        }
        // Continuation can be CR LF SPACE | LF SPACE | CR SPACE
        else if ((aContent[ aIndex ] == CR
                  && aContent[ aIndex + 1 ] == LF
                  && aContent[ aIndex + 2 ] == SP)
                 ||
                 (aContent[ aIndex ] == CR
                  && aContent[ aIndex + 1 ] == SP)
                 ||
                 (aContent[ aIndex ] == LF
                  && aContent[ aIndex + 1 ] == SP))
        {
            // Clean previous entry.
            aContent.Delete(0, aIndex + 1);

            // Delete continuation indicator. Trim cannot be used here. If continuation
            // starts with SP it will be trimmed out and that must be prevented.

            // CR SP or LF SP case
            if (aContent[0] == SP)
            {
                if ((aContent.Length() > 1) &&
                        (aContent[1] != CR) && (aContent[1] != LF))
                {
                    aContent.Delete(0, 1);//for Manifest i JAD
                }
            }
            // CR LF SP case
            else
            {
                if ((aContent.Length() > 2) &&
                        (aContent[2] != CR) && (aContent[2] != LF))
                {
                    aContent.Delete(0, 2);//for Manifest i JAD
                }
                else
                {
                    aContent.Delete(0, 1);//for Manifest i JAD
                }
            }

            aIndex = ReadLineIndexL(ETrue);
            if (aIndex != KAllRead)
            {
                // Do not append whitespace combination. Only content.
                aValue.Append(aContent.Left(aIndex));
                aValue.TrimRight();
            }
            else
            {
                // Can't be continuation if all read. Not leaving
                // as last line can easilly contain unnecessary SPACEs.
                continuation = EFalse;
            }
        }
        else
        {
            continuation = EFalse;
        }
    }
}

void AppMngr2MidletManifestReader::ValidateAttributeNameL(TDesC16& aName)
{
    if (aName.Length() == 0)
    {
        ELOG(EJavaAppMngrPlugin, "Invalid attribute name");
        // User::Leave(KJavaParseInvalidAttributeName);
        User::Leave(KErrArgument);
    }
    TLex lexer(aName);
    TChar ch;

    // Manifest name cannot start with - and _ chars.
    ch = lexer.Get();

    if (ch == '_' || ch == '-')
    {
        ELOG(EJavaAppMngrPlugin, "Invalid attribute name start");
        //User::Leave(KJavaParseInvalidAttributeName);
        User::Leave(KErrArgument);

    }

    lexer.UnGet();

    while ((ch = lexer.Get()) != '\0')
    {
        // Allowed chars: {A-Z} | {a-z} | {0-9} | - | _
        if (!('0' <= ch && ch <= '9')
                && !(ch >= 'a' && ch <= 'z')
                && !(ch >= 'A' && ch <= 'Z')
                && !('-' == ch)
                && !('_' == ch)
                && !('.' == ch))
        {
            ELOG(EJavaAppMngrPlugin, "Invalid attribute name");
            //User::Leave(KJavaParseInvalidAttributeName);
            User::Leave(KErrArgument);
        }
    }
}
