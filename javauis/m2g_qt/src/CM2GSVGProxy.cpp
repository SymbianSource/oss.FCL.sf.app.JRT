/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVGTopt proxy implementation
*
*/

// INCLUDE FILES
#include <AknUtils.h>  // Files add recently
#include <avkon.hrh>   // Files add recently
#include <SvgJavaInterfaceImpl.h>
#include <utf.h> // Character conversion API
#include <featmgr/featmgr.h>
#include "CM2GSVGProxy.h"
#include <eikenv.h>

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// -----------------------------------------------------------------------------
// CM2GSVGProxy::NewL
// -----------------------------------------------------------------------------
CM2GSVGProxy* CM2GSVGProxy::NewL()
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::NewL()");
    CM2GSVGProxy* self = new(ELeave) CM2GSVGProxy;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::~CM2GSVGProxy
// -----------------------------------------------------------------------------
CM2GSVGProxy::~CM2GSVGProxy()
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::~CM2GSVGProxy() - native SVGTopt: %d", iNative);
    if (iNative)
    {
        CloseContainers();
        delete iNative;
        M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::~CM2GSVGProxy() - native deleted");
        iNative = NULL;
    }
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AddCloseL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AddCloseL(const TM2GSvgPathHandle& aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddCloseL() - begin");
    iNative->SvgPathAddClose(aPathHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddCloseL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AddCurveToL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AddCurveToL(
    const TM2GSvgPathHandle& aPathHandle,
    const TM2GPathCurveData& aPathCurveData)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddCurveToL() - begin");
    iNative->SvgPathAddCurveTo(
        aPathHandle,
        aPathCurveData[ 0 ], aPathCurveData[ 1 ],  // X1, Y1
        aPathCurveData[ 2 ], aPathCurveData[ 3 ],  // X2, Y2
        aPathCurveData[ 4 ], aPathCurveData[ 5 ]);  // X3, Y3
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddCurveToL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AddLineToL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AddLineToL(
    const TM2GSvgPathHandle& aPathHandle,
    const TReal32& aX, const TReal32& aY)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddLineToL() - begin");
    iNative->SvgPathAddLineTo(aPathHandle, aX, aY);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddLineToL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AddMoveToL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AddMoveToL(
    const TM2GSvgPathHandle& aPathHandle,
    const TReal32& aX, const TReal32& aY)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddMoveToL() - begin");
    iNative->SvgPathAddMoveTo(aPathHandle, aX, aY);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddMoveToL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AddQuadToL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AddQuadToL(
    const TM2GSvgPathHandle& aPathHandle,
    const TReal32& aX1, const TReal32& aY1,
    const TReal32& aX2, const TReal32& aY2)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddQuadToL() - begin");
    iNative->SvgPathAddQuadTo(aPathHandle, aX1, aY1, aX2, aY2);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AddQuadToL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::AppendChildL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::AppendChildL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgElementHandle& aChildElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AppendChildL() - begin");
    iNative->SvgElementAppendChild(aElementHandle, aChildElementHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::AppendChildL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::BeginElementAtL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::BeginElementAtL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgElementHandle& aElementHandle,
    const TReal32& aOffset)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::BeginElementAtL() offset:%f - begin", aOffset);
    iNative->SvgDocumentBeginElementAt(
        aDocumentHandle, aElementHandle, aOffset);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::BeginElementAtL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CheckRemoveableL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CheckRemoveableL(
    const TM2GSvgElementHandle& aElementHandle,
    TInt& aRemoveable)

{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CheckRemoveableL() - begin");
    aRemoveable = iNative->SvgElementCheckRemoveable(aElementHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CheckRemoveableL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::ClearSvgSurfaceL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::ClearSvgSurfaceL(const TM2GBitmapHandle& /* aSurfaceHandle */)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::ClearSvgSurfaceL() - not supported");
    User::Leave(KErrNotSupported);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CreateDocumentL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CreateDocumentL(const TPtrC16& aData, TM2GSvgDocumentHandle& aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreateDocumentL() - begin");
    aDocumentHandle = M2G_INVALID_HANDLE;

    M2G_DEBUG_1("M2G_DEBUG: M2G_DEBUG: CM2GSVGProxy::CreateDocumentL(): content length=%d - call engine", aData.Length());

    // Create document handle. Note: validity check is done in java side
    aDocumentHandle = iNative->SvgDocumentCreateL(aData);
    // Append document handle to the container
    if (iSvgDocuments.Find(aDocumentHandle) == KErrNotFound)
    {
        TRAPD(err, iSvgDocuments.Append(aDocumentHandle));
        if (err != KM2GOk)
        {
            M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::CreateDocumentL(), cannot add handle=%d, err=%d", aDocumentHandle, err);
        }
    }
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::CreateDocumentL(), handle=%d, total document count=%d - end", aDocumentHandle, iSvgDocuments.Count());
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CreateElementNsL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CreateElementNsL(
    const TM2GSvgAttrType& aType,
    const TM2GSvgDocumentHandle& /* aDocumentHandle */,
    TM2GSvgElementHandle& aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreateElementNsL() - begin");
    // Note: validity check is done in java side
    aElementHandle = iNative->SvgElementCreate(aType);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::CreateElementNsL() handle=%d - end", aElementHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CreatePathL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CreatePathL(TM2GSvgPathHandle& aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreatePathL() - begin");
    aPathHandle = iNative->SvgPathCreate();
    if (aPathHandle == M2G_INVALID_HANDLE)
    {
        User::Leave(KM2GMemoryNotOk);
    }
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreatePathL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CreateSvgEngineL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CreateSvgEngineL(TM2GSvgEngineHandle& aEngineHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreateSvgEngineL() - begin");
    // Create engine handle
    aEngineHandle = iNative->SvgEngineCreate();
    if (aEngineHandle == M2G_INVALID_HANDLE)
    {
        User::Leave(KM2GMemoryNotOk);
    }
    // Append engine handle  to the container
    if (iSvgEngines.Find(aEngineHandle) == KErrNotFound)
    {
        TRAPD(err, iSvgEngines.Append(aEngineHandle));
        if (err != KM2GOk)
        {
            M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::CreateSvgEngineL(), cannot add handle=%d, err=%d", aEngineHandle, err);
        }
    }
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::CreateSvgEngineL(), handle=%d, total engine count=%d - end", aEngineHandle, iSvgEngines.Count());
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CreateSvgSurfaceL
// -----------------------------------------------------------------------------
TM2GBitmapHandle CM2GSVGProxy::CreateSvgSurfaceL(TInt /* aWidth */, TInt /* aHeight */)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CreateSvgSurfaceL() - not supported");

    User::Leave(KErrNotSupported);

    return M2G_INVALID_HANDLE;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::DeleteDocumentL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::DeleteDocumentL(const TM2GSvgDocumentHandle& aDocumentHandle)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::DeleteDocumentL() native:%d, document:%d - begin", iNative, aDocumentHandle);
    TInt findResult = iSvgDocuments.Find(aDocumentHandle);
    if (findResult != KErrNotFound)
    {
        // Try to remove the handle from the container
        TRAPD(err, iSvgDocuments.Remove(findResult));
        if (err != KM2GOk)
        {
            M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::DeleteDocumentL(), cannot add handle=%d, err=%d", aDocumentHandle, err);
        }
        M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DeleteDocumentL()  - container remove result=%d", err);
        // Delete the handle
        iNative->SvgDocumentDestroy(aDocumentHandle);
    }
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DeleteDocumentL() - total document count=%d - end", iSvgDocuments.Count());
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::DeleteSvgEngineL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::DeleteSvgEngineL(const TM2GSvgEngineHandle& aEngineHandle)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DeleteSvgEngineL() engine:%d - begin", aEngineHandle);
    TInt findResult = iSvgEngines.Find(aEngineHandle);
    if (findResult != KErrNotFound)
    {
        // Try to remove the handle from the container
        TRAPD(err, iSvgEngines.Remove(findResult));
        if (err != KM2GOk)
        {
            M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::DeleteSvgEngineL(), cannot add handle=%d, err=%d", aEngineHandle, err);
        }
        M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DeleteSvgEngineL()  - container remove result=%d", err);
        // Delete the handle
        iNative->SvgEngineDestroy(aEngineHandle);
    }
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DeleteSvgEngineL() - total engine count=%d - end", iSvgEngines.Count());
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::DeleteSvgSurfaceL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::DeleteSvgSurfaceL(const TM2GBitmapHandle& /* aSurfaceHandle */)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::DeleteSvgSurfaceL() - not supported");

    User::Leave(KErrNotSupported);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::DestroyPathL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::DestroyPathL(const TM2GSvgPathHandle& aPathHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::DestroyPathL() - begin");
    iNative->SvgPathDestroy(aPathHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::DestroyPathL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::DispatchMouseEventL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::DispatchMouseEventL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt aX, TInt aY, TM2GSvgElementHandle& aElementHandle)
{
    M2G_DEBUG_3("M2G_DEBUG: CM2GSVGProxy::DispatchMouseEventL(): doc handle:%d, x=%d, y=%d- begin", aDocumentHandle, aX, aY);
    aElementHandle = iNative->SvgDocumentDispatchMouseEvent(
                         aDocumentHandle, aX, aY);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::DispatchMouseEventL(): element handle=%d - end", aElementHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::EndElementAtL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::EndElementAtL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgElementHandle& aElementHandle,
    const TReal32& aOffset)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::EndElementAtL() offset=%f - begin", aOffset);
    iNative->SvgDocumentEndElementAt(
        aDocumentHandle, aElementHandle, aOffset);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::EndElementAtL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::FocusOnL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::FocusOnL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgElementHandle& aElementHandle)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::FocusOnL() document=%d, element=%d - begin", aDocumentHandle, aElementHandle);
    iNative->SvgDocumentFocusOn(aDocumentHandle, aElementHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::FocusOnL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::FocusOutL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::FocusOutL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgElementHandle& aElementHandle)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::FocusOutL() document=%d, element=%d - begin", aDocumentHandle, aElementHandle);
    iNative->SvgDocumentFocusOut(aDocumentHandle, aElementHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::FocusOutL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetBBoxL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetBBoxL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TM2GRectData& aRectData, TInt& aResult)
{
    aResult = KM2GOk;
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetBBoxL() - begin");
    iNative->SvgElementGetBBox(
        aElementHandle,
        aAttributeType,
        &aRectData[ 0 ],  // X
        &aRectData[ 1 ],  // Y
        &aRectData[ 2 ],  // Width
        &aRectData[ 3 ]);  // Height
    M2G_DEBUG_4("M2G_DEBUG: CM2GSVGProxy::GetBBoxL(): [x=%f, y=%f, w=%f, h=%f] - end", aRectData[ 0 ], aRectData[ 1 ], aRectData[ 2 ], aRectData[ 3 ]);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetColorTrait
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetColorTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TM2GColorData& aColor,
    TInt& aResult)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetColorTraitL() - begin");
    aResult = iNative->SvgElementGetColorAttribute(
                  aElementHandle,
                  aAttributeType,
                  &aColor[ 0 ],  // Red
                  &aColor[ 1 ],  // Green
                  &aColor[ 2 ]);  // Blue
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetColorTraitL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetElementByIdL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetElementByIdL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TPtrC16& aId,
    TM2GSvgElementHandle& aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetElementByIdL() - begin");
    aElementHandle = iNative->SvgDocumentGetElementById(
                         aDocumentHandle,
                         aId);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetElementByIdL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetElementTypeIdL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetElementTypeIdL(const TM2GSvgElementHandle& aElementHandle, TInt16& aId)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetElementTypeIdL() - begin");
    aId = STATIC_CAST(TInt16, iNative->SvgElementGetType(aElementHandle));
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetElementTypeIdL() type id:%d- end", aId);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetEnumTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetEnumTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeTypeId,
    TInt16& aEnumTrait)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetEnumTraitIdL() - begin");
    aEnumTrait = iNative->SvgElementGetEnumAttribute(
                     aElementHandle,
                     aAttributeTypeId);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetEnumTraitIdL() enum trait:%d - end", aEnumTrait);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetExternalListItemL
// -----------------------------------------------------------------------------
TInt CM2GSVGProxy::GetExternalListItemL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt aIndex,
    TPtrC16& aItem)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetExternalListItemL() index=%d- begin", aIndex);
    TInt result = iNative->SvgDocumentGetExternalListItem(aDocumentHandle, aIndex, aItem);
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::GetExternalListListItemL() result:%d, item sz:%d - end", result, aItem.Length());
    return result;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetExternalListSizeL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetExternalListSizeL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt& aListSz)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetExternalListSizeL() - begin");
    aListSz = iNative->SvgDocumentGetExternalListSize(aDocumentHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetExternalListSizeL(): %d - end", aListSz);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetFirstElementChildL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetFirstElementChildL(
    const TM2GSvgElementHandle& aElementHandle,
    TM2GSvgElementHandle& aChildHandle)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetFirstElementChildL() element=%d - begin", aElementHandle);
    aChildHandle = iNative->SvgElementGetFirstElementChild(aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetFirstElementChildL() child=%d - end", aChildHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetFloatTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetFloatTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TReal32& aFloatTrait)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetFloatTraitL() - begin");
    aFloatTrait =
        iNative->SvgElementGetFloatAttribute(aElementHandle, aAttributeType);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetFloatTraitL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetMatrixTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetMatrixTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TM2GMatrixData& aMatrix, TInt& aResult)
{
    // [ m00 m01 m02 ]
    // [ m10 m11 m12 ]
    // [  0   0   1  ]
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetMatrixTraitL() - begin");
    aResult = iNative->SvgElementGetMatrixAttribute(
                  aElementHandle,
                  aAttributeType,
                  &aMatrix[ 0 ], // m00
                  &aMatrix[ 1 ], // m10
                  &aMatrix[ 2 ], // m01
                  &aMatrix[ 3 ], // m11
                  &aMatrix[ 4 ], // m02
                  &aMatrix[ 5 ]);  // m12
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetMatrixTraitL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetMediaTimeL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetMediaTimeL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TReal32& aSeconds)
{
    aSeconds = iNative->SvgDocumentGetMediaTime(aDocumentHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetNextElementSiblingL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetNextElementSiblingL(
    const TM2GSvgElementHandle& aElementHandle,
    TM2GSvgElementHandle& aSiblingElementHandle)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetNextElementSiblingL(), element=%d - begin", aElementHandle);
    aSiblingElementHandle = iNative->SvgElementGetNextElementSibling(aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetNextElementSiblingL(), sibling=%d - end", aSiblingElementHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetNumberOfSegmentsL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetNumberOfSegmentsL(
    const TM2GSvgPathHandle& aPathHandle,
    TInt& aNumberOfSegments)
{
    aNumberOfSegments = iNative->SvgPathGetSegmentCount(aPathHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetParentL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetParentL(
    const TM2GSvgElementHandle& aElementHandle,
    TM2GSvgElementHandle& aParentElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetParentL() - begin");
    aParentElementHandle = iNative->SvgElementGetParent(aElementHandle);
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::GetParentL() element=%d, parent=%d, - end", aElementHandle, aParentElementHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetPathTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetPathTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TInt& aPathTrait)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetPathTraitL() - begin");
    aPathTrait = iNative->SvgElementGetPathAttribute(aElementHandle, aAttributeType);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetPathTraitL() %d - end", aPathTrait);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetRectTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetRectTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TM2GRectData& aRectData, TInt& aResult)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetRectTraitL() - begin");
    aResult = iNative->SvgElementGetRectAttribute(
                  aElementHandle,
                  aAttributeType,
                  &aRectData[ 0 ], // X
                  &aRectData[ 1 ], // Y
                  &aRectData[ 2 ], // Width
                  &aRectData[ 3 ]);  // Height
    M2G_DEBUG_5("M2G_DEBUG: CM2GSVGProxy::GetRectTraitL(): result=%d [x=%f, y=%f, w=%f, h=%f] - end", aResult, aRectData[ 0 ], aRectData[ 1 ], aRectData[ 2 ], aRectData[ 3 ]);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetRootElementL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetRootElementL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TM2GSvgElementHandle& aRootElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetRootElementL() - begin");
    aRootElementHandle = iNative->SvgDocumentGetRootElement(aDocumentHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetRootElementL() handle:%d - end", aRootElementHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetScreenBBoxL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetScreenBBoxL(
    const TM2GSvgElementHandle& aElementHandle,
    TM2GScreenBBoxData& aScreenBBoxData)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetScreenBBoxL() - begin");
    iNative->SvgElementGetScreenBBox(
        aElementHandle,
        &aScreenBBoxData[ 0 ], // X
        &aScreenBBoxData[ 1 ], // Y
        &aScreenBBoxData[ 2 ], // Width
        &aScreenBBoxData[ 3 ]);  // Height
    M2G_DEBUG_4("M2G_DEBUG: CM2GSVGProxy::GetScreenBBoxL(): [x=%f, y=%f, w=%f, h=%f] - end", aScreenBBoxData[ 0 ], aScreenBBoxData[ 1 ], aScreenBBoxData[ 2 ], aScreenBBoxData[ 3 ]);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetSegmentParameterL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetSegmentParameterL(
    const TM2GSvgPathHandle& aPathHandle,
    TInt aSegmentIndex, TInt aParamIndex,
    TReal32& aSegmentParam)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::SvgPathGetSegmentParameter(): index=%d, paramIndex=%d - begin", aSegmentIndex, aParamIndex);
    aSegmentParam = iNative->SvgPathGetSegmentParameter(
                        aPathHandle,
                        aSegmentIndex,
                        aParamIndex);
    M2G_DEBUG_3("M2G_DEBUG: CM2GSVGProxy::SvgPathGetSegmentParameter(): index=%d, paramIndex=%d, param=%d - end", aSegmentIndex, aParamIndex, aSegmentParam);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetSegmentTypeL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetSegmentTypeL(
    const TM2GSvgPathHandle& aPathHandle,
    TInt aSegmentIndex,
    TInt16& aSegmentType)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetSegmentTypeL(): index=%d - begin", aSegmentIndex);
    aSegmentType = iNative->SvgPathGetSegmentType(
                       aPathHandle,
                       aSegmentIndex);
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::GetSegmentTypeL(): index=%d, type=%d - end", aSegmentIndex, aSegmentType);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetStringTraitL
// -----------------------------------------------------------------------------
TInt CM2GSVGProxy::GetStringTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TPtrC16& aStr)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::GetStringTraitL(): attribute type:%d, handle:%u - begin", aAttributeType, aElementHandle);

    TInt result = iNative->SvgElementGetStringAttribute(aElementHandle,  aAttributeType, aStr);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetStringTraitL(): attribute sz:%d", aStr.Length());
    return result;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetSvgSurfaceHeightL
// -----------------------------------------------------------------------------
TInt CM2GSVGProxy::GetSvgSurfaceHeightL(const TM2GBitmapHandle& /* aSurfaceHandle */)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetSvgSurfaceHeightL - not supported");

    User::Leave(KErrNotSupported);

    return 0;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetSvgSurfaceWidthL
// -----------------------------------------------------------------------------
TInt CM2GSVGProxy::GetSvgSurfaceWidthL(const TM2GBitmapHandle& /* aSurfaceHandle */)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::GetSvgSurfaceWidthL - not supported");

    User::Leave(KErrNotSupported);

    return 0;
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetUsedFromElementL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetUsedFromElementL(
    const TM2GSvgElementHandle& aElementHandle,
    TM2GSvgElementHandle& aHandle)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetUsedFromElementL() used from element=%d - begin", aElementHandle);
    aHandle = iNative->SvgElementGetUsedFromElement(aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::GetUsedFromElementL() found element=%d - end", aHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetViewportHeightL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetViewportHeightL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt& aHeight)
{
    aHeight = iNative->SvgDocumentGetViewportHeight(aDocumentHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::GetViewportWidthL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::GetViewportWidthL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt& aWidth)
{
    aWidth = iNative->SvgDocumentGetViewportWidth(aDocumentHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::InitViewportL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::InitViewportL(const TM2GSvgDocumentHandle& aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::InitViewportL()");
    return iNative->SvgDocumentViewportInit(aDocumentHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::InsertBeforeL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::InsertBeforeL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgElementHandle& aNewChildElementHandle,
    const TM2GSvgElementHandle& aReferenceElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::InsertBeforeL() - begin");
    iNative->SvgElementInsertBefore(
        aElementHandle,
        aNewChildElementHandle,
        aReferenceElementHandle);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::InsertBeforeL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::IsActiveL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::IsActiveL(const TM2GSvgElementHandle& aElementHandle, TInt& aActive)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::IsActiveL() - begin");
    aActive = iNative->SvgElementIsActive(aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::IsActiveL() active:%d - end", aActive);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::IsElementInDomL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::IsElementInDomL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgElementHandle& aElementHandle,
    TInt& aIsElementInDom)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::IsElementInDomL() doc handle=%d, elem handle=%d - begin", aDocumentHandle, aElementHandle);
    aIsElementInDom = iNative->SvgElementElementInDOM(
                          aDocumentHandle, aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::IsElementInDomL() result=%d - end", aIsElementInDom);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::IsUsedL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::IsUsedL(const TM2GSvgElementHandle& aElementHandle, TInt& aResult)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::IsUsedL() - begin");
    aResult = iNative->SvgElementIsUsed(aElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::IsUsedL(), result=%d - end", aResult);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::RemoveChildL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::RemoveChildL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgElementHandle& aChildElementHandle,
    TM2GSvgElementHandle& aHandle)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::RemoveChildL() parent=%d, child=%d - begin", aElementHandle, aChildElementHandle);
    aHandle = iNative->SvgElementRemoveChild(aElementHandle, aChildElementHandle);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::RemoveChildL() result handle=%d - end", aHandle);
}


// -----------------------------------------------------------------------------
// CM2GSVGProxy::RenderDocumentL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::RenderDocumentL(
    const TM2GSvgEngineHandle& aEngineHandle,
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GBitmapHandle& aSurfaceHandle,
    TM2GBitmapHandle aSurfaceMaskHandle,
    TReal32 aCurrentTime)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() native:%d, time=%f - begin", iNative, aCurrentTime);
    M2G_DEBUG_3("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() engine:%d, doc:%d, surface:%d", aEngineHandle, aDocumentHandle, aSurfaceHandle);
    iNative->SvgEngineRenderDocument(
        aEngineHandle, aDocumentHandle, aSurfaceHandle, aSurfaceMaskHandle, aCurrentTime);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() - end");
}

//TODO following changes done for Qimage
//Check for TUint or TInt for subsequent API Call
void CM2GSVGProxy::RenderDocumentL(
    const TM2GSvgEngineHandle& aEngineHandle,
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TM2GSvgBitmapHandle& aSurfaceHandle,
    TM2GSvgBitmapHandle aSurfaceMaskHandle,
    TReal32 aCurrentTime)
    {
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() Qimage variant native:%d, time=%f - begin", iNative, aCurrentTime);
    M2G_DEBUG_3("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() engine:%d, doc:%d, SvgBitmapHandle:%d", aEngineHandle, aDocumentHandle, aSurfaceHandle);
    iNative->SvgEngineRenderDocument(
        aEngineHandle, aDocumentHandle, aSurfaceHandle, aSurfaceMaskHandle, aCurrentTime);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::RenderDocumentL() - end");
    
    }

// -----------------------------------------------------------------------------
// CM2GSVGProxy::RenderQualityL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::RenderQualityL(
    const TM2GSvgDocumentHandle& aEngineHandle,
    TInt aQuality)
{
    iNative->SvgEngineSetRenderQuality(aEngineHandle, aQuality);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::RequestCompletedL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::RequestCompletedL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TPtrC16& aURI,
    const TPtrC8& aResourceData,
    TInt& aCompleted)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GSVGProxy::RequestCompleted() uri sz=%d, resource sz= %d ", aURI.Length(), aResourceData.Length());
    aCompleted = iNative->SvgDocumentRequestCompleted(
                     aDocumentHandle,
                     aURI,
                     aResourceData);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::RequestCompletedL() completed=%d - end", aCompleted);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetColorTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetColorTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    const TM2GColorData& aColor)
{
    iNative->SvgElementSetColorAttribute(
        aElementHandle,
        aAttributeType,
        aColor[ 0 ],  // red
        aColor[ 1 ],  // green
        aColor[ 2 ]);  // blue
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetEnumTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetEnumTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    TInt16 aValue)
{
    iNative->SvgElementSetEnumAttribute(
        aElementHandle,
        aAttributeType,
        aValue);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetFloatTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetFloatTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    const TReal32& aValue)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::SetFloatTraitL() - %f", aValue);
    iNative->SvgElementSetFloatAttribute(
        aElementHandle,
        aAttributeType,
        aValue);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetMatrixTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetMatrixTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    const TM2GMatrixData& aMatrix)
{
    // [ m00 m01 m02 ]
    // [ m10 m11 m12 ]
    // [  0   0   1  ]
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetMatrixTraitL() - begin");
    iNative->SvgElementSetMatrixAttribute(
        aElementHandle,
        aAttributeType,
        aMatrix[ 0 ],  // m00 - A
        aMatrix[ 1 ],  // m10 - B
        aMatrix[ 2 ],  // m01 - C
        aMatrix[ 3 ],  // m11 - D
        aMatrix[ 4 ],  // m02 - E
        aMatrix[ 5 ]);  // m12 - F
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetMatrixTraitL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetMediaTimeL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetMediaTimeL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    const TReal32& aSeconds)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::SetMediaTimeL() %f - begin", aSeconds);
    iNative->SvgDocumentSetMediaTime(aDocumentHandle, aSeconds);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetMediaTimeL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetPathTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetPathTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    const TM2GSvgPathHandle& aPathHandle)
{
    iNative->SvgElementSetPathAttribute(
        aElementHandle,
        aAttributeType,
        aPathHandle);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetRectTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetRectTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeType,
    const TM2GRectData& aRect)
{
    iNative->SvgElementSetRectAttribute(
        aElementHandle,
        aAttributeType,
        aRect[ 0 ],  // aX
        aRect[ 1 ],  // aY
        aRect[ 2 ],  // aWidth
        aRect[ 3 ]);  // aHeight
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetRenderingQualityL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetRenderingQualityL(
    const TM2GSvgDocumentHandle& aEngineHandle,
    TInt aMode)
{
    iNative->SvgEngineSetRenderQuality(aEngineHandle, aMode);
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetStringTraitL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetStringTraitL(
    const TM2GSvgElementHandle& aElementHandle,
    const TM2GSvgAttrType& aAttributeTypeId,
    const TPtrC16& aStr)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetStringTraitL() - begin");
    iNative->SvgElementSetStringAttribute(
        aElementHandle,
        aAttributeTypeId,
        aStr);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::SetStringTraitL(): str sz:%d - end", aStr.Length());
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetViewportWidthL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetViewportWidthL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt aWidth)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::SetViewportWidthL(): %d - begin", aWidth);
    iNative->SvgDocumentSetViewportWidth(aDocumentHandle, aWidth);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetViewportWidthL() - end");
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::SetViewportHeightL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::SetViewportHeightL(
    const TM2GSvgDocumentHandle& aDocumentHandle,
    TInt aHeight)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::SetViewportHeightL(): %d - begin", aHeight);
    iNative->SvgDocumentSetViewportHeight(aDocumentHandle, aHeight);
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::SetViewportHeightL() - end");
}

// == PROTECTED METHODS ==

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CloseContainers
// -----------------------------------------------------------------------------
void CM2GSVGProxy::CloseContainers()
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CloseContainers() - begin");
    if (iNative)
    {
        // Clear document container
        TInt count = iSvgDocuments.Count();
        M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::CloseContainers() - document count=%d", count);
        for (TInt index = 0; index < count; index++)
        {
            iNative->SvgDocumentDestroy(iSvgDocuments[index]);
        }
        iSvgDocuments.Reset();
        iSvgDocuments.Close();
        // Clear engine container
        count = iSvgEngines.Count();
        M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::CloseContainers() - engine count=%d", count);
        for (TInt index = 0; index < count; index++)
        {
            iNative->SvgEngineDestroy(iSvgEngines[index]);
        }
        iSvgEngines.Reset();
        iSvgEngines.Close();
    }
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::CloseContainers() - end");
}

// == PRIVATE METHODS ==

// -----------------------------------------------------------------------------
// CM2GSVGProxy::CM2GSVGProxy
// -----------------------------------------------------------------------------
CM2GSVGProxy::CM2GSVGProxy()
        : CBase(),
        iNative(NULL),
        iSvgEngines(),
        iSvgDocuments()
{
    iSvgEngines.Compress();
    iSvgDocuments.Compress();
}

// -----------------------------------------------------------------------------
// CM2GSVGProxy::ConstructL
// -----------------------------------------------------------------------------
void CM2GSVGProxy::ConstructL()
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GSVGProxy::ConstructL() - begin");

    //Get the font spec with variant default font
    const TInt KApacFontId   = EApacPlain16;
    const TInt KLatintFontId = ELatinBold12;
    TInt fontId = KLatintFontId;
    
  /*  switch (AknLayoutUtils::Variant())
    {
    case EApacVariant:
    {
        fontId = KApacFontId;
    }
    break;

    case EEuropeanVariant:
    default:
        break;
    }
    FeatureManager::InitializeLibL();

    if (FeatureManager::FeatureSupported(KFeatureIdAvkonApac))
    {
    fontId = KApacFontId;
    }

    FeatureManager::UnInitializeLib();
    
    const CFont* font = AknLayoutUtils::FontFromId(fontId);
    TFontSpec spec = font->FontSpecInTwips();*/
    // TODO have to check for substitute of above. 
    const CFont* font = CEikonEnv::Static()->NormalFont();
    TFontSpec spec = font->FontSpecInTwips();
    iNative = CSvgJavaInterfaceImpl::NewL(spec);
    M2G_DEBUG_1("M2G_DEBUG: CM2GSVGProxy::ConstructL() - SVGTopt created: %d", iNative);
}

M2G_NS_END
