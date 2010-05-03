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


#include "org_eclipse_ercp_swt_expanded_internal_OS.h"
#include "eswtwidgetsexpanded.h"
#include "templatehelpers.h"
#include "utils.h"
#include "swtdialogbroker.h"


// ======== LOCAL FUNCTIONS ========


/**
 * Helper function for creating an instance of an expanded widget
 */
template<class C>
static jint NewExpandedWidget(JNIEnv* aJniEnv, C*(MSwtFactory::*aMethodL)(MSwtDisplay&, TSwtPeer, MSwtComposite&, TInt) const, jobject aPeer, jint aParent, jint aStyle)
{
    CSwtDisplay& display = CSwtDisplay::Current();
    const MSwtFactory* factory = &display.Factory();
    return NewWidget(aJniEnv, factory, aMethodL, display, aPeer, aParent, aStyle);
}


extern "C"
{

    /*
     * Class Clipboard
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1New
    (JNIEnv* aJniEnv, jclass, jobject aPeer)
    {
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        CSwtDisplay&   display = CSwtDisplay::Current();
        MSwtClipboard* result  = NULL;
        TRAPD(error, CallMethodL(result, &display.Factory(), &MSwtFactory::NewClipboardL, peerRef));
        ThrowIfError(error, aJniEnv);

        INCREASE_INSTANCE_COUNT(result, Clipboard);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtClipboard* clipboard = reinterpret_cast<MSwtClipboard*>(aHandle);
        TSwtPeer peerRef;
        CallMethod(peerRef, clipboard, &MSwtClipboard::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(Clipboard);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1Clear(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtClipboard* clipboard = reinterpret_cast<MSwtClipboard*>(aHandle);
        TRAPD(error, CallMethodL(clipboard, &MSwtClipboard::ClearL));
        ThrowIfError(error, aJniEnv);
    }

    static jintArray OS_Clipboard_1GetAvailableTypesL(JNIEnv* aJniEnv, jint aHandle , jintArray aTypesArray)
    {
        RArray<TInt> typesArray;
        CleanupClosePushL(typesArray);
        ConvertIntegerArrayL(aJniEnv, aTypesArray, typesArray);

        const MSwtClipboard* clipboard = reinterpret_cast<MSwtClipboard*>(aHandle);
        CallMethodL(clipboard, &MSwtClipboard::GetAvailableTypesL, typesArray);

        TInt  count     = typesArray.Count();
        TInt* arrayBase = (count!=0) ? &(typesArray[0]) : NULL;
        jintArray javaTypeArray = NewJavaIntArray(aJniEnv, arrayBase, count);

        CleanupStack::PopAndDestroy(&typesArray);
        return javaTypeArray;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1GetAvailableTypes
    (JNIEnv* aJniEnv, jclass, jint aHandle , jintArray aTypesArray)
    {
        jintArray javaTypeArray = 0;
        TRAPD(error, (javaTypeArray = OS_Clipboard_1GetAvailableTypesL(aJniEnv, aHandle, aTypesArray)));
        ThrowIfError(error, aJniEnv);
        return javaTypeArray;
    }

    static jobject GetClipboardTextContentsL(JNIEnv* aJniEnv, const MSwtClipboard* aClipboard)
    {
        HBufC* textBuffer=NULL;
        CallMethodL(textBuffer, aClipboard, &MSwtClipboard::GetTextContentsL);
        jobject result = NULL;
        if (textBuffer!=NULL)
        {
            result = NewJavaString(aJniEnv, *textBuffer);
            DeleteInUiHeap(textBuffer);
        }
        return result;
    }

    static jobject GetClipboardByteContentsL(JNIEnv* aJniEnv, const MSwtClipboard* aClipboard, jint aTypeId)
    {
        HBufC8* byteBuffer=NULL;
        CallMethodL(byteBuffer, aClipboard, &MSwtClipboard::GetByteContentsL, aTypeId);
        jobject result = NULL;
        if (byteBuffer!=NULL)
        {
            result= NewJavaByteArray(aJniEnv, *byteBuffer);
            DeleteInUiHeap(byteBuffer);
        }
        return result;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1GetContents
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aTypeId)
    {
        const MSwtClipboard* clipboard = reinterpret_cast<MSwtClipboard*>(aHandle);
        jobject result=NULL;
        TInt error=KErrNone;
        if (aTypeId == KSwtClipboardTypePlaintText)
        {
            TRAP(error, (result=GetClipboardTextContentsL(aJniEnv, clipboard)));
        }
        else
        {
            TRAP(error, (result=GetClipboardByteContentsL(aJniEnv, clipboard, aTypeId)));
        }
        ThrowIfError(error, aJniEnv);
        return result;
    }

    static void SetByteContentToClipBoardL(JNIEnv* aJniEnv, jint aHandle, jobject aObject, jint aTypeId)
    {
        TInt length = aJniEnv->GetArrayLength(static_cast<jbyteArray>(aObject));
        HBufC8* buffer = HBufC8::NewMaxL(length);
        CleanupStack::PushL(buffer);

        // Copy the data to the newly allocated buffer
        aJniEnv->GetByteArrayRegion(static_cast<jbyteArray>(aObject), 0, length, (jbyte*)(buffer->Ptr()));

        MSwtClipboard* clipboard = reinterpret_cast<MSwtClipboard*>(aHandle);
        CallMethodL(clipboard, &MSwtClipboard::SetByteContentL, *buffer, aTypeId);
        CleanupStack::PopAndDestroy(buffer);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Clipboard_1SetContents
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aTypeId, jobject aObject)
    {
        if (aTypeId == KSwtClipboardTypePlaintText)
        {
            CallTextMethodThrow(aJniEnv, aHandle, &MSwtClipboard::SetTextContentL, static_cast<jstring>(aObject));
            return;
        }

        TRAPD(error, SetByteContentToClipBoardL(aJniEnv, aHandle, aObject, aTypeId));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class ColorDialog
     */
    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_ColorDialog_1Open(JNIEnv* aJniEnv, jclass, jobject aPeer, jobject aColor)
    {

        TInt red(-1);
        TInt green(-1);
        TInt blue(-1);
        // Transform the RGB
        if (aColor != NULL)
        {
            jclass classRGB = aJniEnv->GetObjectClass(aColor);

            if (classRGB != NULL)
            {
                TBool failed = EFalse;
                red   = GetIntField(aJniEnv, aColor, classRGB, "red",   failed);
                green = GetIntField(aJniEnv, aColor, classRGB, "green", failed);
                blue  = GetIntField(aJniEnv, aColor, classRGB, "blue",  failed);

                aJniEnv->DeleteLocalRef(classRGB);
            }
        }


        CSwtDisplay& display = CSwtDisplay::Current();
        TRAPD(error, display.RequestRunDialogL(reinterpret_cast< TSwtPeer >(aPeer),
                                               CSwtDialogBroker::EColorDialog, 0, KNullDesC(), KNullDesC(), KNullDesC(), red, green, blue));

        ThrowIfError(error, aJniEnv);

    }


    /*
     * Class FontDialog
     */
    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_FontDialog_1GetCommandOkTitle(JNIEnv* aJniEnv, jclass)
    {
        const MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();
        const HBufC* buf = NULL;
        TRAPD(error, CallMethodL(buf, &utils, &MSwtUiUtils::GetCommandOkTitleL));
        ThrowIfError(error, aJniEnv);
        jstring result = NULL;
        if (buf)
        {
            result = NewJavaString(aJniEnv, *buf);
        }
        DeleteInUiHeap(buf);
        return result;
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_FontDialog_1GetCommandCancelTitle(JNIEnv* aJniEnv, jclass)
    {
        const MSwtUiUtils& utils = CSwtDisplay::Current().UiUtils();
        const HBufC* buf = NULL;
        TRAPD(error, CallMethodL(buf, &utils, &MSwtUiUtils::GetCommandCancelTitleL));
        ThrowIfError(error, aJniEnv);
        jstring result = NULL;
        if (buf)
        {
            result = NewJavaString(aJniEnv, *buf);
        }
        DeleteInUiHeap(buf);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_FontDialog_1GetFontNames(JNIEnv* aJniEnv, jclass, jboolean aScalable)
    {
        const MSwtDisplay& display = CSwtDisplay::Current();
        // This method is responsible for deleting the array
        CDesC16ArrayFlat* array = NULL;
        TRAPD(error, CallMethodL(array, &display, &MSwtDisplay::GetFontNamesL, aScalable));
        ThrowIfError(error, aJniEnv);

        jobjectArray javaStringArray = NULL;
        if (array)
        {
            jclass stringClass = aJniEnv->FindClass("java/lang/String");
            if (stringClass)
            {
                const TInt count = array->Count();
                javaStringArray = aJniEnv->NewObjectArray(count, stringClass, NULL);
                if (javaStringArray)
                {
                    jstring javaString;
                    for (TInt i=0; i<count; ++i)
                    {
                        javaString = NewJavaString(aJniEnv, (*array)[i]);
                        aJniEnv->SetObjectArrayElement(javaStringArray, i, javaString); //lint !e613
                        aJniEnv->DeleteLocalRef(javaString);
                    }
                }
                aJniEnv->DeleteLocalRef(stringClass);
            }
            delete array;
        }
        return javaStringArray;
    }


    /*
     * Class Table
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewExpandedWidget(aJniEnv, &MSwtFactory::NewTableL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Table);
        return handle;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetTableItems(JNIEnv *aJniEnv, jclass, jint aHandle)
    {
        MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        CSwtPeerArray* items  = NULL;
        TRAPD(error, CallMethodL(items, table, &MSwtTable::GetTableItemsL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaObjectArray(aJniEnv, items, "org/eclipse/swt/widgets/TableItem");
        DeleteInUiHeap(items);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetTableColumns(JNIEnv *aJniEnv, jclass, jint aHandle)
    {
        MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        CSwtPeerArray* items  = NULL;
        TRAPD(error, CallMethodL(items, table, &MSwtTable::GetTableColumnsL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaObjectArray(aJniEnv, items, "org/eclipse/swt/widgets/TableColumn");
        DeleteInUiHeap(items);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1CompositeHandle(JNIEnv *, jclass, jint aHandle)
    {
        MSwtTable*    table = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtComposite* result;
        CallMethod(result, table, &MSwtTable::Composite);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Deselect__III(JNIEnv*, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::DeselectRange, aStart, aEnd);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Deselect__I_3I(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::DeselectItems, ints, count);
        delete[] ints;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1IndexOfColumn(JNIEnv *, jclass, jint aHandle, jint aColumnHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtTableColumn* theTableColumn = reinterpret_cast<MSwtTableColumn*>(aColumnHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::IndexOfTableColumn, theTableColumn);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1IndexOfItem__III(JNIEnv *, jclass, jint aHandle, jint aItemHandle, jint aStart)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtTableItem* theTableItem = reinterpret_cast<MSwtTableItem*>(aItemHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::IndexOfTableItemAt, theTableItem, aStart);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1IndexOfItem__II(JNIEnv *, jclass, jint aHandle, jint aItemHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtTableItem* theTableItem = reinterpret_cast<MSwtTableItem*>(aItemHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::IndexOfTableItem, theTableItem);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetColumnCount(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt count;
        CallMethod(count, theTable, &MSwtTable::GetTableColumnCount);
        return count;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetHeaderVisible(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        TBool isVisible;
        CallMethod(isVisible, table, &MSwtTable::GetHeaderVisible);
        return ConvertBoolean(isVisible);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetLinesVisible(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        TBool isVisible;
        CallMethod(isVisible, table, &MSwtTable::GetLinesVisible);
        return ConvertBoolean(isVisible);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetColumn(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        const MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        TSwtPeer result(NULL);
        TRAPD(error, CallMethodL(result, table, &MSwtTable::GetColumnL, aIndex));
        ThrowIfError(error, aJniEnv);
        return result;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetItem(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        const MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        TSwtPeer result(NULL);
        TRAPD(error, CallMethodL(result, table, &MSwtTable::GetItemL, aIndex));
        ThrowIfError(error, aJniEnv);
        return result;
    }


    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetItemCount(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt count;
        CallMethod(count, theTable, &MSwtTable::GetItemCount);
        return count;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Remove__II(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::RemoveItemL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Remove__III(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::RemoveRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Remove__I_3I(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::RemoveItemsL, ints, count));
        delete[] ints;
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1RemoveAll(JNIEnv*, jclass, jint aHandle)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::RemoveAll);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Select__IIZ(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex, jboolean aScroll)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::SelectItemL, aIndex, aScroll));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1Select__III(JNIEnv* aJniEnv, jclass, jint aHandle, jint aStart, jint aEnd)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::SelectRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1SelectAll(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TRAPD(error, CallMethodL(theTable, &MSwtTable::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1SetFocusIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::SetFocusIndex, aIndex);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetSelectionCount(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt count;
        CallMethod(count, theTable, &MSwtTable::GetSelectionCount);
        return count;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetSelectionIndices(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        const CArrayFix<TInt>* selectionArray = NULL;
        TRAPD(error, CallMethodL(selectionArray, theTable, &MSwtTable::GetSelectionIndicesL));
        ThrowIfError(error, aJniEnv);
        TInt count = selectionArray ? selectionArray->Count() : 0;
        jintArray result = aJniEnv->NewIntArray(count);
        if (!result)
        {
            return NULL;
        }
        for (TInt i=0; i<count; ++i)
        {
            TInt value = (*selectionArray)[i]; //lint !e613
            aJniEnv->SetIntArrayRegion(result, i, 1, &value);
        }
        DeleteInUiHeap(selectionArray);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1SetHeaderVisible(JNIEnv*, jclass, jint aHandle, jboolean aVisible)
    {
        MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(table, &MSwtTable::SetHeaderVisible, aVisible);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1SetLinesVisible(JNIEnv*, jclass, jint aHandle, jboolean aVisible)
    {
        MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(table, &MSwtTable::SetLinesVisible, aVisible);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1ShowSelection(JNIEnv*, jclass, jint aHandle)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::ShowSelection);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetSelectionIndex(JNIEnv*, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::GetSelectionIndex);
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1IsSelected(JNIEnv *, jclass, jint aHandle, jint aIndex)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TBool result;
        CallMethod(result, theTable, &MSwtTable::IsSelected,aIndex);
        return ConvertBoolean(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetTopIndex(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::GetTopIndex);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetSelection(JNIEnv *aJniEnv, jclass, jint aHandle)
    {
        MSwtTable* composite = reinterpret_cast<MSwtTable*>(aHandle);
        CSwtPeerArray* items  = NULL;
        TRAPD(error, CallMethodL(items, composite, &MSwtTable::GetSelectionL));
        if (error)
        {
            Throw(error, aJniEnv);
            return NULL;
        }

        jobjectArray result = NewJavaObjectArray(aJniEnv, items, "org/eclipse/swt/widgets/TableItem");
        DeleteInUiHeap(items);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1SetTopIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        CallMethod(theTable, &MSwtTable::SetTopIndex, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1ShowColumn(JNIEnv *, jclass, jint aHandle, jint aColumnHandle)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtTableColumn* theTableColumn = reinterpret_cast<MSwtTableColumn*>(aColumnHandle);
        CallMethod(theTable, &MSwtTable::ShowTableColumn, *theTableColumn);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1ShowItem(JNIEnv *, jclass, jint aHandle, jint aTableItemHandle)
    {
        MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        MSwtTableItem* theTableItem = reinterpret_cast<MSwtTableItem*>(aTableItemHandle);
        CallMethod(theTable, &MSwtTable::ShowTableItem, *theTableItem);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetItemHeight(JNIEnv *, jclass, jint aHandle)
    {
        const MSwtTable* theTable = reinterpret_cast<MSwtTable*>(aHandle);
        TInt result;
        CallMethod(result, theTable, &MSwtTable::GetItemHeight);
        return result;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetItemFromPoint(JNIEnv *, jclass, jint aHandle, jint aX, jint aY)
    {
        const MSwtTable* table = reinterpret_cast<MSwtTable*>(aHandle);
        TSwtPeer result;
        TPoint p(aX,aY);
        CallMethod(result, table, &MSwtTable::GetItemFromPoint, p);
        return result;
    }


    /*
     * Class TableColumn
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle, jint aIndex)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();

        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtTable* parent = reinterpret_cast<MSwtTable*>(aParent);
        MSwtTableColumn * handle =NULL;
        TRAPD(error, CallMethodL(handle, factory, &MSwtFactory::NewTableColumnL,
                                 peerRef, *parent, aStyle, aIndex));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
            return 0;
        }

        INCREASE_INSTANCE_COUNT(handle, TableColumn);
        return reinterpret_cast<jint>(handle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtTableColumn* tableColumn = reinterpret_cast<MSwtTableColumn*>(aHandle);
        TSwtPeer         peerRef;
        CallMethod(peerRef, tableColumn, &MSwtTableColumn::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(TableColumn);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1GetAlignment(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtTableColumn::Alignment);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1GetWidth(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtTableColumn::Width);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1Pack(JNIEnv*, jclass, jint aHandle)
    {
        MSwtTableColumn* column = reinterpret_cast<MSwtTableColumn*>(aHandle);
        CallMethod(column, &MSwtTableColumn::Pack);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1SetAlignment(JNIEnv*, jclass, jint aHandle, jint aAlignment)
    {
        MSwtTableColumn* column = reinterpret_cast<MSwtTableColumn*>(aHandle);
        CallMethod(column, &MSwtTableColumn::SetAlignment, aAlignment);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1SetImage(JNIEnv*, jclass, jint aHandle, jint aImageHandle)
    {
        MSwtTableColumn* tableColumn = reinterpret_cast<MSwtTableColumn*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        CallMethod(tableColumn, &MSwtTableColumn::SetColumnImage, image);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1GetText(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const TDesC* text = CallNonLeavingGetter(aHandle, &MSwtTableColumn::GetText);
        ASSERT(text!=NULL);
        return NewJavaString(aJniEnv, *text);
    }

    static void TableColumn_SetTextL(JNIEnv* aJniEnv, MSwtTableColumn* aTableColumn, jstring aText)
    {
        HBufC* convertedText = ConvertStringLC(aJniEnv, aText);
        CallMethodL(aTableColumn, &MSwtTableColumn::SetTextL, *convertedText);
        CleanupStack::PopAndDestroy(convertedText);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1SetText(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        MSwtTableColumn* tableColumn = reinterpret_cast<MSwtTableColumn*>(aHandle);
        TRAPD(error, TableColumn_SetTextL(aJniEnv, tableColumn, aText));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1SetWidth(JNIEnv*, jclass, jint aHandle, jint aWidth)
    {
        MSwtTableColumn* column = reinterpret_cast<MSwtTableColumn*>(aHandle);
        CallMethod(column, &MSwtTableColumn::SetWidth, aWidth);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetGridLineWidth(JNIEnv *, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtTable::GetGridLineWidth);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Table_1GetHeaderHeight(JNIEnv *, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtTable::GetHeaderHeight);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1GetResizable(JNIEnv*, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtTableColumn::GetResizable));
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1SetResizable(JNIEnv*, jclass, jint aHandle, jboolean aResizable)
    {
        MSwtTableColumn* column = reinterpret_cast<MSwtTableColumn*>(aHandle);
        CallMethod(column, &MSwtTableColumn::SetResizable, aResizable);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableColumn_1GetStyle(JNIEnv*, jclass, jint aHandle)
    {
        return CallNonLeavingGetter(aHandle, &MSwtTableColumn::GetStyle);
    }


    /*
     * Class TableItem
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1New(JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint /*aStyle*/, jint aIndex)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();

        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtTable* parent = reinterpret_cast<MSwtTable*>(aParent);
        MSwtTableItem* handle = NULL;
        TRAPD(error, CallMethodL(handle, factory, &MSwtFactory::NewTableItemL,
                                 display, peerRef, *parent, aIndex));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
            return 0;
        }

        INCREASE_INSTANCE_COUNT(handle, TableItem);
        return reinterpret_cast<jint>(handle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1Dispose(JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        TSwtPeer peerRef;
        CallMethod(peerRef, tableItem, &MSwtTableItem::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(TableItem);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetBackground__I(JNIEnv* aJniEnv, jclass aClass, jint aHandle)
    {
        return Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetBackground__II(aJniEnv, aClass, aHandle, 0);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetBackground__II(JNIEnv *, jclass, jint aHandle, jint aColumnIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color;
        CallMethod(color, tableItem, &MSwtTableItem::GetBackground, aColumnIndex);
        INCREASE_INSTANCE_COUNT(color, Color);
        return reinterpret_cast<jint>(color);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetBounds(JNIEnv* aJniEnv, jclass, jint aHandle, jint aIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        TRect result(TRect::EUninitialized);
        CallMethod(result, tableItem, &MSwtTableItem::GetBounds, aIndex);
        return NewJavaRectangle(aJniEnv, result);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetChecked(JNIEnv*, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtTableItem::GetChecked));
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetFont__I(JNIEnv* aJniEnv, jclass aClass, jint aHandle)
    {
        return Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetFont__II(aJniEnv, aClass, aHandle, 0);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetFont__II(JNIEnv *, jclass, jint aHandle, jint aColumnIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtFont* font;
        CallMethod(font, tableItem, &MSwtTableItem::GetFont, aColumnIndex);
        ASSERT(font!=NULL);
        return reinterpret_cast<jint>(font);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetForeground__I(JNIEnv* aJniEnv, jclass aClass, jint aHandle)
    {
        return Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetForeground__II(aJniEnv, aClass, aHandle, 0);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetForeground__II(JNIEnv *, jclass, jint aHandle, jint aColumnIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color;
        CallMethod(color, tableItem, &MSwtTableItem::GetForeground, aColumnIndex);
        INCREASE_INSTANCE_COUNT(color, Color);
        return reinterpret_cast<jint>(color);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetGrayed(JNIEnv*, jclass, jint aHandle)
    {
        return ConvertBoolean(CallNonLeavingGetter(aHandle, &MSwtTableItem::GetGrayed));
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetImageBounds(JNIEnv* aJniEnv, jclass, jint aHandle, jint aColumnIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        TRect result(TRect::EUninitialized);
        CallMethod(result, tableItem, &MSwtTableItem::GetImageBounds, aColumnIndex);
        return NewJavaRectangle(aJniEnv, result);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1GetText(JNIEnv* aJniEnv, jclass, jint aHandle, jint aColumnIndex)
    {
        const MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const TDesC* text;
        CallMethod(text, tableItem, &MSwtTableItem::GetText, aColumnIndex);
        ASSERT(text!=NULL);
        return NewJavaString(aJniEnv, *text);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetBackground__II(JNIEnv*, jclass, jint aHandle, jint aColorHandle)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(tableItem, &MSwtTableItem::SetBackgroundForAll, color);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetBackground__III(JNIEnv*, jclass, jint aHandle, jint aColorHandle, jint aColumnIndex)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(tableItem, &MSwtTableItem::SetBackground, aColumnIndex, color);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetChecked(JNIEnv*, jclass, jint aHandle, jboolean aChecked)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        CallMethod(tableItem, &MSwtTableItem::SetChecked, aChecked);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetFont__II(JNIEnv*, jclass, jint aHandle, jint aFontHandle)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        MSwtFont* font = reinterpret_cast<MSwtFont*>(aFontHandle);
        CallMethod(tableItem, &MSwtTableItem::SetFontForAll, font);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetFont__III(JNIEnv*, jclass, jint aHandle, jint aFontHandle, jint aColumnIndex)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        MSwtFont* font = reinterpret_cast<MSwtFont*>(aFontHandle);
        CallMethod(tableItem, &MSwtTableItem::SetFont, aColumnIndex, font);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetForeground__II(JNIEnv*, jclass, jint aHandle, jint aColorHandle)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(tableItem, &MSwtTableItem::SetForegroundForAll, color);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetForeground__III(JNIEnv*, jclass, jint aHandle, jint aColorHandle, jint aColumnIndex)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        const MSwtColor* color = reinterpret_cast<MSwtColor*>(aColorHandle);
        CallMethod(tableItem, &MSwtTableItem::SetForeground, aColumnIndex, color);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetGrayed(JNIEnv*, jclass, jint aHandle, jboolean aIsGrayed)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        CallMethod(tableItem, &MSwtTableItem::SetGrayed, aIsGrayed);
    }

    static void TableItem_SetImageL(JNIEnv* aJniEnv, MSwtTableItem* aItem, jintArray aImageHandles)
    {
        TInt count = aJniEnv->GetArrayLength(aImageHandles);
        if (count == 0)
            return;

        RPointerArray<MSwtImage> imageArray;
        CleanupClosePushL(imageArray);

        for (TInt index=0; index<count; ++index)
        {
            TInt imageHandle;
            aJniEnv->GetIntArrayRegion(aImageHandles, index, 1, &imageHandle);
            User::LeaveIfError(imageArray.Append(reinterpret_cast<MSwtImage*>(imageHandle)));
        }

        CallMethod(aItem, &MSwtTableItem::SetImageArray, imageArray);

        CleanupStack::PopAndDestroy(); // imageArray
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetImage__I_3I(JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aImageHandles)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        TRAPD(error, TableItem_SetImageL(aJniEnv, tableItem, aImageHandles));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetImage__III(JNIEnv*, jclass, jint aHandle, jint aIndex, jint aImageHandle)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        CallMethod(tableItem, &MSwtTableItem::SetImageByIndex, aIndex, image);
    }

    static void TableItem_SetTextArrayL(JNIEnv* aJniEnv, jint aHandle, jobjectArray aStringArray)
    {
        CDesCArray* resultArray = ConvertStringArrayL(aJniEnv, aStringArray);
        if (resultArray == NULL) return;
        CleanupStack::PushL(resultArray);

        MSwtTableItem* theList = reinterpret_cast<MSwtTableItem*>(aHandle);
        CallMethodL(theList, &MSwtTableItem::SetTextArrayL, resultArray);
        CleanupStack::PopAndDestroy(resultArray);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetText__I_3Ljava_lang_String_2(JNIEnv* aJniEnv, jclass, jint aHandle, jobjectArray aStringArray)
    {
        TRAPD(error, (TableItem_SetTextArrayL(aJniEnv, aHandle, aStringArray)));
        ThrowIfError(error, aJniEnv);
    }

    static void TableItem_SetTextL(JNIEnv* aJniEnv, MSwtTableItem* aTableItem, jstring aText, jint aIndex)
    {
        HBufC* convertedText;
        convertedText = ConvertStringLC(aJniEnv, aText);
        ASSERT(convertedText);
        CallMethodL(aTableItem, &MSwtTableItem::SetTextL, aIndex, *convertedText);
        CleanupStack::PopAndDestroy(convertedText);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_TableItem_1SetText__ILjava_lang_String_2I(JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText, jint aIndex)
    {
        MSwtTableItem* tableItem = reinterpret_cast<MSwtTableItem*>(aHandle);
        TRAPD(error, TableItem_SetTextL(aJniEnv, tableItem, aText, aIndex));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Tree
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1New
    (JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewExpandedWidget(aJniEnv, &MSwtFactory::NewTreeL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Tree);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1CompositeHandle
    (JNIEnv*, jclass, jint aHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        MSwtComposite* composite;
        CallMethod(composite, tree, &MSwtTree::Composite);
        return reinterpret_cast<jint>(composite);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Add
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aItemHandle)
    {
        TInt res = -1;
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        TRAPD(error, CallMethodL(res, tree, &MSwtTree::AddL, aItemHandle));
        ThrowIfError(error, aJniEnv);
        return res;
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Bounds
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aItemHandle)
    {
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        TRect res(TRect::EUninitialized);
        CallMethod(res, tree, &MSwtTree::Bounds, aItemHandle);
        return NewJavaRectangle(aJniEnv, res);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Check
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle, jboolean aState)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::Check, aItemHandle, aState);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Collapse
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::Collapse, aItemHandle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Expand
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::Expand, aItemHandle);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Insert
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aItemHandle, jint aIndex)
    {
        TInt res = -1;
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        TRAPD(error, CallMethodL(res, tree, &MSwtTree::InsertL, aItemHandle, aIndex));
        ThrowIfError(error, aJniEnv);
        return res;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1IsExpanded
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle)
    {
        TBool result = EFalse;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(result, tree, &MSwtTree::IsExpanded, aItemHandle);
        return ConvertBoolean(result);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1IsSelected
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle)
    {
        TBool result = EFalse;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(result, tree, &MSwtTree::IsSelected, aItemHandle);
        return ConvertBoolean(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1ItemAt
    (JNIEnv*, jclass, jint aHandle, jint aX, jint aY)
    {
        TInt res = 0;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(res, tree, &MSwtTree::ItemAt, aX, aY);
        return res;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1ItemHeight
    (JNIEnv*, jclass, jint aHandle)
    {
        TInt res = 0;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(res, tree, &MSwtTree::ItemHeight);
        return res;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Remove
    (JNIEnv*, jclass, jint aHandle, jint aItemHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::Remove, aItemHandle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1RemoveAll
    (JNIEnv*, jclass, jint aHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::RemoveAll);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Select
    (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aItemHandles, jboolean aState)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aItemHandles, ints);
        if (count == 0)
            return;

        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::Select, ints, count, aState);
        delete[] ints;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1SelectAll
    (JNIEnv*, jclass, jint aHandle, jboolean aState)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(tree, &MSwtTree::SelectAll, aState);
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1Selection
    (JNIEnv* aJniEnv, jclass, jint aHandle)
    {
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        const CArrayFix<TInt>* selectionArray = NULL;
        TRAPD(error, CallMethodL(selectionArray, tree, &MSwtTree::SelectionL));
        ThrowIfError(error, aJniEnv);
        TInt count = selectionArray ? selectionArray->Count() : 0;
        jintArray result = aJniEnv->NewIntArray(count);
        if (!result)
        {
            DeleteInUiHeap(selectionArray);
            return NULL;
        }
        for (TInt i = 0; i < count; i++)
        {
            TInt value = (*selectionArray)[i];
            aJniEnv->SetIntArrayRegion(result, i, 1, &value);
        }
        DeleteInUiHeap(selectionArray);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1SelectionCount
    (JNIEnv*, jclass, jint aHandle)
    {
        TInt res = 0;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(res, tree, &MSwtTree::SelectionCount);
        return res;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1SetImage
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aItemHandle, jint aImageHandle)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TRAPD(error, CallMethodL(tree, &MSwtTree::SetImageL, aItemHandle, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1SetText
    (JNIEnv* aJniEnv, jclass, jint aHandle, jint aItemHandle, jstring aText)
    {
        MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        TRAPD(error,
              HBufC* convertedText = ConvertStringL(aJniEnv, aText);
              CleanupStack::PushL(convertedText);
              TPtr des(convertedText->Des());
              CallMethodL(tree, &MSwtTree::SetTextL, aItemHandle, des);
              CleanupStack::PopAndDestroy(convertedText);
             );
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Tree_1TopItem
    (JNIEnv*, jclass, jint aHandle)
    {
        TInt res = -1;
        const MSwtTree* tree = reinterpret_cast<MSwtTree*>(aHandle);
        CallMethod(res, tree, &MSwtTree::TopItem);
        return res;
    }


    /*
     *  Class DirectoryDialog
     */
    JNIEXPORT void JNICALL DirectoryDialog_OpenL(
        JNIEnv* aJniEnv,
        jobject aPeer,
        jstring aFilterPath,
        jstring aTitle)
    {
        HBufC* filterPath = (aFilterPath != NULL) ?
                            ConvertStringLC(aJniEnv, aFilterPath) : HBufC::NewLC(0);
        HBufC* title = (aTitle != NULL) ?
                       ConvertStringLC(aJniEnv, aTitle) : HBufC::NewLC(0);

        ASSERT(filterPath);
        ASSERT(title);

        CSwtDisplay& display = CSwtDisplay::Current();

        display.CSwtDisplay::RequestRunDialogL(
            reinterpret_cast< TSwtPeer >(aPeer),
            CSwtDialogBroker::EDirectoryDialog, 0, *title, *filterPath);

        CleanupStack::PopAndDestroy(2);   // title, filterPath
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_DirectoryDialog_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jstring aInitPath,
        jstring aTitle)
    {
        TRAPD(
            error,
            DirectoryDialog_OpenL(
                aJniEnv,
                aPeer,
                aInitPath,
                aTitle));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class Browser
     */
    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1ExistsInstanceAlready
    (JNIEnv *, jclass)
    {
        MSwtDisplay& display = CSwtDisplay::Current();
        MSwtUiUtils& uiUtils = display.UiUtils();
        TInt count = 0;
        CallMethod(count, &uiUtils, &MSwtUiUtils::ActiveBrowserCount);
        return ConvertBoolean(count > 0);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1New(JNIEnv* aJniEnv
            , jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        jint handle = NewExpandedWidget(aJniEnv, &MSwtFactory::NewBrowserL, aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, Browser);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1CompositeHandle(JNIEnv*
            , jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        MSwtComposite* composite;
        //composite=browser->Composite();
        CallMethod(composite, browser, &MSwtBrowser::Composite);
        return reinterpret_cast< jint >(composite);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1Back
    (JNIEnv * aJniEnv, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        TBool success = EFalse;
        TRAPD(error, CallMethodL(success, browser, &MSwtBrowser::BackL));
        ThrowIfError(error, aJniEnv);
        if (error)
        {
            success = EFalse;
        }
        return ConvertBoolean(success);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1Forward
    (JNIEnv * aJniEnv, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        TBool success = EFalse;
        TRAPD(error, CallMethodL(success, browser, &MSwtBrowser::ForwardL));
        ThrowIfError(error, aJniEnv) ;
        if (error)
        {
            success = EFalse;
        }
        return ConvertBoolean(success);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1GetUrl
    (JNIEnv * aJniEnv, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        HBufC * url =  NULL;
        //TRAPD( error, ( url =  browser->GetUrlL() ) );
        TRAPD(error, CallMethodL(url, browser, &MSwtBrowser::GetUrlL));
        ThrowIfError(error, aJniEnv);
        jstring jUrl = NewJavaString(aJniEnv, (url!=NULL) ? *url : KNullDesC());
        DeleteInUiHeap(url);
        //delete url;
        return jUrl;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1IsBackEnabled
    (JNIEnv *, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        TBool isBackEnabled;
        //isBackEnabled=browser->IsBackEnabled();
        CallMethod(isBackEnabled, browser, &MSwtBrowser::IsBackEnabled);
        return ConvertBoolean(isBackEnabled);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1IsForwardEnabled
    (JNIEnv *, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        TBool isForwardEnabled;
        //isForwardEnabled=browser->IsForwardEnabled();
        CallMethod(isForwardEnabled, browser, &MSwtBrowser::IsForwardEnabled);
        return ConvertBoolean(isForwardEnabled);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1Refresh
    (JNIEnv * aJniEnv, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);
        TRAPD(error, CallMethodL(browser, &MSwtBrowser::RefreshL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1SetText
    (JNIEnv * aJniEnv, jclass, jint aHandle, jstring aHtml)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);

        HBufC* html = NULL;
        TRAPD(error, (html = ConvertStringL(aJniEnv, aHtml)));
        ThrowIfError(error, aJniEnv);
        if (error)
        {
            return JNI_FALSE;
        }

        TBool success = EFalse;
        TRAPD(error1, CallMethodL(success, browser, &MSwtBrowser::SetTextL, *html));
        ThrowIfError(error1, aJniEnv);
        if (error1)
        {
            success = EFalse;
        }
        delete html;
        return ConvertBoolean(success);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1SetUrl
    (JNIEnv * aJniEnv, jclass, jint aHandle, jstring aUrl)
    {
        MSwtBrowser* browser = reinterpret_cast< MSwtBrowser* >(aHandle);

        HBufC* url = NULL;
        TRAPD(error, (url = ConvertStringL(aJniEnv, aUrl)));
        ThrowIfError(error, aJniEnv);
        if (error)
        {
            return JNI_FALSE;
        }

        TBool success = EFalse;
        TRAPD(error1, CallMethodL(success, browser, &MSwtBrowser::SetUrlL, *url));
        ThrowIfError(error1, aJniEnv);
        delete url;
        if (error1)
        {
            success = EFalse;
        }
        return ConvertBoolean(success);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_expanded_internal_OS_Browser_1Stop
    (JNIEnv * aJniEnv, jclass, jint aHandle)
    {
        MSwtBrowser* browser = reinterpret_cast<MSwtBrowser*>(aHandle);
        TRAPD(error, CallMethodL(browser, &MSwtBrowser::StopL));
        ThrowIfError(error, aJniEnv);
    }

} // extern "C"
