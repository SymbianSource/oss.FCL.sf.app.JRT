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
* Description:  SVGTopt proxy interface
*
*/

#ifndef MM2GSVGPROXY_H
#define MM2GSVGPROXY_H

//  INCLUDES
#include "M2GUtils.h"

M2G_NS_START

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Interface class
 */
class MM2GSVGProxy
{
public: // METHODS

    /**
     * Dtor
     */
    virtual ~MM2GSVGProxy() {};

    /**
     * Add close to the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @throws exception if not ok
     */
    virtual void AddCloseL(const TM2GSvgPathHandle& aPathHandle) = 0;

    /**
     * Add curve to the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aPathCurveData Curve data
     * @throws exception if not ok
     */
    virtual void AddCurveToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TM2GPathCurveData& aPathCurveData) = 0;

    /**
     * Add line to the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aX -
     * @param aY -
     * @throws exception if not ok
     */
    virtual void AddLineToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX, const TReal32& aY) = 0;
    /**
     * Add move to the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aX -
     * @param aY -
     * @throws exception if not ok
     */
    virtual void AddMoveToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX, const TReal32& aY) = 0;

    /**
     * Add quad to the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aX1 -
     * @param aY1 -
     * @param aX2 -
     * @param aY2 -
     * @throws exception if not ok
     */
    virtual void AddQuadToL(
        const TM2GSvgPathHandle& aPathHandle,
        const TReal32& aX1, const TReal32& aY1,
        const TReal32& aX2, const TReal32& aY2) = 0;

    /**
     * Append child
     * @since Series S60 3.1
     * @param aElementHandle
     * @param aChildElementHandle
     * @throws exception if not ok
     */
    virtual void AppendChildL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aChildElementHandle) = 0;

    /**
     * Set the begin animation time for the given element.
     * @since Series S60 3.1
     * @param aDocumentHandle
     * @param aElementHandle
     * @param aOffset
     * @throws exception if not ok
     */
    virtual void BeginElementAtL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        const TReal32& aOffset) = 0;

    /**
     * Check if element is removable
     * @since Series S60 3.1
     * @param aSurfaceHandle
     * @param aRemoveable Result
     * @throws exception if not ok
     */
    virtual void CheckRemoveableL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aRemoveable) = 0;


    /**
     * Clear svg surface
     * @since Series S60 3.1
     * @param aSurfaceHandle Surface handle
     * @throws exception if not ok
     */
    virtual void ClearSvgSurfaceL(const TM2GSvgDocumentHandle& aSurfaceHandle) = 0;

    /**
     * Create document
     * @since Series S60 3.1
     * @param aData Document content
     * @param aDocumentHandle Document handle to be returned
     * @throws exception if not ok
     */
    virtual void CreateDocumentL(
        const TPtrC16& aData,
        TM2GSvgDocumentHandle& aDocumentHandle) = 0;

    /**
     * Create svg engine
     * @since Series S60 3.1
     * @param aEngineHandle Svg engine handle
     * @throws exception if not ok
     */
    virtual void CreateSvgEngineL(TM2GSvgEngineHandle& aEngineHandle) = 0;

    /**
     * Create path
     * @since Series S60 3.1
     * @param aPathHandle Path handle to be returned
     * @throws exception if not ok
     */
    virtual void CreatePathL(TM2GSvgPathHandle& aPathHandle) = 0;

    /**
     * Create svg surface
     * @since Series S60 3.1
     * @param aWidth Surface width
     * @param aHeight Surface height
     * @return Svg surface
     * @throws exception if not ok
     */
    virtual TM2GBitmapHandle CreateSvgSurfaceL(TInt aWidth, TInt aHeight) = 0;

    /**
     * Create new element
     * @since Series S60 3.1
     * @param aType Element type id
     * @param aParentHandle Parent of the new element.
     * Note: Can be '0' if element is not added to DOM.
     * @param aDocumentHandle Document handle.
     * @param aRootHandle Root element handle.
     * @param aEngineHandle Element handle to be returned
     * @throws exception if not ok
     */
    virtual void CreateElementNsL(
        const TM2GSvgAttrType& aType,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TM2GSvgElementHandle& aEngineHandle) = 0;

    /**
     * Delete document
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle
     * @throws exception if not ok
     */
    virtual void DeleteDocumentL(const TM2GSvgDocumentHandle& aDocumentHandle) = 0;

    /**
     * Delete svg engine
     * @since Series S60 3.1
     * @param aEngineHandle Engine handle
     * @throws exception if not ok
     */
    virtual void DeleteSvgEngineL(const TM2GSvgEngineHandle& aEngineHandle) = 0;

    /**
     * Deletes svg surface
     * @since Series S60 3.1
     * @param aSurfaceHandle Surface handle
     * @throws exception if not ok
     */
    virtual void DeleteSvgSurfaceL(const TM2GSvgDocumentHandle& aSurfaceHandle) = 0;

    /**
     * Delete path
     * @since Series S60 3.1
     * @param aPathHandle Path handle
     * @throws exception if not ok
     */
    virtual void DestroyPathL(const TM2GSvgPathHandle& aPathHandle) = 0;

    /**
     * Dispatches a mouse event
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aX X coordinate.
     * @param aY Y coordinate
     * @param aElementHandle Target element handle
     * @throws exception if not ok
     */
    virtual void DispatchMouseEventL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aX, TInt aY, TM2GSvgElementHandle& aElementHandle) = 0;

    /**
     * Set the end animation time for the given element.
     * @since Series S60 3.1
     * @param aDocumentHandle
     * @param aElementHandle
     * @param aOffset
     * @throws exception if not ok
     */
    virtual void EndElementAtL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        const TReal32& aOffset) = 0;


    /**
     * Set focus on.
     * @since Series S60 3.1
     * @param aDocumentHandle
     * @param aElementHandle
     * @throws exception if not ok
     */
    virtual void FocusOnL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle) = 0;

    /**
     * Set focus out.
     * @since Series S60 3.1
     * @param aDocumentHandle
     * @param aElementHandle
     * @throws exception if not ok
     */
    virtual void FocusOutL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle) = 0;

    /**
     * Get a bounding box
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aRectData Rect components are saved to this parameter
     * @param aResult Result
     * @throws exception if not ok
     */
    virtual void GetBBoxL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GRectData& aRectData, TInt& aResult) = 0;

    /**
     * Get colors
     * @since Series S60 3.1
     * @param aElementHandle Element handle
     * @param aAttributeType Attribute type
     * @param aColor Result
     * @param aResult Extra result info
     * @throws exception if not ok
     */
    virtual void GetColorTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GColorData& aColor,
        TInt& aResult) = 0;

    /**
     * Get element by id
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aId Element id
     * @param aElementHandle Element handle to be returned
     * @throws exception if not ok
     */
    virtual void GetElementByIdL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TPtrC16& aId,
        TM2GSvgElementHandle& aElementHandle) = 0;

    /**
     * Get element by id
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aId Result element type id.
     * @throws exception if not ok
     */
    virtual void GetElementTypeIdL(const TM2GSvgElementHandle& aElementHandle, TInt16& aId) = 0;

    /**
     * Get an enumeration value of the specified attribute.
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aEnumTraiT Enum attribute.
     * @throws exception if not ok
     */
    virtual void GetEnumTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt16& aEnumTrait) = 0;

    /**
     * Get external list item
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aIndex Index.
     * @param aItem Returns external item according to a given index
     * @return KErrNone if no error
     * @throws Exception if not ok
     */
    virtual TInt GetExternalListItemL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aIndex,
        TPtrC16& aItem) = 0;

    /**
     * Get external list size
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aListSz Size.
     * @throws exception if not ok
     */
    virtual void GetExternalListSizeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aListSz) = 0;

    /**
     * Get a first chlid element
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aChildHandle Returns child element handle.
     * @throws exception if not ok
     */
    virtual void GetFirstElementChildL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aChildHandle) = 0;

    /**
     * Get a float trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param Float trait
     * @throws exception if not ok
     */
    virtual void GetFloatTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TReal32& aFloatTrait) = 0;

    /**
     * Get a matrix trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aMatrix Matrix trait
     * @param aResult
     * @throws exception if not ok
     */
    virtual void GetMatrixTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GMatrixData& aMatrix, TInt& aResult) = 0;

    /**
     * Get a media time
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aSeconds Media time to be returned
     * @throws exception if not ok
     */
    virtual void GetMediaTimeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TReal32& aSeconds) = 0;

    /**
     * Get a next element sibling
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aSiblingElementHandle Sibling handle
     * @throws exception if not ok
     */
    virtual void GetNextElementSiblingL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aSiblingElementHandle) = 0;

    /**
     * Get the segment count for the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aNumberOfSegments Segment count
     * @throws exception if not ok
     */
    virtual void GetNumberOfSegmentsL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt& aNumberOfSegments) = 0;

    /**
     * Get a parent
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aParentElementHandle Parent Element handle
     * @throws exception if not ok
     */
    virtual void GetParentL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aParentElementHandle) = 0;

    /**
     * Get a path trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aPathTrait Path trait
     * @throws exception if not ok
     */
    virtual void GetPathTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt& aPathTrait) = 0;

    /**
     * Get a rect trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aRectData Rect components are saved to this parameter
     * @param aResult Result
     * @throws exception if not ok
     */
    virtual void GetRectTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TM2GRectData& aRectData, TInt& aResult) = 0;

    /**
     * Get a root element
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aRootElementHandle Root element handle
     * @throws exception if not ok
     */
    virtual void GetRootElementL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TM2GSvgElementHandle& aRootElementHandle) = 0;

    /**
     * Get screen b box
     * @since Series S60 3.1
     * @param aElementHandle Element handle
     * @param aScreenBBoxData Screen box data out
     * @throws exception if not ok
     */
    virtual void GetScreenBBoxL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GScreenBBoxData& aScreenBBoxData) = 0;

    /**
     * Get the segment parameter for the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aSegmentIndex -
     * @param aParamIndex -
     * @param aSegmentParam Segment parameter to be returned.
     * @throws exception if not ok
     */
    virtual void GetSegmentParameterL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt aSegmentIndex, TInt aParamIndex,
        TReal32& aSegmentParam) = 0;

    /**
     * Get the segment type for the given path.
     * @since Series S60 3.1
     * @param aPathHandle Path handle.
     * @param aSegmentIndex -
     * @param aSegmentType Segment type
     * @throws exception if not ok
     */
    virtual void GetSegmentTypeL(
        const TM2GSvgPathHandle& aPathHandle,
        TInt aSegmentIndex,
        TInt16& aSegmentType) = 0;

    /**
     * Get a string trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type.
     * @param aStr Returns a string attribute

     * @throws exception if not ok
     */
    virtual TInt GetStringTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TPtrC16& aStr) = 0;
    /**
     * Get a svg surface height
     * @since Series S60 3.1
     * @param aSurfaceHandle Surface handle.
     * @return Svg surface height
     * @throws exception if not ok
     */
    virtual TInt GetSvgSurfaceHeightL(const TM2GBitmapHandle& aSurfaceHandle) = 0;

    /**
     * Get a svg surface width
     * @since Series S60 3.1
     * @param aSurfaceHandle Surface handle.
     * @return Svg surface width
     * @throws exception if not ok
     */
    virtual TInt GetSvgSurfaceWidthL(const TM2GBitmapHandle& aSurfaceHandle) = 0;

    /**
     * Finds handle to the actual cloned element
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aHandle Element handle to be returned
     * @throws exception if not ok
     */
    virtual void GetUsedFromElementL(
        const TM2GSvgElementHandle& aElementHandle,
        TM2GSvgElementHandle& aHandle) = 0;

    /**
     * Viewport height.
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aHeight Wieport height to be returned.
     * @throws exception if not ok
     */
    virtual void GetViewportHeightL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aHeight) = 0;

    /**
     * Viewport width.
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aWidth Viewport width.
     * @throws exception if not ok
     */
    virtual void GetViewportWidthL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt& aWidth) = 0;

    /**
     * Init native engine & document
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle
     * @throws exception if not ok
     */
    virtual void InitViewportL(
        const TM2GSvgDocumentHandle& aDocumentHandle) = 0;

    /**
     * Add given child element to the given element before
     * the given reference element.
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aNewChildElementHandle Child element handle.
     * @param aReferenceElementHandle Child element handle.
     * @throws exception if not ok
     */
    virtual void InsertBeforeL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aNewChildElementHandle,
        const TM2GSvgElementHandle& aReferenceElementHandle) = 0;

    /**
     * Check if element is active.
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aActive Return "1" if active
     * @throws exception if not ok
     */
    virtual void IsActiveL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aActive) = 0;

    /**
     * Checks if an element in DOM.
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aElementHandle Element handle.
     * @param aIsElementInDom Result.
     * @throws exception if not ok
     */
    virtual void IsElementInDomL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aIsElementInDom) = 0;

    /**
     * Checks whether this element is child of a use element
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aResult Return "1" if used otherwise returns "0"
     * @throws exception if not ok
     */
    virtual void IsUsedL(
        const TM2GSvgElementHandle& aElementHandle,
        TInt& aResult) = 0;

    /**
     * Remove child
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aURI Uri
     * @param aResourceData Resource
     * @param aHandle Removed element handle
     * @throws exception if not ok
     */
    virtual void RemoveChildL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgElementHandle& aChildElementHandle,
        TM2GSvgElementHandle& aHandle) = 0;
    /**
     * Render document
     * @since Series S60 3.1
     * @param aEngineHandle Engine handle.
     * @param aDocumentHandle Document handle.
     * @param aSurfaceHandle Surface.
     * @param aSurfaceMaskHandle Mask.
     * @throws exception if not ok
     */
    virtual void RenderDocumentL(
        const TM2GSvgEngineHandle& aEngineHandle,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GBitmapHandle& aSurfaceHandle,
        TM2GBitmapHandle aSurfaceMaskHandle,
        TReal32 aCurrentTime) = 0;

//    pankaj Qimage related changes TODO following Changes done 
    virtual void RenderDocumentL(
        const TM2GSvgEngineHandle& aEngineHandle,
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TM2GSvgBitmapHandle& aSurfaceHandle,
        TM2GSvgBitmapHandle aSurfaceMaskHandle,
        TReal32 aCurrentTime)=0;
    
    /**
     * Sets render quality
     * @since Series S60 3.1
     * @param aEngineHandle Engine handle.
     * @param aQuality Quality.
     * @throws exception if not ok
     */
    virtual void RenderQualityL(
        const TM2GSvgEngineHandle& aEngineHandle,
        TInt aQuality) = 0;

    /**
     * Request completed
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aURI -
     * @param aResourceData -
     * @param aCompeleted Result
     * @throws exception if not ok
     */
    virtual void RequestCompletedL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TPtrC16& aURI,
        const TPtrC8& aResourceData,
        TInt& aCompleted) = 0;

    /**
     * Set color trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aColor Color components: RGB
     * @throws exception if not ok
     */
    virtual void SetColorTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GColorData& aColor) = 0;

    /**
     * Set enum trait
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aValue Value
     * @throws exception if not ok
     */
    virtual void SetEnumTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        TInt16 aValue) = 0;

    /**
     * Set float attribute
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aValue Value
     * @throws exception if not ok
     */
    virtual void SetFloatTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TReal32& aValue) = 0;

    /**
     * Set matrix attribute
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aMatrix -
     * @throws exception if not ok
     */
    virtual void SetMatrixTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GMatrixData& aMatrix) = 0;

    /**
     * Set media time
     * @since Series S60 3.1
     * @param aDocumentHandle Document handle.
     * @param aSeconds Media time
     * @throws exception if not ok
     */
    virtual void SetMediaTimeL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        const TReal32& aSeconds) = 0;

    /**
     * Set path attribute
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aPathHandle -
     * @throws exception if not ok
     */
    virtual void SetPathTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GSvgPathHandle& aPathHandle) = 0;

    /**
     * Set rect attribute
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aRect Rect trait to be set
     * @throws exception if not ok
     */
    virtual void SetRectTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeType,
        const TM2GRectData& aRect) = 0;

    /**
     * Set render quality
     * @since Series S60 3.1
     * @param aEngineHandle Engine handle.
     * @param aMode
     * @throws exception if not ok
     */
    virtual void SetRenderingQualityL(
        const TM2GSvgEngineHandle& aEngineHandle,
        TInt aMode) = 0;

    /**
     * Set string attribute
     * @since Series S60 3.1
     * @param aElementHandle Element handle.
     * @param aAttributeType Attribute type
     * @param aStr String trait to be set
     * @throws exception if not ok
     */
    virtual void SetStringTraitL(
        const TM2GSvgElementHandle& aElementHandle,
        const TM2GSvgAttrType& aAttributeTypeId,
        const TPtrC16& aStr) = 0;

    /**
     * Set viewport height
     * @since Series S60 3.1
     * @param aDocumentHandle Document
     * @param aWidth Width
     * @throws exception if not ok
     */
    virtual void SetViewportHeightL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aHeight) = 0;

    /**
     * Set viewport width
     * @since Series S60 3.1
     * @param aDocumentHandle Document
     * @param aWidth Width
     * @throws exception if not ok
     */
    virtual void SetViewportWidthL(
        const TM2GSvgDocumentHandle& aDocumentHandle,
        TInt aWidth) = 0;
};

M2G_NS_END

#endif // MM2GSVGPROXY_H
