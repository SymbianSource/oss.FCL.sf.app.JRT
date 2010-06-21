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
* Defines the Text Editor native side component
*
*/


#ifndef CMIDTEXTEDITOR_H
#define CMIDTEXTEDITOR_H

// EXTERNAL INCLUDES
#include <e32base.h>

// INTERNAL INCLUDES
#include <MMIDTextEditor.h>
#include <MMIDCustomComponent.h>
#include <MMIDScalable.h>

#ifdef RD_TACTILE_FEEDBACK
#include <peninputclient.h>
#endif // RD_TACTILE_FEEDBACK

#include <coecobs.h>
#include <reflcdui.h>

// FORWARD DECLARATIONS
class CMIDTextEditorEdwin;
class CMIDEditingStateIndicator;
class CMIDEdwinUtils;
class CCoeControl;
class MMIDCustomComponentContainer;
class CMIDUtils;
class MMIDFont;

// CLASS DESCRIPTION
/**
 * Defines the Text Editor native side component.
 */
NONSHARABLE_CLASS(CMIDTextEditor) :
        public CBase,
        public MMIDTextEditor,
        public MMIDCustomComponent,
        public MDirectContent,
        public MCoeControlObserver,
        public MMIDScalable
{
public: // Type definitions

    class TCtorParams
    {
    public:
        // Maximum size of the editor.
        TInt iMaxSize;
        // The width of the editor in pixels.
        TInt iWidth;
        // The height of the editor in pixels or in rows.
        TInt iHeight;
        // Indicates if the size is in pixels or in rows.
        TBool iHeightInRows;
        // The shared CMIDUtils instance. Not owned.
        CMIDUtils* iUtils;

        TCtorParams::TCtorParams() :
                iMaxSize(0),
                iWidth(0),
                iHeight(0),
                iHeightInRows(EFalse),
                iUtils(NULL) {}
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams The construction parameters of this class.
     * @return New instance from this class
     */
    static CMIDTextEditor* NewL(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CMIDTextEditor();

public: // From MMIDCustomComponent

    /**
     * Returns the count of controls in this custom component.
     *
     * Note that the count must not change during the lifetime
     * of the object. The count of all components is updated
     * when it is added to the custom component container.
     *
     * This is done in order to improve the performance when counting
     * countrols.
     *
     * @return The count of controls in this custom component.
     * @since S60 5.0
     */
    TInt CustomComponentControlCount() const;

    /**
     * Returns an UI control of this custom component.
     *
     * The index specifies which component is requested. The method
     * works similar to CCoeControl::ComponentControl() method. The
     * ownership of the control is NOT transferred to the caller.
     *
     * @param aIndex The index of the control to be retrieved.
     * @return The UI control of this custom component.
     * @since S60 5.0
     */
    CCoeControl* CustomComponentControl(TInt aIndex);

    /**
     * Indicates that the container is about to be disposed.
     *
     * The custom component does not have to remove itself
     * from the container since the container removes all components
     * when disposing.
     *
     * @since S60 5.0
     */
    void CustomComponentContainerDisposing();

    /**
     * Handles traversing.
     *
     * Used when pressing key down and up.
     *
     * @param aEvent Key Event.
     * @since S60 5.0
     */
    void TraverseL(const TKeyEvent& aEvent);

    /**
     * Processes the received pointer event.
     *
     * @param  aPointerEvent The pointer event. iPosition will be reset to the
     *         parent window position. iParentPosition will be invalid for the
     *         container.
     * @since S60 5.0
     */
    void ProcessPointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * Gets the current touch-enabled state
     *
     * @return true if the editor is touch-enabled, false otherwise
     * @since S60 5.0
     */
    TBool IsTouchEnabled();

public: // From MMIDTextEditor

    /**
     * Specifies whether or not the editor will receive touch-events.
     *
     * @param enabled true to enabled touch-event, false to disable
     * @since S60 5.0
     */
    void SetTouchEnabled(TBool aEnabled);

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
     * @since S60 5.0
     */
    void SetObserver(MMIDTextEditorObserver* aObserver);

    /**
     * Sets the size of this text editor.
     *
     * Note: Calling this function may result in a call to
     *        CMIDTextEditorEdwin::SizeChanged().
     *
     * @param aWidth
     *          The width of the editor in pixels.
     * @param aHeight
     *          The height of the editor in pixels.
     * @since S60 5.0
     */
    void SetEditorSize(TInt aWidth, TInt aHeight);

    /**
     * Returns the size of this editor.
     *
     * @return The size of this editor.
     * @since S60 5.0
     */
    TSize EditorSize() const;

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
    void SetParentL(MMIDCustomComponentContainer* aComponentContainer);

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
    void SetDirectContainerL(MDirectContainer* aDirectContainer);

    /**
     * Sets the elevation of this text editor component.
     *
     * If the specified elevation exeeds to amount of items in the
     * custom component container, the item becomes the topmost item
     * in the custom component stack.
     *
     * Note that the elevation requsted may not be the actual elevation
     * of the item (if, for example, the elevation is higher than the
     * amount of all items, the elevation is set to the top).
     *
     * @param aElevation The new elevation (Z-position) of this item.
     * @since S60 5.0
     */
    void SetElevationL(TInt aElevation);

    /**
     * Returns the elevation of this text editor component.
     *
     * @return The elevation of this text editor component.
     * @since S60 5.0
     */
    TInt Elevation();

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
    void SetVisibleL(TBool aVisible);

    /**
     * Returns the visibility status of the editor.
     *
     * @return <code>ETrue</code> if the editor is visible and
     *         <code>EFalse</code> if not.
     * @since S60 5.0
     */
    TBool IsVisible() const;

    /**
     * Sets the position of this text editor component.
     *
     * Note: Calling this function results in a call to
     *          CMIDTextEditorEdwin::PositionChanged().
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    void SetPosition(TInt aX, TInt aY);

    /**
     * Sets the focus status of this text editor component.
     *
     * Setting the fucus to a text editor starts the editor to capture key
     * events. The key events are comsumed by the editor and the parent
     * will not possibly receive events at that time.
     *
     * @param aFocusState If <code>true</code>, the text editor component
     *        will start capturing key events from its parent. If
     *        <code>false</code> the focues is removed from this text
     *        editor.
     * @since S60 5.0
     */
    void SetFocusStateL(TBool aFocusState);

    /**
     * Gets the focus status of this text editor component.
     *
     * @return focus state of text component.
     * @since S60 5.0
     */
    TInt GetFocusState();

    /**
     * Returns the content of this text editor in a new <code>HBufC</code>
     * descriptor. The ownership of the returned value is transferred to
     * the caller.
     *
     * @param The content of the editor in a new <code>HBufC</code>
     * descriptor.
     * @since S60 5.0
     */
    HBufC* ContentL();

    /**
     * Sets a new content to this text editor.
     *
     * The content replaces any previous content in this text editor.
     * Note that if the current set of text constraints does not allow
     * the new content, the method leaves with <code>KErrArgument</code>.
     *
     * @param aContent The new content for this text editor.
     * @since S60 5.0
     */
    void SetContentL(const TDesC& aContent);

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
    void InsertContentL(const TDesC& aContent, TInt aPosition);

    /**
     * Deletes the content from this text editor.
     *
     * The content is deleted based on the given offset and length. If the
     * resulting content is not valid for the current set of constraints,
     * this method leaves with <code>KErrArgument</code>
     *
     * The offset and length must specify a valid range within the
     * content.
     *
     * @param aOffset The beginning of the region to be deleted.
     * @param aLength The number of characters to be deleted.
     * @since S60 5.0
     */
    void DeleteContentL(TInt aOffset, TInt aLength);

    /**
     * Returns the length of the current content in this text editor.
     *
     * @return The length of the current content.
     * @since S60 5.0
     */
    TInt ContentLength() const;

    /**
     * Returns the height of the full content in this text editor.
     *
     * @return The height of the full content.
     * @since S60 5.0
     */
    TInt ContentHeight() const;

    /**
     * Returns the line margin height of this text editor.
     *
     * @return The line margin height of this text editor.
     * @since S60 5.0
     */
    TInt LineMarginHeight() const;

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
    TInt VisibleContentPosition() const;

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
    void SetSelectionL(TInt aIndex, TInt aLength);

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
    HBufC* SelectionL();

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
    void SetConstraintsL(TUint aConstraints);

    /**
     * Sets the multiline status of this text editor component.
     *
     * TextEditor is by default single-line editor, which means that user
     * is not possible to insert line breaks to the editor. A possible Enter
     * (or similar) key event should be passed in this case to the parent object
     * as a normal key event for client to handle it as appropriately. A single-line
     * editor will have horizontal scrolling of text if it is possible to enter text
     * that does not fit to the editor. This may happen if maximum size is large enough
     * for the width of the editor.
     * In multi-line editor user is able to insert line breaks but also word
     * wrapping is enabled automatically on word boundaries.
     *
     * @param aMultiline <code>true</code> if multi-line editor,
     *        <code>false</code> if single-line editor.
     */
    void SetMultilineL(TBool aMultiline);

    /**
     * Gets the multiline status of this text editor component.
     *
     * @return <code>true</code> if multi-line editor,
     *        <code>false</code> if single-line editor.
     */
    virtual TBool IsMultiline();

    /**
     * Sets a hint to the implementation as to the input mode that should
     * be used when the user initiates editing of this text editor.
     *
     * The character subset parameter names and subset of Unicode characters
     * that is used by the implementation to choose an initial input mode.
     *
     * If an empty string is passed, the implementation uses the default
     * character set.
     *
     * @param aCharacterSubset The character subset for the initial input
     *        mode.
     * @since S60 5.0
     */
    void SetInitialInputModeL(const TDesC& aCharacterSubset);

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
    TInt SetMaxSizeL(TInt aMaxSize);

    /**
     * Sets a new position for the cursor within this text editor.
     *
     * Note that the index must be valid (i.e. within the limit
     * of the current content length).
     *
     * @param aIndex The new index for the cursor.
     * @since S60 5.0
     */
    void SetCursorPositionL(TInt aIndex);

    /**
     * Returns the current position of the cursor in the text editor.
     *
     * @return The current position of the cursor in the text editor.
     * @since S60 5.0
     */
    TInt CursorPosition() const;

    /**
     * Sets the color for the specified color type defined in TColorType.
     *
     * @param aColor The color to be set.
     * @param aColorType The type of the color to be set.
     * @since S60 5.0
     */
    void SetColorL(const TRgb& aColor, TColorType aColorType);

    /**
     * Gets the color of the specified color type defined in TColorType.
     *
     * @param aColorType The type of the color to get.
     *
     * @return The color of the type.
     * @since S60 5.0
     */
    TInt GetColor(TColorType aColorType);

    /**
     * Sets a new font for this text editor.
     *
     * @param aFont The new font.
     * @since S60 5.0
     */
    void SetFontL(MMIDFont* aFont);

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
    void SetPreferredTouchInputMode(TInt aInputMode);

    /**
     * Gets the preferred touch input mode which is currently active.
     *
     * @see AknFepGlobalEnums.h
     *
     * @return The preferred touch input mode.
     * @since S60 5.0
     */
    TInt PreferredTouchInputMode();

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
    void SetDisabledTouchInputModes(TInt aInputModes);

    /**
     * Gets the disabled touch input modes which are currently not
     * available in the touch input UI.
     *
     * @see AknFepGlobalEnums.h
     *
     * @return The disabled touch input modes.
     * @since S60 5.0
     */
    TInt DisabledTouchInputModes();

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
    void SetIndicatorVisibilityL(TBool aVisible);

    /**
     * Sets the custom indicator's position.
     *
     * Position is relative to the parent object of this text editor.
     *
     * @param aX The x coordinate of the anchor point.
     * @param aY The y coordinate of the anchor point.
     * @since S60 5.0
     */
    void SetIndicatorPosition(TInt aX, TInt aY);

    /**
     * Returns the size of the area needed for drawing the custom
     * editing state indicator.
     *
     * Currently, the size cannot be adjusted through this interface.
     *
     * @return The size of the custom editing state indicator.
     * @since S60 5.0
     */
    TSize IndicatorSize() const;

    /**
     * Sets the editor to use default indicator container.
     *
     * @since S60 5.0
     */
    void SetDefaultIndicatorsL();

public: // From MMIDScalable

    TBool IsScalingOn() const;

public: // own methods

    /**
     * Handles fullscreen state of parent Canvas.
     *
     * @since S60 5.0
     */
    void HandleFullscreenModeChange();

    /**
     * Handles changes of screen orientation.
     *
     * @since S60 5.0
     */
    void HandleResolutionChange();

    /**
     * Sets the caret in the Editor at x, y location.
     *
     * @param x
     *      The x coordinate of the wanted caret position.
     *
     * @param y
     *      The y coordinate of the wanted caret position.
     */
    void SetCaretXYL(TInt aX, TInt aY);

public: // From MMIDComponent

    /**
     * Invoked by the framework to clean up any  resources used by the
     * implementation.
     *
     * Typically, the implementation will do "delete this" but is clearly
     * not restricted to
     *
     * @since S60 5.0
     */
    void Dispose();

public: // From MDirectContent

    /**
     * Notify content that container window rect is available.
     *
     * @param aRect Rwindow rect on which content can be displayed
     * @since S60 5.2
     */
    void MdcContainerWindowRectChanged(const TRect& /*aRect*/);

    /**
     * Notify content that container visiblity has changed. The content
     * must not be displayed when it's container is invisible.
     *
     * @param aVisible ETrue if the container is visible, EFalse if it is
     *        invisible.
     * @since S60 5.0
     */
    void MdcContainerVisibilityChanged(TBool aVisible);

    /**
     * Notify content that the container bounds have changed.
     *
     * @param aRect Maximum area the content can occupy in screen
     *        co-ordinates. The content should be clipped to this area.
     * @since S60 5.0
     */
    void MdcContentBoundsChanged(const TRect& aRect);

    /**
     * Notify content that its display area has changed.
     * This should only be called when the content is displayed on an
     * Item.
     *
     * @param aDisplayRect Display area in Item co-ordinates.
     * @param aScreenRect Display area in screen co-ordinates.
     * @since S60 5.0
     */
    void MdcItemContentRectChanged(
        const TRect& aContentRect, const TRect& aScreenRect);

    /**
     * Notify content that container is about to be destoryed.
     *
     * @since S60 5.0
     */
    void MdcContainerDestroyed();

    /**
     * Aborts DSA.
     *
     * @since S60 5.0
     */
    void MdcAbortDSA();

    /**
     * Resumes DSA.
     *
     * @since S60 5.0
     */
    void MdcResumeDSA();


public: // From MCoeControlObserver

    /**
     * Handles an event from an observed control.
     *
     * This function is called when a control for which this control is
     * the observer calls CCoeControl::ReportEventL(). It should be
     * implemented by the observer control, and should handle all events
     * sent by controls it observes.
     *
     * @param aControl The control that sent the event.
     * @param aEventType The event type.
     * @since S60 5.0
     */
    void HandleControlEventL(
        CCoeControl* aControl,
        TCoeEvent aEventType);

private: // New methods

    /**
     * Updates the indicator's position based on the editor's current
     * position.
     *
     * Nothing is done if indicator position is user defined.
     *
     * @since S60 5.0
     */
    void UpdateIndicatorPosition();

    /**
     * Returns a converted content.
     *
     * The method performs locale, language and phone number specific
     * conversion for the given text. The ownership is transferred to
     * the caller.
     *
     * The returned value is left to the cleanup stack.
     *
     * @param aContent The content to be converted.
     * @return The converted content.
     * @since S60 5.0
     */
    HBufC* ConvertedContentLC(const TDesC& aContent);

private: // Construtors

    /**
     * Returns result of connecting to pen input server
     *
     * @return ETrue if connected, EFalse otherwise.
     * @since S60 5.0
     */
    TBool isConnected();

    /**
     * C++ constructor.
     */
    CMIDTextEditor();

    /**
     * Second phase constructor
     *
     * @param aParams The construction parameters of this class.
     */
    void ConstructL(const TCtorParams& aParams);

private: // Data

    // The current set of constraints.
    TUint iConstraints;
    // True if editor was created with row count and size is not changed.
    TBool iRowCountActive;
    // Row count that has been used to initialize the text editor.
    TUint iRowCount;
    // Editing state indicator container. Owned.
    CMIDEditingStateIndicator* iEditingStateIndicator;
    // Text editor control. Owned.
    CMIDTextEditorEdwin* iTextEdwin;
    // Editor window utils. Owned.
    CMIDEdwinUtils* iEdwinUtils;
    // Container for this object. Not owned.
    MMIDCustomComponentContainer* iComponentContainer;
    // Container for the direct content. Not owned.
    MDirectContainer* iDirectContainer;
    // Text editor observer. Not owned.
    MMIDTextEditorObserver* iObserver;
    // Text editor focus state.
    TBool iFocusState;
    // Indicates receiveing of pointer events on focused editor.
    TBool iTouchEnabled;
    // The shared CMIDUtils instance. Not owned.
    CMIDUtils* iUtils;
    // Nonscaled CMIDFont. This is needed for corect scaling. Owned.
    MMIDFont* iNonScaledFont;
    // Nonscaled position of CMIDTextEditor. This is needed for corect scaling.
    TPoint iNonScaledPosition;
    // Nonscaled editor size. This is needed for correct scaling.
    TSize iNonScaledEditorSize;

#ifdef RD_TACTILE_FEEDBACK
    // True if editor is already connected to the pen input server
    TBool iPenInputServerConnected;
    // Pen input client for touch input handling.
    RPeninputServer iPenServer;
#endif // RD_TACTILE_FEEDBACK
};

#endif // CMIDTEXTEDITOR_H

// End of file
