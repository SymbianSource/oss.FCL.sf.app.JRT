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
* Description:  SVGTopt proxy
*
*/

#ifndef CM2GSVGPROXY_H
#define CM2GSVGPROXY_H

//  INCLUDES
#include "MM2GSVGProxy.h"

M2G_NS_START

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSvgJavaInterfaceImpl;

// CLASS DECLARATION
/**
 *
 */
class CM2GSVGProxy : public CBase, public MM2GSVGProxy
{
public: // METHODS
    /**
     * Two-phased constructor.
     * @return Reference counted proxy wrapper
     */
    static CM2GSVGProxy* NewL();

    /**
     * Destructor.
     */
    virtual ~CM2GSVGProxy();

    /**
     * @see MM2GSVGProxy::AddCloseL
     */
    virtual void AddCloseL(const TM2GSvgPathHandle& aPathHandle);

    /**
     * @see MM2GSVGProxy::AddCurveToL
     */
    virtual void AddCurveToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TM2GPathCurveData& aPathCurveData);

    /**
     * @see MM2GSVGProxy::AddLineToL
     */
    virtual void AddLineToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX, const TReal32& aY);

    /**
     * @see MM2GSVGProxy::AddMoveToL
     */
    virtual void AddMoveToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX, const TReal32& aY);

    /**
     * @see MM2GSVGProxy::AddQuadToL
     */
    virtual void AddQuadToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX1, const TReal32& aY1,
        const TReal32& aX2, const TReal32& aY2);

    /**
     * @see MM2GSVGProxy::AppendChildL
     */
    virtual void AppendChildL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aChildElementHandle);

    /**
     * @see MM2GSVGProxy::BeginElementAtL
     */
    virtual void BeginElementAtL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        const TReal32& aOffset);

    /**
     * @see MM2GSVGProxy::CheckRemoveableL
     */
    virtual void CheckRemoveableL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aRemoveable);

    /**
     * @see MM2GSVGProxy::ClearSvgSurfaceL
     */
    virtual void ClearSvgSurfaceL(const TM2GBitmapHandle& aSurfaceHandle);

    /**
     * @see MM2GSVGProxy::CreateDocumentL
     */
    virtual void CreateDocumentL(
        const TPtrC16& aData,
        TM2GSvgDocumentHandle& aDocumentHandle);

    /**
     * @see MM2GSVGProxy::CreateElementNsL
     */
    virtual void CreateElementNsL(
        const TM2GSvgAttrType& aType,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TM2GSvgElementHandle& aElementHandle);

    /**
     * @see MM2GSVGProxy::CreatePathL
     */
    virtual void CreatePathL(TM2GSvgPathHandle& aPathHandle);


    /**
     * @see MM2GSVGProxy::CreateSvgEngineL
     */
    virtual void CreateSvgEngineL(TM2GSvgEngineHandle& aEngineHandle);


    /**
     * @see MM2GSVGProxy::TSvgSurfaceHandleL
     */
    virtual TM2GBitmapHandle CreateSvgSurfaceL(TInt aWidth, TInt aHeight);

    /**
     * @see MM2GSVGProxy::DeleteDocumentL
     */
    virtual void DeleteDocumentL(const TM2GSvgDocumentHandle& aDocumentHandle);

    /**
     * @see MM2GSVGProxy::DeleteSvgEngineL
     */
    virtual void DeleteSvgEngineL(const TM2GSvgEngineHandle& aEngineHandle);

    /**
     * @see MM2GSVGProxy::DeleteSvgSurfaceL
     */
    virtual void DeleteSvgSurfaceL(const TM2GBitmapHandle& aSurfaceHandle);

    /**
     * @see MM2GSVGProxy::DestroyPathL
     */
    virtual void DestroyPathL(const TM2GSvgPathHandle& aPathHandle);

    /**
     * @see MM2GSVGProxy::DispatchMouseEventL
     */
    virtual void DispatchMouseEventL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aX, TInt aY, TM2GSvgElementHandle& aElementHandle);

    /**
     * @see MM2GSVGProxy::EndElementAtL
     */
    virtual void EndElementAtL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        const TReal32& aOffset);

    /**
     * @see MM2GSVGProxy::FocusOnL
     */
    virtual void FocusOnL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle);

    /**
     * @see MM2GSVGProxy::FocusOutL
     */
    virtual void FocusOutL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle);

    /**
     * @see MM2GSVGProxy::GetBBoxL
     */
    virtual void GetBBoxL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GRectData& aRectData, TInt& aResult);

    /**
     * @see MM2GSVGProxy::GetColorTraitL
     */
    virtual void GetColorTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GColorData& aColor,
        TInt& aResult);

    /**
     * @see MM2GSVGProxy::GetElementByIdL
     */
    virtual void GetElementByIdL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TPtrC16& aId,
        TM2GSvgElementHandle& aElementHandle);

    /**
     * @see MM2GSVGProxy::GetElementTypeIdL
     */
    virtual void GetElementTypeIdL(const TM2GSvgElementHandle& aElementHandle, TInt16& aId);

    /**
     * @see MM2GSVGProxy::GetEnumAttributeL
     */
    virtual void GetEnumTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt16& aEnumTrait);

    /**
     * @see MM2GSVGProxy::GetExternalItemL
     */
    virtual TInt GetExternalListItemL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aIndex,
        TPtrC16& aItem);

    /**
     * @see MM2GSVGProxy::GetExternalListSizeL
     */
    virtual void GetExternalListSizeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aListSz);

    /**
     * @see MM2GSVGProxy::GetFirstElementChildL
     */
    virtual void GetFirstElementChildL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aChildHandle);

    /**
     * @see MM2GSVGProxy::GetFloatTraitL
     */
    virtual void GetFloatTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TReal32& aFloatTrait);

    /**
     * @see MM2GSVGProxy::GetMatrixTraitL
     */
    virtual void GetMatrixTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GMatrixData& aMatrix, TInt& aResult);

    /**
     * @see MM2GSVGProxy::GetMediaTimeL
     */
    virtual void GetMediaTimeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TReal32& aSeconds);

    /**
     * @see MM2GSVGProxy::GetNextElementSiblingL
     */
    virtual void GetNextElementSiblingL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aSiblingElementHandle);

    /**
     * @see MM2GSVGProxy::GetNumberOfSegmentsL
     */
    virtual void GetNumberOfSegmentsL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt& aNumberOfSegments);

    /**
     * @see MM2GSVGProxy::GetParentL
     */
    virtual void GetParentL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aParentElementHandle);

    /**
     * @see MM2GSVGProxy::GetPathTraitL
     */
    virtual void GetPathTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt& aPathTrait);

    /**
     * @see MM2GSVGProxy::GetRectTraitL
     */
    virtual void GetRectTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GRectData& aRectData, TInt& aResult);

    /**
     * @see MM2GSVGProxy::GetRootElementL
     */
    virtual void GetRootElementL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TM2GSvgElementHandle& aRootElementHandle);

    /**
     * @see MM2GSVGProxy::GetScreenBBoxL
     */
    virtual void GetScreenBBoxL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GScreenBBoxData& aScreenBBoxData);

    /**
     * @see MM2GSVGProxy::GetSegmentParameterL
     */
    virtual void GetSegmentParameterL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt aSegmentIndex, TInt aParamIndex,
        TReal32& aSegmentParam);

    /**
     * @see MM2GSVGProxy::GetSegmentTypeL
     */
    virtual void GetSegmentTypeL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt aSegmentIndex,
        TInt16& aSegmentType);

    /**
     * @see MM2GSVGProxy::GetStringTraitL
     */
    virtual TInt GetStringTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TPtrC16& aStr);


    /**
     * @see MM2GSVGProxy::GetSvgSurfaceHeightL
     */
    virtual TInt GetSvgSurfaceHeightL(const TM2GBitmapHandle& aSurfaceHandle);

    /**
     * @see MM2GSVGProxy::GetSvgSurfaceWidthL
     */
    virtual TInt GetSvgSurfaceWidthL(const TM2GBitmapHandle& aSurfaceHandle);

    /**
     * @see MM2GSVGProxy::GetUsedFromElement
     */
    virtual void GetUsedFromElementL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aHandle);

    /**
     * @see MM2GSVGProxy::GetViewportHeightL
     */
    virtual void GetViewportHeightL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aHeight);

    /**
     * @see MM2GSVGProxy::GetViewportWidthL
     */
    virtual void GetViewportWidthL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aWidth);

    /**
     * @see MM2GSVGProxy::InitViewportL
     */
    virtual void InitViewportL(
        const TM2GSvgDocumentHandle& aDocumentHandle);

    /**
     * @see MM2GSVGProxy::InsertBeforeL
     */
    virtual void InsertBeforeL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aNewChildElementHandle,
        const TM2GSvgElementHandle& aReferenceElementHandle);

    /**
     * @see MM2GSVGProxy::IsActiveL
     */
    virtual void IsActiveL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aActive);

    /**
     * @see MM2GSVGProxy::IsElementInDomL
     */
    virtual void IsElementInDomL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aIsElementInDom);

    /**
     * @see MM2GSVGProxy::IsUsedL
     */
    virtual void IsUsedL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aResult);

    /**
     * @see MM2GSVGProxy::RemoveChildL
     */
    virtual void RemoveChildL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aChildElementHandle,
        TM2GSvgElementHandle& aHandle);

    /**
     * @see MM2GSVGProxy::RenderDocumentL
     */
    virtual void RenderDocumentL(
        const TM2GSvgEngineHandle& aEngineHandle,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GBitmapHandle& aSurfaceHandle,
        TM2GBitmapHandle aSurfaceMaskHandle,
        TReal32 aCurrentTime);

//    pankaj Qimage related changes TODO following Changes done 
    virtual void RenderDocumentL(
        const TM2GSvgEngineHandle& aEngineHandle,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgBitmapHandle& aSurfaceHandle,
        TM2GSvgBitmapHandle aSurfaceMaskHandle,
        TReal32 aCurrentTime);
     /**
     * @see MM2GSVGProxy::RenderQualityL
     */
    virtual void RenderQualityL(
        const TM2GSvgEngineHandle& aEngineHandle,
        TInt aQuality);
    /**
     * @see MM2GSVGProxy::RequestCompletedL
     */
    virtual void RequestCompletedL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TPtrC16& aURI,
        const TPtrC8& aResourceData,
        TInt& aCompleted);

    /**
     * @see MM2GSVGProxy::SetColorTraitL
     */
    virtual void SetColorTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GColorData& aColor);

    /**
     * @see MM2GSVGProxy::SetEnumTraitL
     */
    virtual void SetEnumTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt16 aValue);

    /**
     * @see MM2GSVGProxy::SetFloatTraitL
     */
    virtual void SetFloatTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TReal32& aValue);

    /**
     * @see MM2GSVGProxy::SetMatrixTraitL
     */
    virtual void SetMatrixTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GMatrixData& aMatrix);

    /**
     * @see MM2GSVGProxy::SetMediaTimeL
     */
    virtual void SetMediaTimeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TReal32& aSeconds);

    /**
     * @see MM2GSVGProxy::SetPathTraitL
     */
    virtual void SetPathTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GSvgPathHandle& aPathHandle);

    /**
     * @see MM2GSVGProxy::SetRectTraitL
     */
    virtual void SetRectTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GRectData& aRect);

    /**
     * @see MM2GSVGProxy::SetRenderingQualityL
     */
    virtual void SetRenderingQualityL(
        const TM2GSvgEngineHandle& aEngineHandle,
        TInt aMode);

    /**
     * @see MM2GSVGProxy::SetStringTraitL
     */
    virtual void SetStringTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeTypeId,
        const TPtrC16& aStr);

    /**
     * @see MM2GSVGProxy::SetViewportHeightL
     */
    virtual void SetViewportHeightL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aHeight);


    /**
     * @see MM2GSVGProxy::SetViewportWidthL
     */
    virtual void SetViewportWidthL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aWidth);

protected: // METHODS
    /**
     * Close engine & document containers
     */
    virtual void CloseContainers();

private: // METHODS
    /**
     * C++ default constructor.
     */
    CM2GSVGProxy();

    /**
     * Initialization.
     */
    void ConstructL();

    /**
     * C++ copy constructor
     */
    CM2GSVGProxy(const CM2GSVGProxy&);

    /**
     * C++ assignment operator
     */
    CM2GSVGProxy& operator=(const CM2GSVGProxy&);

private: // VARIABLES
    CSvgJavaInterfaceImpl* iNative;
    RArray<TM2GSvgEngineHandle> iSvgEngines;
    RArray<TM2GSvgDocumentHandle> iSvgDocuments;
};

M2G_NS_END

#endif // CM2GSVGPROXY_H


