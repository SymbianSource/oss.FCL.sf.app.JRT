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
* Description: Defines an interface for LCDUI Text editor component.
*
*/


#ifndef MMIDTEXTEDITOR_H
#define MMIDTEXTEDITOR_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <lcdui.h>

// FORWARD DECLARATIONS
class MMIDCustomComponentContainer;

// CLASS DESCRIPTION
/**
 * Observer interface for listening actions in a text editor.
 *
 * @since S60 5.0
 */
class MMIDTextEditorObserver : public MMIDComponent
{
public: // Type definitions

    /**
     * Input actions for the observer to handle.
     */
    enum TInputAction
    {
        // The content of the editor has changed.
        EActionContentChange = 0x1,
        // The options of the editor have changed.
        EActionOptionsChange = 0x2,
        // The caret in the editor has moved.
        EActionCaretMove = 0x4,
        // Traverse to the previous item.
        EActionTraversePrevious = 0x8,
        // Traverse to the next item.
        EActionTraverseNext = 0x10,
        // Repaint request to the parent.
        EActionPaintRequest = 0x20,
        // Direction of the writing language has changed.
        EActionDirectionChange = 0x40,
        // The input mode in the editor has changed.
        EActionInputModeChange = 0x80,
        // The langauge in the editor has changed.
        EActionLanguageChange = 0x100,
        // A character is deleted from the editor.
        EActionCharacterDeletion = 0x200,
        // Focus of the text editor has changed
        EActionFocusChange = 0x400
    };

public: // New methods

    /**
     * Called when an input action occurs in the editor.
     *
     * Some events can be controlled by the observer. In this case the
     * return value is checked when the editor performs the operation.
     * Note that there are some actions that are performed regardless of
     * the return value of this method call.
     *
     * @param aActions contains the set of input actions that occured.
     * @since S60 5.0
     */
    virtual void NotifyInputAction(TUint aActions) = 0;

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDTextEditorObserver() {}

};

// CLASS DESCRIPTION
/**
 * Interface for the Text editor component.
 *
 * This interface can be used to interact as a custom UI component
 * in LCDUI components that implement a container for custom
 * components.
 *
 * The client can use this interface to access information about
 * UI controls that the implementing class provides.
 *
 * @since S60 5.0
 */
class MMIDTextEditor : public MMIDComponent
{
public: // Type definitions

    enum TColorType
    {
        // Color indicator for background color.
        EColorBackground,
        // Color indicator for foregraound (font) color.
        EColorForeground,
        // Color indicator for highlight background color.
        EColorHighlightBackground,
        // Color indicator for highlight foreground color.
        EColorHighlightForeground
    };

    enum TFocusState
    {
        // Focus off
        EFocusStateNo,
        // Partial focus - not used on S60 text editor
        EFocusStatePartial,
        // Focus on
        EFocusStateFull
    };

public: // New methods
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    virtual TType Type() const
    {
        return ECanvasTextEditor;
    }

    /**
     * Sets the observer for this text editor component.
     *
     * The observer is notified when an action is about to happen
     * in the text editor.
     *
     * The ownership of the object is not transferred to this component.
     *
     * @param aObserver The text editor observer or <code>NULL</code>
     *        if the observer should be removed.
     */
    virtual void SetObserver(MMIDTextEditorObserver* aObserver) = 0;

    /**
     * Sets the size of this text editor.
     *
     * @param aWidth
     *          The width of the editor in pixels.
     * @param aHeight
     *          The height of the editor in pixels.
     * @since S60 5.0
     */
    virtual void SetEditorSize(TInt aWidth, TInt aHeight) = 0;

    /**
     * Returns the size of this editor.
     *
     * @return The size of this editor.
     * @since S60 5.0
     */
    virtual TSize EditorSize() const = 0;

    /**
     * Sets the parent of this text editor component. The parent must
     * implement the custom component container interface in order to
     * provide necessary services for registering custom UI components.
     *
     * The ownership of the parent is not transffered to this object.
     *
     * @param aComponentContainer The parent MIDP custom component
     *        container.
     * @since S60 5.0
     */
    virtual void SetParentL(
        MMIDCustomComponentContainer* aComponentContainer) = 0;

    /**
     * Sets the direct container of this text editor component.
     *
     * Note that the text editor does not necessary need to have direct
     * container. If the parent component is responsible for somekind of
     * custom drawing using direct screena access, this method can be used
     * to register the direct container.
     *
     * The text editor adds itself to the direct container so that it
     * is not able to draw on top of the text editor when direct content
     * is added to the screen.
     *
     * @param aDirectContainer The direct container of this compoennt if
     *        any. <code>NULL</code> removes the current container.
     * @since S60 5.0
     */
    virtual void SetDirectContainerL(
        MDirectContainer* aDirectContainer) = 0;

    /**
     * Sets the elevation of this text editor component.
     *
     * If the specified elevation exeeds to amount of items in the
     * custom component container, the item becomes the topmost item
     * in the custom component stack.
     *
     * Note that the elevation requsted may not be the actual elevation
     * of the item (if, for example, the elevation is higher than the
     * amount of all items). The actual elevation can be fetched using
     * <code>Elevation()</code>.
     *
     * @param aElevation The new elevation (Z-position) of this item.
     * @since S60 5.0
     */
    virtual void SetElevationL(TInt aElevation) = 0;

    /**
     * Returns the elevation of this text editor component.
     *
     * @return The elevation of this text editor component.
     * @since S60 5.0
     */
    virtual TInt Elevation() = 0;

    /**
     * Sets this text editor component visible if it is hidden.
     *
     * Depending on the current status of the text editor, this operation
     * is no-op if there is nothing to do. (i.e. the text editor is set
     * hidden when it is already hidden).
     *
     * @param aVisible Indicates the visibility status of the text editor.
     * @since S60 5.0
     */
    virtual void SetVisibleL(TBool aVisible) = 0;

    /**
     * Returns the visibility status of the editor.
     *
     * @return <code>ETrue</code> if the editor is visible and
     *         <code>EFalse</code> if not.
     * @since S60 5.0
     */
    virtual TBool IsVisible() const = 0;

    /**
     * Sets the position of this text editor component.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    virtual void SetPosition(TInt aX, TInt aY) = 0;

    /**
     * Sets the focus status of this text editor component.
     *
     * Setting the focus to a text editor starts the editor to capture key
     * events. The key events are comsumed by the editor and the parent
     * will not possibly receive events at that time.
     *
     * @param aFocusState If <code>EFocusStateFull</code>, the text editor
     *        component will start capturing key events from its parent.
     *        If <code>EFocusStateNo</code> the focues is removed from
     *        this text editor.
     * @since S60 5.0
     */
    virtual void SetFocusStateL(TInt aFocusState) = 0;

    /**
     * Gets the focus state of this text editor component.
     *
     * @return Current focus state text editor component.
     * @since S60 5.0
     */
    virtual TInt GetFocusState() = 0;

    /**
     * Returns the content of this text editor in a new <code>HBufC</code>
     * descriptor. The ownership of the returned value is transferred to
     * the caller.
     *
     * @return The content of the editor in a new <code>HBufC</code>
     *         descriptor.
     * @since S60 5.0
     */
    virtual HBufC* ContentL() = 0;

    /**
     * Sets a new content to this text editor.
     *
     * The content replaces any previous content in this text editor.
     * Note that if the current set of text constraints does not allow the
     * new content, the method leaves with <code>KErrArgument</code>.
     *
     * @param aContent The new content for this text editor.
     * @since S60 5.0
     */
    virtual void SetContentL(const TDesC& aContent) = 0;

    /**
     * Inserts a content into the current content of this text editor.
     *
     * The content is inserted just prior to the character indicated by
     * the position parameter, where zero specifies the first character of
     * the content in this text editor.
     *
     * If position is less than or equal to zero, the insertion occurs at
     * the beginning of the content. If position is greater than or equal
     * to the current size of the content, the insertion occurs
     * immediately after the end of the content.
     *
     * @param aContent The content to be inserted.
     * @param aPosition The position at which the insertion starts.
     * @since S60 5.0
     */
    virtual void InsertContentL(
        const TDesC& aContent,
        TInt aPosition) = 0;

    /**
     * Deletes the content from this text editor.
     *
     * The content is deleted based on the given offset and length. If the
     * resulting content is not valid for the current set of constraints,
     * this method leaves with <code>KErrArgument</code>. In this case,
     * the state is not changed.
     *
     * The cursor position is modified depending on the its position
     * versus the range of deleted content.
     *
     * The offset and length must specify a valid range within the
     * content.
     *
     * @param aOffset The beginning of the region to be deleted.
     * @param aLength The number of characters to be deleted.
     * @since S60 5.0
     */
    virtual void DeleteContentL(TInt aOffset, TInt aLength) = 0;

    /**
     * Returns the length of the current content in this text editor.
     *
     * @return The length of the current content.
     * @since S60 5.0
     */
    virtual TInt ContentLength() const = 0;

    /**
     * Returns the height of the full content in this text editor.
     *
     * @return The height of the full content.
     * @since S60 5.0
     */
    virtual TInt ContentHeight() const = 0;

    /**
     * Returns the line margin height of this text editor.
     *
     * @return The line margin height of this text editor.
     * @since S60 5.0
     */
    virtual TInt LineMarginHeight() const = 0;

    /**
     * Returns the topmost pixel position of the topmost visible line in
     * the editor.
     *
     * The returned y coordinate value must be relative to the whole
     * content height, not just the visible part.
     *
     * @return The topmost pixel position of the visible content.
     * @since S60 5.0
     */
    virtual TInt VisibleContentPosition() const = 0;

    /**
     * Sets the selection in this text editor.
     *
     * The selection begins from <code>aIndex</code> and continues
     * the amount of characters specified by <code>aLength</code>.
     *
     * The selected area is highlighted with current highlight
     * colors.
     *
     * The index and length must specify a valid range within the content.
     *
     * @param aIndex The beginning of the range to be selected.
     * @param aLength The length of the range to be selected.
     * @since S60 5.0
     */
    virtual void SetSelectionL(TInt aIndex, TInt aLength) = 0;

    /**
     * Returns the current selection in this text editor in a new
     * <code>HBufC</code> descriptor.
     *
     * The ownership of the returned value is transferred to the caller.
     *
     * @return The current selection int the editor in a new
     *         <code>HBufC</code> descriptor.
     * @since S60 5.0
     */
    virtual HBufC* SelectionL() = 0;

    /**
     * Sets the constraints of this text editor. If the constraints are
     * not valid for the current content of the editor, the content is set
     * to empty.
     *
     * @param aConstraints The new set of constraints. Note that
     *        validation of the input must be done in the client side
     *        (i.e. in the Java side).
     * @since S60 5.0
     */
    virtual void SetConstraintsL(TUint aConstraints) = 0;

    /**
     * Sets a hint to the implementation as to the input mode that should
     * be used when the user initiates editing of this text editor.
     *
     * The character subset parameter names a subset of Unicode characters
     * that is used by the implementation to choose an initial input mode.
     * If an empty string is passed, the implementation uses the default
     * character set.
     *
     * @param aCharacterSubset The character subset for the initial input
     *        mode.
     * @since S60 5.0
     */
    virtual void SetInitialInputModeL(
        const TDesC& aCharacterSubset) = 0;

    /**
     * Sets the maximum size of this text editor.
     *
     * If the maximum size is less than the length of the current content,
     * the content is truncated from the end to fit into the new maximum
     * size.
     *
     * If the content is truncated and the resulting new content is not
     * valid for the current input constraints, this method leaves with
     * <code>KErrArgument</code>.
     *
     * @param aMaxSize The new maximum size for this text editor.
     * @return The actual size that the editor accepted. May be smaller
     *         than the requested maximum size.
     * @since S60 5.0
     */
    virtual TInt SetMaxSizeL(TInt aMaxSize) = 0;

    /**
     * Sets a new position for the cursor within this text editor.
     *
     * Note that the index must be valid (i.e. within the limit
     * of the current content length).
     *
     * @param aIndex The new index for the cursor.
     * @since S60 5.0
     */
    virtual void SetCursorPositionL(TInt aIndex) = 0;

    /**
     * Returns the current position of the cursor in the text editor.
     *
     * @return The current position of the cursor in the text editor.
     * @since S60 5.0
     */
    virtual TInt CursorPosition() const = 0;

    /**
     * Sets the color for the specified color type defined in TColorType.
     *
     * @param aColor The color to be set.
     * @param aColorType The type of the color to be set.
     * @since S60 5.0
     */
    virtual void SetColorL(
        const TRgb& aColor,
        TColorType aColorType) = 0;

    /**
     * Sets a new font for this text editor.
     *
     * @param aFont The new font.
     * @since S60 5.0
     */
    virtual void SetFontL(MMIDFont* aFont) = 0;

    /**
     * Sets the preferred touch input mode.
     *
     * The modes must map to values defined in Pen Input Client API.
     * The specified touch input mode becomes the preferred touch input
     * mode for the current application.
     *
     * @see AknFepGlobalEnums.h
     *
     * @param aInputMode. The preferred touch input mode.
     * @since S60 5.0
     */
    virtual void SetPreferredTouchInputMode(TInt aInputMode) = 0;

    /**
     * Gets the preferred touch input mode which is currently active.
     *
     * @see AknFepGlobalEnums.h
     *
     * @return The preferred touch input mode.
     * @since S60 5.0
     */
    virtual TInt PreferredTouchInputMode() = 0;

    /**
     * Sets the disabled touch input modes.
     *
     * The modes must map to values defined in Pen Input Client API.
     * The specified modes are disabled from the on screen touch
     * input UI and cannot be selected by the user.
     *
     * @see AknFepGlobalEnums.h
     *
     * @param aInputModes The modes to be disabled.
     * @since S60 5.0
     */
    virtual void SetDisabledTouchInputModes(TInt aInputModes) = 0;

    /**
     * Gets the disabled touch input modes which are currently not
     * available in the touch input UI.
     *
     * @see AknFepGlobalEnums.h
     *
     * @return The disabled touch input modes.
     * @since S60 5.0
     */
    virtual TInt DisabledTouchInputModes() = 0;

    /**
     * Sets the custom indicator's visibility.
     *
     * Note that the visibility of the indiator cannot be
     * controlled if the editor is not visible. In that case
     * this method is a no-op.
     *
     * @param aVisible Defines the visibility of the custom indicator.
     * @since S60 5.0
     */
    virtual void SetIndicatorVisibilityL(TBool aVisible) = 0;

    /**
     * Sets the custom indicator's position.
     *
     * Position is relative to the parent object of this text editor.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    virtual void SetIndicatorPosition(TInt aX, TInt aY) = 0;

    /**
     * Returns the size of the area needed for drawing the custom
     * editing state indicator.
     *
     * Currently, the size cannot be adjusted through this interface.
     *
     * @return The size of the custom editing state indicator.
     * @since S60 5.0
     */
    virtual TSize IndicatorSize() const = 0;

    /**
     * Sets the editor to use default indicator container.
     *
     * @since S60 5.0
     */
    virtual void SetDefaultIndicatorsL() = 0;

protected: // Destructor

    /**
     * Destructor. Disallows destruction through this interface
     */
    virtual ~MMIDTextEditor() {}
};

#endif // MMIDTEXTEDITOR_H

// End of file
