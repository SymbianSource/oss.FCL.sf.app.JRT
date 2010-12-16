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
* Defines a custom editor window for Text editor component.
*
*/


#ifndef CMIDTEXTEDITOREDWIN_H
#define CMIDTEXTEDITOREDWIN_H

// INTERNAL INCLUDES
#include "CMIDEdwinUtils.h"
#include "CMIDDisplayable.h"
// Used for scaling
#include <MMIDScalable.h>

// EXTERNAL INCLUDES
#include <eikedwin.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
#include <coreuiavkonlcdui.h>
#endif // RD_JAVA_S60_RELEASE_9_2

// FORWARD DECLARATIONS
class CMIDEditingStateIndicator;
class MMIDTextEditorObserver;
class CAknExtendedInputCapabilities;
class MMIDCustomComponentContainer;
class CMIDTextEditor;

// CLASS DESCRIPTION
/**
 * Defines a custom editor window for Text editor component.
 *
 * The edwin component is derived in order to catch key events before those
 * are delegated to the actual editor. This way the focus and unfocus
 * functionality can be implemented to Text editor component.
 *
 * Additinally, this class performs predefined initialization of the edwin.
 */
NONSHARABLE_CLASS(CMIDTextEditorEdwin) :
        public CEikEdwin,
        public MEikEdwinObserver,
        public MMIDScalable
{
public: // Constructors and destructor

    /**
     * C++ constructor.
     *
     * @param aEdwinUtils A reference to editor window utils.
     */
    CMIDTextEditorEdwin(CMIDEdwinUtils& aEdwinUtils);

    /**
     * Destructor.
     */
    virtual ~CMIDTextEditorEdwin();

    /**
     * Handles Symbian 2nd phase construction.
     *
     * NOTE: This is similar to eikedwin in purpose.
     *
     * Completes construction of a new editor window, initialising its
     * heap-stored members from the supplied arguments.
     *
     * @param aEdwinFlags Bitwise ORed combination of flags from
     *        CEikEdwin::TFlags. Default value is 0.
     * @param aWidthInChars Specifies the width for the editor window,
     *        in characters or pixels: see the @c
     *        CEikEdwin::EWidthInPixels flag. Default value is 0.
     * @param aTextLimit Maximum length for the text to present as
     *        editable. Default value is 0.
     * @param aNumberOfLines Height for the editor window in lines.
     *        Default value is 0.
     */
    void ConstructL(
        TInt aEdwinFlags = 0,
        TInt aWidthInChars = 0,
        TInt aTextLimit = 0,
        TInt aNumberOfLines = 0);

public: // From CCoeControl

    /**
     * Called when a key event is offered to this control.
     *
     * @param aKeyEvent The key event that occured.
     * @param aType The type of the event code.
     * @return Key response.
     * @since S60 5.0
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType);

    /**
     * Indicates if traversal is needed
     *
     * Used when changing focus by key down and up.
     *
     * @param aEvent Key Event.
     * @return <code>ETrue</code> if the traverse is needed,
     *         <code>EFalse</code> otherwise.
     * @since S60 5.0
     */
    TBool Traverse(const TKeyEvent& aEvent);

    /**
     * Draws the background for this editor window and lets the base
     * class to draw on top of the background.
     *
     * @param aRect The rect to which to draw.
     * @since S60 5.0
     */
    void Draw(const TRect& aRect) const;

    /**
     * This method is called when the editor focus changes.
     *
     * @param aDrawNow Contains the value that was passed to it by
     *        <code>SetFocus()</code>.
     * @since S60 5.0
     */
    void FocusChanged(TDrawNow aDrawNow);

    /**
     * This method is called when the position of the editor changes.
     *
     * @since S60 5.0
     */
    void PositionChanged();

    /**
     * Used to override default colors.
     *
     * @see CCoeControl::OverrideColorL()
     *
     * @param aLogicalColor The logical colour. Indicates which part
     *        of a control the physical colour maps to. The set of logical
     *        colours for a standard application are defined in
     *        TLogicalColor.
     *
     * @param aColor The new physical colour to which the logical colour
     *        should be mapped.
     * @since S60 5.0
     */
    void OverrideColorL(TInt aLogicalColor, TRgb aColor);

public: // From CEikEdwin

    /**
     * Creates the custom drawer for this editor window.
     *
     * Ownership is transferred to the caller.
     *
     * @return The custom drawer for this editor window.
     * @since S60 5.0
     */
    CLafEdwinCustomDrawBase* CreateCustomDrawL();

    /**
     * Gets an object whose type is encapsulated by the
     * specified <code>TTypeUid object</code>.
     *
     * @param aId Encapsulates the Uid that identifies the type
     *        of object required.
     * @return Encapsulates the pointer to the object provided.
     *         Note that the encapsulated pointer may be NULL.
     * @since S60 5.0
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * Called when text is pasted to the editor.
     *
     * The default implementation in CEikEdwin does not currently
     * do anything. This method checks that the pasted content.
     * is valid for the given input constraints.
     *
     * @param aStartPos The start index of the pasted text.
     * @param aLength The length of the pasted text.
     * @since S60 5.0
     */
    void HandleTextPastedL(TInt aStartPos, TInt& aLength);

    /**
     * Resource change handling.
     *
     * @param aType Resource change type.
     * @since S60 5.0
     */
    void HandleResourceChange(TInt aType);

    /**
     * Handles external text changes.
     *
     * @since S60 5.0
     */
    void HandleTextChangedL();

public: // From MEikEdwinObserver

    /**
     * This function is invoked by CEikEdwin to report an
     * event to its observer. The observer may respond appropriately.
     *
     * @param aEdwin The originating edwin.
     * @param aEventType The event being reported.
     * @since S60 5.0
     */
    void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

    /**
     * Allows a control created for direct container content display to pass
     * on pointer events.
     *
     * @param  aPointerEvent The pointer event.
     * @since S60 5.0
     */
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2

public: // New methods

#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Gets StatusPane
     *
     * @return pointer to StatusPane.
     * @since S60 5.0
     */
    CEikStatusPane* GetStatusPane();

    /**
     * Handles lost focus.
     *
     * @since S60 5.0
     */
    void FocusLost();

    /**
     * Handles disabling of partial VKB.
     *
     * @since S60 5.0
     */
    void HandlePartialVKBDisable();

#endif // RD_JAVA_S60_RELEASE_9_2

    /**
     * Handles Scrollbar change event sending.
     *
     * @return The scroll bar change event or 0 if it shouldn't be updated.
     *
     * @since S60 5.0
     */
    TInt ScrollBarEvent();

    /**
     * Sets the top parent of this control. Note that the ownership
     * is not transferred to this control.
     *
     * The top parent is used when the editor is set as transparent.
     * If transparency is enabled, the editor must issue redraw to the
     * parent also in order to correctly draw the transparent background.
     *
     * @param aControl The parent control for this control.
     * @param aContainer The container of parent control of this control.
     * @since S60 5.0
     */
    void SetTopParent(CCoeControl* aControl,
                      MMIDCustomComponentContainer* aContainer);

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
     * Sets MID editing state indicator for this editor window.
     *
     * The instance is supplied in <code>MopSupplyObject()</code> if set.
     *
     * @param aIndicator MID editing state indicator. The ownership of the
     *        object is not transferred to this class.
     * @since S60 5.0
     */
    void SetEditingStateIndicator(
        CMIDEditingStateIndicator* aIndicator);

    /**
     * Sets the initial input mode for this edwin.
     *
     * Note that the mode is applied when the next editing session
     * starts (i.e. editor gets focus). This method does not yet
     * perform any changes to the current input mode.
     *
     * @param aCharacterSubset The initial input mode for editing.
     * @since S60 5.0
     */
    void SetInitialInputModeL(const TDesC& aCharacterSubset);

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
     * Returns <code>true</code> if the current constraints mode contains
     * the specified constraint value.
     *
     * @param aConstraint The constraint to be checked.
     * @return <code>true</code> if current constraints mode accepts
     *         the specified constraint.
     * @since S60 5.0
     */
    inline TBool IsConstraintSet(TUint aConstraint) const;

    /**
     * Sets a new position for the cursor within this text editor.
     *
     * @param aIndex The new index for the cursor.
     * @param aSelect <code>ETrue</code> to extend the cursor’s selection
     *        to the new position. <code>EFalse</code> otherwise.
     * @since S60 5.0
     */
    void SetCursorPosL(
        TInt aCursorPosition,
        TBool aSelect);

    /**
     * Returns the current position of the cursor in the text editor.
     *
     * @return The current position of the cursor in the text editor.
     * @since S60 5.0
     */
    TInt CursorPos() const;

    /**
     * Gets the current cursor selection.
     *
     * @return The current cursor selection.
     * @since S60 5.0
     */
    TCursorSelection Selection();

    /**
     * Sets word wrapping on or off for this text editor.
     *
     * @param aWrapIsOn <code>ETrue</code> enables word wrapping,
     *        <code>EFalse</code> disables word wrapping.
     * @since S60 5.0
     */
    void SetWordWrapL(TBool aWrapIsOn);

    /**
     * Returns <code>ETrue</code> if wrap is enabled (i.e. the editor
     * is a multiline editor or <code>EFalse</code> if not.
     *
     * @return <code>ETrue</code> if wrap is enabled.
     * @since S60 5.0
     */
    TBool IsWrapEnabled() const;

    /**
     * Sets the size of this editor window.
     *
     * This method is overwritten due to problems setting size
     * of CEikEdwin before it has container window set.
     *
     * Note: calling this function may result in a call to SizeChanged().
     *
     * @param aWidth The width of the editor in pixels.
     * @param aHeight The height of the editor in pixels.
     * @since S60 5.0
     */
    void SetSize(TInt aWidth, TInt aHeight);

    /**
     * Returns the current size of this edwin.
     *
     * @return The current size of this edwin.
     * @since S60 5.0
     */
    TSize Size() const;

    /**
     * Restores the default input state of the editor window.
     *
     * Note that this sets all input related parameters to their
     * default values. After this, the editor input mode is in the
     * same state as it was when the editor was first constructed.
     *
     * @since S60 5.0
     */
    void RestoreDefaultInputState();

    /**
     * Checks whether this editor window is initialized or not.
     *
     * @return <code>true</code> if the editor window is fully activated
     *         and can be drawn. <code>false</code> otherwise.
     * @since S60 5.0
     */
    TBool IsInitialized() const;

    /**
     * Initializes this text editor edwin component.
     *
     * @param aDisplayable Parent displayable.
     * @since S60 5.0
     */
    void InitializeL(CMIDDisplayable* aDisplayable);

    /**
     * Uninitializes this text editor edwin component.
     *
     * @since S60 5.0
     */
    void Uninitialize();

    /**
     * Returns the calculated height for this text editor edwin component.
     *
     * @return The calculated height for this text editor edwin component.
     * @since S60 5.0
     */
    TInt EditorWindowHeight() const;

    /**
     * Returns the total number of lines in this text editor edwin
     * component.
     *
     * @return The total number of lines in this text editor edwin
     * component
     * @since S60 5.0
     */
    TInt NumberOfLines() const;

    /**
     * Returns the line height of this text editor edwin component.
     *
     * @return The line height of this text editor edwin component
     * @since S60 5.0
     */
    TInt LineHeight() const;

    /**
     * Returns the topmost pixel position of the topmost visible line in
     * the editor.
     *
     * The returned y coordinate value must be relative to the whole
     * content
     * height, not just the visible part.
     *
     * @return The topmost pixel position of the visible content.
     * @since S60 5.0
     */
    TInt VisibleContentPosition() const;

    /**
     * Returns <code>ETrue</code> if draw is ongoing at the moment.
     *
     * @return The drawing state of this control.
     * @since S60 5.0
     */
    TBool DrawOngoing() const;

    /**
     * Returns <code>ETrue</code> if this editor is using transparency.
     *
     * @return The transparency state of this control.
     * @since S60 5.0
     */
    TBool IsTransparent() const;

    /**
     * Redraws the editor's content correctly.
     *
     * Note that this method must be used to redraw the editor correctly
     * in situations that the background color is transparent.
     *
     * @since S60 5.0
     */
    void Redraw() const;

    /**
     * Returns the current content of the editor in a constant
     * descriptor pointer.
     *
     * Note that this returns the clear text if password editor is used.
     * @return The content of the editor.
     * @since S60 5.0
     */
    const TPtrC Read() const;

    /**
     * Sets current fullscreen canvas size.
     *
     * @param Current fullscreen canvas size.
      *
     * @since S60 5.0
     */
    inline void SetOnScreenCanvasRect(const TRect& aSize);

    /**
     * Gets current fullscreen canvas size.
     *
     * @return Current fullscreen canvas size.
      *
     * @since S60 5.0
     */
    inline TRect GetOnScreenCanvasRect() const;

    /**
     * Sets flag if scaling is on now.
     *
     * @param Flag if scaling is on now.
     *
     * @since S60 5.0
     */
    inline void SetScaling(TBool aScaling);

    /**
     * Sets the item. Called during construction of CMIDTextEditor.
     *
     * @param aItem CMIDTextEditorto which is this edwin assigned.
     * @since S60 5.0
     */
    inline void SetItem(CMIDTextEditor* aItem);

public: // From MMIDScalable
    inline TBool IsScalingOn() const;

protected: // New methods.

    /**
     * Handles special key events.
     *
     * @param aKeyEvent The key event that occured.
     * @param aType The type of the event code.
     * @return Key response.
     * @since S60 5.0
     */
    TKeyResponse HandleSpecialKeyEventsL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

private: // New methods

    /**
     * Sets the Edwin’s editable content.
     *
     * @param aText New value for the content.
     * @since S60 5.0
     */
    void SetDocumentContentL(CGlobalText* aText);

    /**
     * Retuns the pointer to the Avkon input capabilities.
     *
     * The ownership of the object is not transferred to the caller.
     * @return Avkon input capabilities.
     * @since S60 5.0
     */
    CAknExtendedInputCapabilities* AknInputCaps() const;

    /**
     * Removes invalid numeric content from the current content.
     *
     * Note that this method must only be called for numeric and decimal
     * editors.
     *
     * @return <code>ETrue</code> if some content was removed and
     *         <code>EFalse</code> if not.
     * @since S60 5.0
     */
    TBool RemoveInvalidNumericContentL();

private: // Data

    // The current position of the cursor.
    TInt iCursorPosition;
    // Current set of constraints.
    TUint iConstraints;
    // Stores the initialization status of this text editor.
    TBool iInitialized;
    // Stores the multiline status of this text editor.
    TBool iMultiline;
    // Part of transparency workaround. Drawing state.
    mutable TBool iDrawInvoked;
    // Current background color.
    TRgb iBackgroundColor;
    // Current content selection.
    TCursorSelection iSelection;
    // The size of the editor before initialization.
    TSize iSize;
    // Workaround for cursor event.
    TInt iCursorPosForAction;
    // Initial input mode current case. Set when editor gets focus.
    TInt iInitialCurrentCase;
    // Initial input mode. Set when editor gets focus.
    TInt iInitialCurrentInputMode;
    // Initial input mode current language. Set when editor gets focus.
    TLanguage iInitialCurrentLanguage;
    // Editor window utils. Used.
    CMIDEdwinUtils& iEdwinUtils;
    // MID Editing state indicator. Not owned.
    CMIDEditingStateIndicator* iIndicator;
    // Text editor observer. Not owned.
    MMIDTextEditorObserver* iObserver;
    // The top parent of this control. Not owned.
    CCoeControl* iParent;
    // Direction of writing
    TAknLayoutId iDirection;
    // Visible content height used for sending scroll event
    TInt iVisibleContentHeight;
#ifdef RD_JAVA_S60_RELEASE_9_2
    // Used for Statuspane visibility
    CAknAppUi* iJavaAppUi;
    /**
     * Displayable.
     * Not own.
     */
    CMIDDisplayable* iDisplayable;
    // Container on which it is. Now own.
    MMIDCustomComponentContainer* iContainer;
#endif // RD_JAVA_S60_RELEASE_9_2
    // Canvas fullscreen size
    TRect iOnScreenCanvasRect;
    // Flag if scaling is on now.
    TBool iIsScalingOn;
    // TextEditor on which it is. not own
    CMIDTextEditor* iItem;
};

// ---------------------------------------------------------------------------
// CMIDTextEditorEdwin::IsConstraintSet
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
inline TBool CMIDTextEditorEdwin::IsConstraintSet(TUint aConstraint) const
{
    return (iConstraints & MMIDTextField::EConstraintMask) == aConstraint;
}

inline void CMIDTextEditorEdwin::SetOnScreenCanvasRect(const TRect& aRect)
{
    iOnScreenCanvasRect = aRect;
}

inline TRect CMIDTextEditorEdwin::GetOnScreenCanvasRect() const
{
    return iOnScreenCanvasRect;
}

inline TBool CMIDTextEditorEdwin::IsScalingOn() const
{
    return iIsScalingOn;
}

inline void CMIDTextEditorEdwin::SetScaling(TBool aScaling)
{
    iIsScalingOn = aScaling;
}

inline void CMIDTextEditorEdwin::SetItem(CMIDTextEditor* aItem)
{
    iItem = aItem;
}

#endif // CMIDTEXTEDITOREDWIN_H

// End of file
