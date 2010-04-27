/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <baclipb.h>
#include <s32ucmp.h>
#include "swtcontrolhelper.h"
#include "swtclipboard.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtclipboard.h>
#endif

// ======== MEMBER FUNCTIONS ========


/**
 * 1st phase constructor.
 */
CSwtClipboard::CSwtClipboard(TSwtPeer aPeer)
        : iPeer(aPeer)
{
}

/**
 * 2nd phase constructor
 */
void CSwtClipboard::ConstructL()
{
    iCoeEnv = CEikonEnv::Static(); // codescanner::eikonenvstatic
    ClearL();
}

/**
 * 1st and 2nd phase constructor wrapper.
 */
CSwtClipboard* CSwtClipboard::NewL(TSwtPeer aPeer)
{
    CSwtClipboard* self = new(ELeave) CSwtClipboard(aPeer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/**
 * Destructor.
 */
CSwtClipboard::~CSwtClipboard()
{
    iCoeEnv = NULL;
}

/**
 * If the Java type is not supported, the returned
 * <code>TUid</code> will have a null value.
 */
TUid CSwtClipboard::ConvertJavaToNativeType(TInt aJavaType) const
{
    switch (aJavaType)
    {
    case KSwtClipboardTypePlaintText:
        return KClipboardUidTypePlainText;

    case KSwtClipboardTypeByte:
        return KSwtWidgetsExpandedUid;

    default:
        return TUid::Uid(aJavaType);
    }
}

HBufC8* CSwtClipboard::GetByteContentsL(TInt aDataId) const
{
    CClipboard* clipboard = NULL;
    clipboard = CClipboard::NewForReadingLC(iCoeEnv->FsSession());

    RStoreReadStream stream;
    TUid typeId(ConvertJavaToNativeType(aDataId));
    TStreamId streamId(clipboard->StreamDictionary().At(typeId));
    if (streamId == KNullStreamId)
    {
        CleanupStack::PopAndDestroy(clipboard);
        return NULL;
    }

    stream.OpenLC(clipboard->Store(), streamId);
    TInt length = stream.ReadInt32L();

    HBufC8* byteBuffer = HBufC8::NewLC(length);
    TPtr8 ptr(byteBuffer->Des());
    stream.ReadL(ptr, length);
    CleanupStack::Pop(byteBuffer);
    CleanupStack::PopAndDestroy(2, clipboard);

    return byteBuffer;
}

HBufC* CSwtClipboard::GetTextContentsL() const
{
    TPtrC ptr(SwtControlHelper::GetClipboardTextContentLC());
    if (ptr.Length() == 0)
    {
        return NULL;
    }

    HBufC* buffer = ptr.AllocL();
    CleanupStack::PopAndDestroy(); // ptr;
    return buffer;
}

TAny* CSwtClipboard::GetContentsL(TInt aDataId) const
{
    if (aDataId == KSwtClipboardTypePlaintText)
    {

        return GetTextContentsL();
    }
    else //if(aDataId==KSwtClipboardTypeByte)
    {
        return GetByteContentsL(aDataId);
    }
}

TSwtPeer CSwtClipboard::Dispose()
{
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

void CSwtClipboard::ClearL()
{
    CEikonEnv* env = iCoeEnv;
    User::LeaveIfNull(env);
    RFs& fs = env->FsSession();
    CClipboard::Clear(fs);
}

void CSwtClipboard::GetAvailableTypesL(RArray<TInt>& aTypes) const
{
    CClipboard* clipboard = NULL;
    clipboard = CClipboard::NewForReadingLC(iCoeEnv->FsSession());
    CStreamDictionary& streamDictionary = clipboard->StreamDictionary();

    for (TInt i=0; i<aTypes.Count();)
    {
        TUid typeId(ConvertJavaToNativeType(aTypes[i]));
        if (streamDictionary.At(typeId) == KNullStreamId)
            aTypes.Remove(i);
        else
            ++i;
    }

    CleanupStack::PopAndDestroy(clipboard);
}

void CSwtClipboard::SetByteContentL(const TDesC8& aPtr, TInt aJavaType)
{
    CClipboard* clipboard = NULL;
    clipboard = CClipboard::NewForWritingLC(iCoeEnv->FsSession());

    RStoreWriteStream stream;
    TStreamId streamId(stream.CreateLC(clipboard->Store()));
    stream.WriteInt32L(aPtr.Length());
    stream.WriteL(aPtr);
    stream.CommitL();

    TUid typeId(ConvertJavaToNativeType(aJavaType));
    clipboard->StreamDictionary().AssignL(typeId, streamId);
    clipboard->CommitL();
    CleanupStack::PopAndDestroy(2, clipboard);
}

void CSwtClipboard::SetTextContentL(const TDesC& aPtr)
{
    CClipboard* clipboard = NULL;
    clipboard = CClipboard::NewForWritingLC(iCoeEnv->FsSession());

    CPlainText* text = CPlainText::NewL();
    CleanupStack::PushL(text);
    text->InsertL(0, aPtr);
    text->CopyToStoreL(clipboard->Store(), clipboard->StreamDictionary(), 0, aPtr.Length());
    clipboard->CommitL();

    CleanupStack::PopAndDestroy(2, clipboard);   // text and clipboard;
}
