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
* Description: Defines the Text Editor native side component
*
*/

// EXTERNAL INCLUDES
#include <MMIDCustomComponentContainer.h>
#include <lcdui.h>
#include <aknappui.h>
#include <aknnavi.h>
#include <j2me/jdebug.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
// Used with partial VKB
#include <AknPriv.hrh>
#endif // RD_JAVA_S60_RELEASE_9_2

// INTERNAL INCLUDES
#include "CMIDTextEditor.h"
#include "CMIDTextEditorEdwin.h"
#include "CMIDEditingStateIndicator.h"
#include "CMIDEdwinUtils.h"
#include "CMIDUtils.h"
#include "CMIDFont.h"
#include "CMIDDisplayable.h"

// Default background color.
const TInt KDefColorBgRed       = 0;
const TInt KDefColorBgGreen     = 0;
const TInt KDefColorBgBlue      = 0;
const TInt KDefColorBgAlpha     = 0;

// Default foreground color.
const TInt KDefColorFgRed       = 0;
const TInt KDefColorFgGreen     = 0;
const TInt KDefColorFgBlue      = 0;
const TInt KDefColorFgAlpha     = 255;

// Default highlight background color.
const TInt KDefColorHlBgRed     = 0;
const TInt KDefColorHlBgGreen   = 0;
const TInt KDefColorHlBgBlue    = 0;
const TInt KDefColorHlBgAlpha   = 255;

// Default highlight foreground color.
const TInt KDefColorHlFgRed     = 255;
const TInt KDefColorHlFgGreen   = 255;
const TInt KDefColorHlFgBlue    = 255;
const TInt KDefColorHlFgAlpha   = 255;

// Count of compound components in this custom component.
const TInt KComponentCount      = 2;

// For all touch input modes to be disabled
const TInt KAllTouchModesDisabled    = 63;

// ---------------------------------------------------------------------------
// CMIDTextEditor::NewL
// ---------------------------------------------------------------------------
//
CMIDTextEditor* CMIDTextEditor::NewL(const TCtorParams& aParams)
{
    CMIDTextEditor* self = new(ELeave) CMIDTextEditor();
    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::~CMIDTextEditor
// ---------------------------------------------------------------------------
//
CMIDTextEditor::~CMIDTextEditor()
{
    DEBUG("CMIDTextEditor::~CMIDTextEditor +");

#ifdef RD_TACTILE_FEEDBACK
    if (iPenInputServerConnected)
    {
        // Close pen input server if the device is touch enabled and
        // connection was established
        iPenInputServerConnected = EFalse;
        iPenServer.Close();
    }
#endif // RD_TACTILE_FEEDBACK

    // Remove this component from the container if set.
    if (iComponentContainer)
    {
        iComponentContainer->UnregisterComponent(this);
    }

    // Remove association from the previous direct container.
    if (iDirectContainer)
    {
        iDirectContainer->MdcRemoveContent(this);
    }

    delete iTextEdwin;
    delete iEdwinUtils;
    delete iEditingStateIndicator;

    iUtils = NULL;
    iComponentContainer = NULL;
    iDirectContainer = NULL;

    DEBUG("CMIDTextEditor::~CMIDTextEditor -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CustomComponentControlCount
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::CustomComponentControlCount() const
{
    return KComponentCount;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CustomComponentControl
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CCoeControl* CMIDTextEditor::CustomComponentControl(TInt aIndex)
{
    switch (aIndex)
    {
    case 0:
    {
        return iTextEdwin;
    }
    case 1:
    {
        return iEditingStateIndicator;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CustomComponentContainerDisposing
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::CustomComponentContainerDisposing()
{
    DEBUG("CMIDTextEditor::CustomComponentContainerDisposing +");

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iFocusState)
    {
        iTextEdwin->CloseVKB();
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    // Call SetFocus on edwin
    iTextEdwin->SetFocus(EFalse);
    // Disable the custom indicators as in Avkon if not controlled by
    // the client application.
    if (iEditingStateIndicator->EnabledState() ==
            CMIDEditingStateIndicator::EIndicatorStateRelative)
    {
        iEditingStateIndicator->MakeVisible(EFalse);
    }
    // Set editor window uninitialized
    iTextEdwin->Uninitialize();
    iComponentContainer = NULL;
    DEBUG("CMIDTextEditor::CustomComponentContainerDisposing -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetObserver
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetObserver(MMIDTextEditorObserver* aObserver)
{
    DEBUG("CMIDTextEditor::SetObserver +");

    iObserver = aObserver;
    iTextEdwin->SetObserver(aObserver);

    DEBUG("CMIDTextEditor::SetObserver -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::isTouchEnabled
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditor::IsTouchEnabled()
{
    return iTouchEnabled;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::setTouchEnabled
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetTouchEnabled(TBool aEnabled)
{
    iTouchEnabled = aEnabled;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetEditorSize
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetEditorSize(TInt aWidth, TInt aHeight)
{
    DEBUG_INT2("CMIDTextEditor::SetEditorSize + aWidth=%d, aHeight=%d",
               aWidth, aHeight);

    // We need store non-scaled size of editor for possible resolution
    // and fullscreen mode changes.
    iNonScaledEditorSize = TSize(aWidth, aHeight);

    // If parent is in fullscreen mode then it tries scale input size.
    // Only setting otherwise.
    if (IsScalingOn())
    {
        iTextEdwin->SetSize(iUtils->DoScaling(aWidth, CMIDUtils::EHorizontal),
                            iUtils->DoScaling(aHeight, CMIDUtils::EVertical));
        // Update the size of the indicator. Height does not need to be
        // calculated.
        iEditingStateIndicator->SetSize(iUtils->DoScaling(
                                            aWidth, CMIDUtils::EHorizontal), -1);
    }
    else
    {
        iTextEdwin->SetSize(aWidth, aHeight);
        // Update the size of the indicator. Height does not need to be
        // calculated.
        iEditingStateIndicator->SetSize(aWidth, -1);
    }

    // Updating of position of indicator is needed for change of its size.
    UpdateIndicatorPosition();

    // Redraw the editor
    iTextEdwin->Redraw();

    // Change the flag so now the font size changes will not
    // resize the editor any more.
    iRowCountActive = EFalse;

    DEBUG("CMIDTextEditor::SetEditorSize -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::EditorSize
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TSize CMIDTextEditor::EditorSize() const
{
    DEBUG("CMIDTextEditor::EditorSize");

    TSize editorSize = iTextEdwin->Size();

    // When scaling is on it has to descaled editor size.
    if (IsScalingOn())
    {
        editorSize = iUtils->DoDescaling(editorSize);
    }

    return editorSize;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetParentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetParentL(
    MMIDCustomComponentContainer* aComponentContainer)
{
    DEBUG("CMIDTextEditor::SetParentL +");

    // Remove association from the previous container.
    if (iComponentContainer)
    {
        DEBUG("CMIDTextEditor::SetParentL, unregistering old association");

        // Remove focus
        SetFocusStateL(EFalse);
        // Remove editor from container
        iComponentContainer->UnregisterComponent(this);
        // Set editor window uninitialized
        iTextEdwin->Uninitialize();
        iComponentContainer->Control().DrawDeferred();
        iComponentContainer = NULL;
    }

    // Associate this component to the new container.
    if (aComponentContainer)
    {
        DEBUG("CMIDTextEditor::SetParentL, registering new");

        CCoeControl& control = aComponentContainer->Control();
        // Register container window for the editor.
        iTextEdwin->SetContainerWindowL(control);
        iTextEdwin->SetTopParent(&control);
        // Initialize the editor window.
        CMIDDisplayable* displayable = NULL;
#ifdef RD_JAVA_S60_RELEASE_9_2
        displayable = aComponentContainer->GetDisplayable();
#endif // RD_JAVA_S60_RELEASE_9_2
        iTextEdwin->InitializeL(displayable);
        // Set container window also for the indicator.
        iEditingStateIndicator->SetContainerWindowL(&control);

        // Register this custom component to the container.
        aComponentContainer->RegisterComponentL(this);
    }

    // Store container. NULL is ok.
    iComponentContainer = aComponentContainer;

    if (iComponentContainer)
    {
        if (!iRowCountActive)
        {
            // We need call SetEditorSize to prevent wrong
            // scaling of TextEditor.
            // (only for TextEditor not created with row counter)
            SetEditorSize(iTextEdwin->Size().iWidth, iTextEdwin->Size().iHeight);
        }
        else
        {
            iNonScaledEditorSize = iTextEdwin->Size();
        }
        SetFontL(iNonScaledFont);

        if (iUtils)
        {
            // It sets edwin variables necessary for correct clipping.
            iTextEdwin->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());
            iTextEdwin->SetScaling(IsScalingOn());
    
            // It sets indicator variables necessary for correct clipping.
            iEditingStateIndicator->SetScalingOn(IsScalingOn());
            iEditingStateIndicator->SetCanvasRect(iUtils->GetOnScreenCanvasRect());
        }
    }
    
    DEBUG("CMIDTextEditor::SetParentL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetDirectContainerL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetDirectContainerL(MDirectContainer* aDirectContainer)
{
    // Remove association from the previous direct container.
    if (iDirectContainer)
    {
        iDirectContainer->MdcRemoveContent(this);
    }

    // Associate this component to the new direct container. Note that this
    // needs to be done in order to disable the direct screen access of
    // the MID parent component. This way it does not draw on top of
    // the text editor when repaint etc. methods are called.
    if (aDirectContainer)
    {
        aDirectContainer->MdcAddContent(this);
    }

    // Store new container. Or NULL if passed.
    iDirectContainer = aDirectContainer;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetElevationL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetElevationL(TInt aElevation)
{
    DEBUG_INT("CMIDTextEditor::SetElevationL +, aElevation=%d", aElevation);

    // Parent must have been set.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    // The elevation is corrected in custom component container.
    iComponentContainer->SetComponentIndexL(this, aElevation);

    DEBUG("CMIDTextEditor::SetElevationL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::Elevation
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::Elevation()
{
    DEBUG("CMIDTextEditor::Elevation +");

    // Parent must have been set.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    TInt index = iComponentContainer->ComponentIndex(this);

    DEBUG_INT("CMIDTextEditor::Elevation - index=%d", index);

    return index;
}

void CMIDTextEditor::TraverseL(const TKeyEvent& aEvent)
{
    if (iTextEdwin->Traverse(aEvent))
    {
        if (iObserver)
        {
            if ((aEvent.iCode == EKeyUpArrow) ||
                    (aEvent.iScanCode == EStdKeyUpArrow))
            {
                iObserver->NotifyInputAction(
                    MMIDTextEditorObserver::EActionTraversePrevious);
            }
            else if ((aEvent.iCode == EKeyDownArrow) ||
                     (aEvent.iScanCode == EStdKeyDownArrow))
            {
                iObserver->NotifyInputAction(
                    MMIDTextEditorObserver::EActionTraverseNext);
            }
        }
    }
    else
    {
        iTextEdwin->OfferKeyEventL(aEvent, EEventKeyDown);
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::Show
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetVisibleL(TBool aVisible)
{
    DEBUG_INT("CMIDTextEditor::SetVisibleL +, aVisible=%d", aVisible);

    // SetParentL must be called before Show.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    if (aVisible && !iTextEdwin->IsVisible())
    {
        DEBUG("CMIDTextEditor::SetVisibleL, setting editor visible");

        // Show the text editor when it is hidden.
        iTextEdwin->MakeVisible(aVisible);

        // If setting visible already focused editor
        if (iFocusState)
        {
            iFocusState = EFalse;
            SetFocusStateL(ETrue);
            // Cursor is not shown automatically if the editor has focus.
            // So, set cursor visible.
            iTextEdwin->SetCursorVisible(ETrue);
        }
        else
        {
            // MakeVisible automatically selects the editor's content
            // Remove the selection at this point. The selection can be
            // set again with the API.
            SetCursorPositionL(iTextEdwin->CursorPos());
            iTextEdwin->Redraw();
        }
    }
    else if (!aVisible && iTextEdwin->IsVisible())
    {
        DEBUG("CMIDTextEditor::SetVisibleL, setting editor hidden");

        if (iFocusState)
        {
            // Remove the focus from the editor also.
            SetFocusStateL(EFalse);
            // Keep focus state for visibility changes
            iFocusState = ETrue;
        }
        // Hide the text editor when it is visible.
        iTextEdwin->MakeVisible(EFalse);

        // Always disable indicator if the editor is hidden.
        iEditingStateIndicator->MakeVisible(EFalse);
        iTextEdwin->Redraw();
    }

    DEBUG("CMIDTextEditor::SetVisibleL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::IsVisible
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditor::IsVisible() const
{
    return iTextEdwin->IsVisible();
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetPosition(TInt aX, TInt aY)
{
    DEBUG_INT2("CMIDTextEditor::SetPosition +, aX=%d, aY=%d", aX, aY);

    // We need store non-scaled position for possible resolution
    // and fullscreen mode changes.
    iNonScaledPosition = TPoint(aX, aY);

    // If parent is in fullscreen mode then it tries scale input position.
    // Only setting otherwise.
    if (IsScalingOn())
    {
        DEBUG_INT2("CMIDTextEditor::SetPosition DoScaling, aX=%d, aY=%d",
                   iUtils->DoScaling(aX, CMIDUtils::EHorizontal),
                   iUtils->DoScaling(aY, CMIDUtils::EVertical));
        iTextEdwin->SetPosition(iUtils->DoScalingAndPositioning(TPoint(aX, aY)));
    }
    else
    {
        iTextEdwin->SetPosition(TPoint(aX, aY));
    }

    // There is needed redraw.
    iTextEdwin->Redraw();

    // Update indicator's position if needed.
    UpdateIndicatorPosition();

    DEBUG("CMIDTextEditor::SetPosition -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetCaretXYL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetCaretXYL(TInt aX, TInt aY)
{
    DEBUG_INT2("CMIDTextEditor::SetCaretXYL +, aX=%d, aY=%d", aX, aY);

    // Requested point should be already recalculated to be relative
    // to editor position on canvas.
    TPoint pos(aX, aY);
    // If position will be found in formatted text, posInfo will be filled
    // with desired informations.
    TTmPosInfo2 posInfo;

    if (iTextEdwin->TextLayout()->FindXyPos(pos, posInfo))
    {
        // Position was found. Try to set cursor to found position.
        iTextEdwin->SetCursorPosL(posInfo.iDocPos.iPos, EFalse);
    }

    DEBUG("CMIDTextEditor::SetCaretXYL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetFocusStateL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetFocusStateL(TBool aFocusState)
{
    DEBUG_INT("CMIDTextEditor::SetFocusStateL +, aFocusState=%d",
              aFocusState);

    if ((aFocusState) && !(iFocusState))
    {
        // Set focus if the text editor is not focused
        iFocusState = ETrue;

        // Send the information about new focused component to parent
        iComponentContainer->SetFocusedComponent(this);

        // If the editor is visible, give him focus.
        if (iTextEdwin->IsVisible())
        {
            DEBUG("CMIDTextEditor::SetFocusStateL, visible - iTextEdwin->setFocus(true)");
            // Call SetFocus on edwin
            iTextEdwin->SetFocus(ETrue);
            // Set focus automatically selects the editor's content
            // Remove the selection at this point. The selection can be
            // set again with the API.
            SetCursorPositionL(iTextEdwin->CursorPos());
            
            if (iEditingStateIndicator->EnabledState() ==
                             CMIDEditingStateIndicator::EIndicatorStateRelative)
            {
                // Enable the custom indicators as in Avkon if not controlled
                // by the client application
                iEditingStateIndicator->MakeVisible(ETrue);
            }
            iTextEdwin->Redraw();
        }
    }
    else if (!(aFocusState) && (iFocusState))
    {
        // Remove focus if the text editor is focused.
        iFocusState = EFalse;

#ifdef RD_JAVA_S60_RELEASE_9_2
        iTextEdwin->FocusLost();
#endif // RD_JAVA_S60_RELEASE_9_2

        // Call SetFocus on edwin
        iTextEdwin->SetFocus(EFalse);

        if (iComponentContainer)
        {
            // Send the information about new focused component to parent
            iComponentContainer->SetFocusedComponent(NULL);
        }

        // Disable the custom indicators as in Avkon if not controlled by
        // the client application.
        if (iEditingStateIndicator->EnabledState() ==
                CMIDEditingStateIndicator::EIndicatorStateRelative)
        {
            iEditingStateIndicator->MakeVisible(EFalse);
        }
        iTextEdwin->Redraw();
    }

    DEBUG("CMIDTextEditor::SetFocusStateL -");
}

TBool CMIDTextEditor::GetFocusState()
{
    return iFocusState;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ContentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
HBufC* CMIDTextEditor::ContentL()
{
    DEBUG("CMIDTextEditor::ContentL +");

    HBufC* content = iTextEdwin->GetTextInHBufL();

    if (!content)
    {
        // Return an empty content if the current content of the editor
        // is NULL.
        content = HBufC::NewL(0);

        DEBUG("CMIDTextEditor::ContentL -, returning empty descriptor");

        return content;
    }

    TPtr ptr = content->Des();

    // Otherwise, convert content to Java specific format.
    // First, if the constraints indicates that this is a decimal field
    // convert the string from localized decimal.
    if (iTextEdwin->IsConstraintSet(MMIDTextField::EDecimal))
    {
        DEBUG("CMIDTextEditor::ContentL, convert from localized decimal");

        iEdwinUtils->ConvertFromLocalizedDecimal(content);
    }

    // Perform number conversion if it is needed.
    if (iEdwinUtils->IsNumberConversionNeeded(iConstraints))
    {
        DEBUG(
            "CMIDTextEditor::ContentL, language specific number conversion");

        AknTextUtils::ConvertDigitsTo(ptr, EDigitTypeWestern);
    }

    // Map Java-specific characters in the buffer.
    CMIDUtils::MapETextToJavaChars(content);

    DEBUG_STR("CMIDTextEditor::ContentL - content=%S", *content);

    return content;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetContentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetContentL(const TDesC& aContent)
{
    DEBUG("CMIDTextEditor::SetContentL +");

    // Leave if text is not valid for the current set of constraints.
    // Note that if the constraints are set to phone number invalid
    // characters are simply removed. See CMIDEdwin.
    if (!(iTextEdwin->IsConstraintSet(MMIDTextField::EPhoneNumber)) &&
            !(iEdwinUtils->ConstraintsValidForText(
                  aContent,
                  iConstraints, ETrue)))
    {
        User::Leave(KErrArgument);
    }

    // Get converted content (localized, number conversion and phone number
    // specific).
    HBufC* buf = ConvertedContentLC(aContent);

    // Set text to the editor window.
    iTextEdwin->SetTextL(buf);

    // Text buffer is not needed anymore, pop and destroy it.
    CleanupStack::PopAndDestroy(buf);

    // Text has been changed so inform the editor window that
    // it needs to redraw itself again.
    iTextEdwin->HandleTextChangedL();

    // Cursor state is not automatically updated. Do it here. False indicates
    // that the text is not selected from the point where the cursor is moved.
    SetCursorPositionL(ContentLength());

    DEBUG("CMIDTextEditor::SetContentL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::InsertContentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::InsertContentL(const TDesC& aContent, TInt aPosition)
{
    DEBUG_STR("CMIDTextEditor::InsertContentL +, aContent=%S", aContent);
    DEBUG_INT("CMIDTextEditor::InsertContentL, aPosition=%d", aPosition);

    // Validate the new content before making any conversions or
    // modifications.
    // Leave if text is not valid for the current set of constraints.
    // Note that if the constraints are set to phone number invalid
    // characters are simply removed. See CMIDEdwin.
    if (!(iTextEdwin->IsConstraintSet(MMIDTextField::EPhoneNumber)) &&
            !(iEdwinUtils->ConstraintsValidForText(
                  aContent,
                  iConstraints, ETrue)))
    {
        User::Leave(KErrArgument);
    }

    TInt contentLength = ContentLength();

    // Get cursor position internally because CursorPosition() checks
    // wether the editor window is initialized or not.
    TInt cursorPosition = CursorPosition();

    // Get language converted content.
    HBufC* originalContent = ContentL();
    CleanupStack::PushL(originalContent);

    // If position is negative, convert it to zero.
    if (aPosition < 0)
    {
        aPosition = 0;
    }
    // Or if the position is greater than the length.
    else if (aPosition > contentLength)
    {
        aPosition = contentLength;
    }

    DEBUG_INT("CMIDTextEditor::InsertContentL, aPosition=%d",
              aPosition);
    DEBUG_INT("CMIDTextEditor::InsertContentL, cursorPosition=%d",
              cursorPosition);

    // Get converted content (localized, number conversion and phone number
    // specific).
    HBufC* content = ConvertedContentLC(aContent);

    // Leave with KErrArgument if the content is not valid.
    if (!iEdwinUtils->ConstraintsValidForInsertedTextL(
                *originalContent, *content, aPosition, iConstraints, ETrue))
    {
        DEBUG("CMIDTextEditor::InsertContentL -, text not valid");

        User::Leave(KErrArgument);
    }

    // Text can be now inserted.
    iTextEdwin->Text()->InsertL(aPosition, content->Des());

    // Notify editor that its content has been changed.
    iTextEdwin->HandleTextChangedL();

    // Update cursor position based on the inserted content.
    if (cursorPosition >= aPosition)
    {
        DEBUG("CMIDTextEditor::InsertContentL, adjusting cursor");

        // Cursor is set to the end of inserted text.
        cursorPosition += content->Length();
    }

    // else, cursor is on the left side of the inserted text.
    DEBUG_INT("CMIDTextEditor::InsertContentL, cursorPosition=%d",
              cursorPosition);

    SetCursorPositionL(cursorPosition);

    // New and original contents are not needed anymore.
    CleanupStack::PopAndDestroy(2, originalContent);

    DEBUG("CMIDTextEditor::InsertContentL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::DeleteContentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::DeleteContentL(TInt aOffset, TInt aLength)
{
    DEBUG_INT2("CMIDTextEditor::DeleteContentL +, aOffset=%d, aLength=%d",
               aOffset, aLength);

    TInt originalSize = ContentLength();

    // Get cursor position internally because CursorPosition() checks
    // wether the editor window is initialized or not.
    TInt cursorPosition = CursorPosition();

    // Store current content to local heap descriptor. The content
    // needs to be verified before actual deletion. Note that if
    // the constraints are not valid for the new text, the state of the
    // editor must not be changed by this method!!!
    HBufC* originalContent = HBufC::NewLC(originalSize);

    // Note that FEP transaction is also in this text content.
    // Note that this function cannot be totally leave safe because
    // the cursor must be replaced acording to the deletion.
    TPtr contentPtr = originalContent->Des();
    iTextEdwin->GetText(contentPtr);

    DEBUG_STR("CMIDTextEditor::DeleteContentL - old content=%S",
              contentPtr);

    // Perform deletion at this point. Editor is not changed yet.
    contentPtr.Delete(aOffset, aLength);

    DEBUG_STR("CMIDTextEditor::DeleteContentL - new content=%S",
              contentPtr);

    // Leave if the content is not valid for the current constraints.
    if (!iEdwinUtils->ConstraintsValidForText(
                contentPtr, iConstraints, ETrue))
    {
        DEBUG("CMIDTextEditor::DeleteContentL -, text not valid");

        User::Leave(KErrArgument);
    }

    // Constraints are valid. Perform with the modified text. Setting the text
    // cancels to ongoing FEP transaction. Selection is also canceled.
    iTextEdwin->SetTextL(originalContent);
    CleanupStack::PopAndDestroy(originalContent);

    // Notify editor window that the text has changed.
    iTextEdwin->HandleTextChangedL();

    // Modify cursor position. FEP transaction cannot be restored anymore.
    // Cursor was in the middle of the deleted range.
    if ((cursorPosition > aOffset) &&
            (cursorPosition < aOffset + aLength))
    {
        cursorPosition = aOffset;
    }
    // Cursor was on the right side of the deleted range.
    else if (cursorPosition >= aOffset + aLength)
    {
        cursorPosition =
            (cursorPosition - aLength < 0 ? 0 : cursorPosition - aLength);
    }

    // else, cursor was on the left side and does not need modification.
    SetCursorPositionL(cursorPosition);

    DEBUG("CMIDTextEditor::DeleteContentL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ContentLength
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::ContentLength() const
{
    DEBUG_INT("CMIDTextEditor::ContentLength, length=%d",
              iTextEdwin->TextLength());

    return iTextEdwin->TextLength();
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ContentHeight
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::ContentHeight() const
{
    DEBUG("CMIDTextEditor::ContentHeight +");

    // calculation of content height
    TInt lineHeight = iTextEdwin->LineHeight();
    TInt rowCount = iTextEdwin->NumberOfLines();
    TInt height = lineHeight * rowCount;

    // If parent is in fullscreen mode,
    // then content height has to be descaled.
    if (IsScalingOn())
    {
        height = iUtils->DoDescaling(height, CMIDUtils::EHorizontal);
    }

    DEBUG_INT("CMIDTextEditor::ContentHeight - , contentHeight=%d",
              height);

    return height;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::LineMarginHeight
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::LineMarginHeight() const
{
    DEBUG("CMIDTextEditor::LineMarginHeight +");

    // Currently line margin is equal to zero since content height is
    // line height * row count which gives the same height as the editor's
    // height is. There is no method for getting the line margin height
    // so at the moment it is considered to be zero.

    return 0;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::VisibleContentPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::VisibleContentPosition() const
{
    return iTextEdwin->VisibleContentPosition();
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetSelectionL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetSelectionL(TInt aIndex, TInt aLength)
{
    DEBUG_INT2("CMIDTextEditor::SetSelectionL +, aIndex=%d, aLength=%d",
               aIndex, aLength);

    TInt endIndex = aIndex + aLength;

    __ASSERT_DEBUG((aIndex >= 0 && aIndex <= ContentLength()) &&
                   (aLength >= 0 && endIndex <= ContentLength()),
                   User::Invariant());

    // Set cursor to the beginning of the new selection and set the new
    // position using the end index and selection. CEikEdwin::SetSelectionL
    // always sets the cursor to the beginning of the selection so it cannot
    // be used.
    iTextEdwin->SetCursorPosL(aIndex, EFalse);
    iTextEdwin->SetCursorPosL(endIndex, ETrue);

    DEBUG("CMIDTextEditor::SetSelectionL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SelectionL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
HBufC* CMIDTextEditor::SelectionL()
{
    DEBUG("CMIDTextEditor::SelectionL +");

    // Get the whole content from the editor. Content is already correctly
    // converted at this point when using ContentL() method.
    HBufC* content = ContentL();
    CleanupStack::PushL(content);

    // Get current selection and remove all other contents from the
    // descriptor.
    TCursorSelection selection = iTextEdwin->Selection();
    TInt lowerPos = selection.LowerPos();
    TInt length = selection.Length();

    DEBUG_INT2("CMIDTextEditor::SelectionL, lower pos=%d, length=%d",
               lowerPos, length);

    HBufC* ret = HBufC::NewL(length);
    TPtr retPtr = ret->Des();

    // Get the selected text from the editor and append it to the new
    // descriptor.
    retPtr.Append(content->Mid(lowerPos, length));

    // Content is not needed anymore.
    CleanupStack::PopAndDestroy(content);

    DEBUG("CMIDTextEditor::SelectionL -");

    return ret;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetConstraintsL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetConstraintsL(TUint aConstraints)
{
    DEBUG_INT("CMIDTextEditor::SetConstraintsL +, aConstraints=%d",
              aConstraints);

    HBufC* content = ContentL();
    CleanupStack::PushL(content);

    TBool currentTextValid =
        iEdwinUtils->ConstraintsValidForText(
            content->Des(), aConstraints, ETrue);

    // Set the content to empty if the new constraints are not valid
    // for the current text.
    if (!currentTextValid)
    {
        iTextEdwin->SetTextL(&KNullDesC);
        iTextEdwin->SetCursorPosL(0, EFalse);
        iTextEdwin->HandleTextChangedL();
    }

    // Set constraints to the text editor window.
    iTextEdwin->SetConstraintsL(aConstraints);
    // Store constraints of this text editor.
    iConstraints = aConstraints;

    if (currentTextValid)
    {
        // Convert breaks if needed if constraints that do not allow line
        // breaks. Note that numeric, decimal and phonenumber do not allow
        // line breaks and are checked before this so no need to check here
        // again.
        if (iConstraints & MMIDTextField::EPassword  ||
                iTextEdwin->IsConstraintSet(MMIDTextField::EUrl) ||
                iTextEdwin->IsConstraintSet(MMIDTextField::EMailAddr))
        {
            TPtr ptr = content->Des();
            iEdwinUtils->CropToSingleLine(ptr);
        }

        // Set the content to the editor as it was cleared during
        // SetConstraintsL method.
        SetContentL(*content);
    }

    CleanupStack::PopAndDestroy(content);

    DEBUG("CMIDTextEditor::SetConstraintsL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetMultilineL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetMultilineL(TBool aMultiline)
{
    DEBUG_INT("CMIDTextEditor::SetMultilineL +, aMultiline=%d", aMultiline);

    // Do nothing if the editor state is equal to the requested state or
    // if the constraint modifier PASSWORD is set. Passwords are single
    // line editors due to CMIDEdwinUtils::CPasswordText implementation.
    if (aMultiline == iTextEdwin->IsWrapEnabled() ||
            iConstraints & MMIDTextField::EPassword)
    {
        DEBUG("CMIDTextEditor::SetMultilineL -, ignoring request");

        return;
    }

    // Content needs to be cropped to single line.
    if (!aMultiline)
    {
        HBufC* content = ContentL();
        CleanupStack::PushL(content);

        // Crop line and paragraph breaks.
        TPtr ptr = content->Des();
        iEdwinUtils->CropToSingleLine(ptr);

        // Set the cropped content to the editor.
        SetContentL(*content);
        CleanupStack::PopAndDestroy(content);
    }

    // Apply wrap on.
    iTextEdwin->SetWordWrapL(aMultiline);
    // Text has been changed so inform the editor window that
    // it needs to redraw itself again.
    iTextEdwin->HandleTextChangedL();
    // Cursor position handling is done in CMIDTextEditorEdwin
    iTextEdwin->SetCursorPosL(iTextEdwin->CursorPos(), EFalse);

    DEBUG("CMIDTextEditor::SetMultilineL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::IsMultiline
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditor::IsMultiline()
{
    DEBUG("CMIDTextEditor::IsMultiline +");
    TBool multiline = iTextEdwin->IsWrapEnabled();
    DEBUG("CMIDTextEditor::SetMultilineL -");
    return multiline;
}


// ---------------------------------------------------------------------------
// CMIDTextEditor::SetInitialInputModeL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetInitialInputModeL(const TDesC& aCharacterSubset)
{
    DEBUG("CMIDTextEditor::SetInitialInputModeL +");

    // Edwin handles the initial input mode change.
    iTextEdwin->SetInitialInputModeL(aCharacterSubset);

    DEBUG("CMIDTextEditor::SetInitialInputModeL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetMaxSizeL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::SetMaxSizeL(TInt aMaxSize)
{
    DEBUG_INT("CMIDTextEditor::SetMaxSizeL +, aMaxSize=%d", aMaxSize);

    // Maximum size cannot be zero or less.
    __ASSERT_DEBUG(aMaxSize > 0, User::Invariant());

    TInt contentLength = ContentLength();
    // Truncat from the end if the new maximum size is too small.

    if (contentLength > aMaxSize)
    {
        DeleteContentL(aMaxSize, contentLength - aMaxSize);
    }

    // Store text limit to the editor.
    iTextEdwin->SetTextLimit(aMaxSize);

    // Set upper full format length. This prevents the layout
    // from dropping lines from formatting and causing problems
    // when calculating line height and number of lines in the editor.
    iTextEdwin->SetUpperFullFormattingLength(aMaxSize);

    DEBUG("CMIDTextEditor::SetMaxSizeL -");

    return aMaxSize;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetCursorPositionL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetCursorPositionL(TInt aIndex)
{
    DEBUG_INT("CMIDTextEditor::SetCursorPositionL +, aIndex=%d", aIndex);

    // Cursor must be set to a valid position within the content.
    __ASSERT_DEBUG(aIndex >= 0 && aIndex <= ContentLength(),
                   User::Invariant());

    // Cursor position handling is done in CMIDTextEditorEdwin
    iTextEdwin->SetCursorPosL(aIndex, EFalse);

    DEBUG("CMIDTextEditor::SetCursorPositionL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CursorPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::CursorPosition() const
{
    DEBUG("CMIDTextEditor::CursorPosition");

    // Cursor position handling is done in CMIDTextEditorEdwin
    return iTextEdwin->CursorPos();
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetColorL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetColorL(const TRgb& aColor, TColorType aColorType)
{
    DEBUG_INT("CMIDTextEditor::SetColorL + aColorType=%d", aColorType);

    __ASSERT_DEBUG(
        aColorType == MMIDTextEditor::EColorBackground ||
        aColorType == MMIDTextEditor::EColorForeground ||
        aColorType == MMIDTextEditor::EColorHighlightBackground ||
        aColorType == MMIDTextEditor::EColorHighlightForeground,
        User::Invariant());

    if (aColorType == MMIDTextEditor::EColorForeground)
    {
        // Apply new character format for the editor.
        // Font color is handled internally in the editor window.
        TCharFormat charFormat;
        TCharFormatMask charMask;
        iTextEdwin->OverrideColorL(EColorControlText, aColor);

        charMask.SetAttrib(EAttColor);
        charFormat.iFontPresentation.iTextColor = aColor;

        // Apply character formats.
        CGlobalText* text = static_cast< CGlobalText* >(iTextEdwin->Text());
        text->ApplyCharFormatL(charFormat, charMask, 0, ContentLength());
    }
    else if (aColorType == MMIDTextEditor::EColorHighlightBackground)
    {
        // Highlight color is handled in the custom drawer.
        // Just override the default color of highlight background
        // and let the custom drawer to do the work.
        iTextEdwin->OverrideColorL(
            EColorControlHighlightBackground, aColor);
    }
    else if (aColorType == MMIDTextEditor::EColorHighlightForeground)
    {
        // Highlight color is handled in the custom drawer.
        // Just override the default color of highlight background
        // and let the custom drawer to do the work.
        iTextEdwin->OverrideColorL(EColorControlHighlightText, aColor);
    }
    else if (aColorType == MMIDTextEditor::EColorBackground)
    {
        // Background color is handled internally in the Text Editor edwin.
        // Just override the default color of the control's background
        // and let the editor do the work. Background color alpha channel
        // does not work correctly with paragraph format so it is not used
        // here.
        iTextEdwin->OverrideColorL(EColorControlBackground, aColor);

        // Apply paraformat if the color is not transparent.
        if (!iTextEdwin->IsTransparent())
        {
            CParaFormat* paraFormat = new(ELeave) CParaFormat();
            CleanupStack::PushL(paraFormat);
            paraFormat->iFillColor = aColor;

            TParaFormatMask paraMask;
            paraMask.SetAttrib(EAttFillColor);

            static_cast< CGlobalText* >(
                iTextEdwin->Text())->ApplyParaFormatL(
                    paraFormat,
                    paraMask,
                    0,
                    iTextEdwin->TextLength());

            CleanupStack::PopAndDestroy(paraFormat);
        }

        // Apply same color to the custom indicator.
        iEditingStateIndicator->OverrideColorL(
            EColorControlBackground, aColor);
    }

    // OverrideColorL does not apply colors yet.
    // Redraw the control in order to get the colors in use.
    iTextEdwin->Redraw();

    DEBUG("CMIDTextEditor::SetColorL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::GetColorL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::GetColor(TColorType aColorType)
{
    DEBUG_INT("CMIDTextEditor::GetColorL + aColorType=%d", aColorType);

    __ASSERT_DEBUG(
        aColorType == MMIDTextEditor::EColorBackground ||
        aColorType == MMIDTextEditor::EColorForeground,
        User::Invariant());

    TRgb color;
    if (aColorType == MMIDTextEditor::EColorForeground)
    {
        iTextEdwin->GetColor(EColorControlText, color);
    }
    else if (aColorType == MMIDTextEditor::EColorBackground)
    {
        iTextEdwin->GetColor(EColorControlBackground, color);
    }

    TInt red = color.Red();
    TInt green = color.Green();
    TInt blue = color.Blue();
    TInt alpha = color.Alpha();
    TInt returnColor = (alpha<<24)|(red<<16)|(green<<8)|blue;

    DEBUG("CMIDTextEditor::GetColorL -");
    return returnColor;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetFontL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetFontL(MMIDFont* aFont)
{
    DEBUG("CMIDTextEditor::SetFontL +");

    TFontSpec fontspec = (aFont->Font(ETrue))->FontSpecInTwips();
    // If scaling is on, this code does scaling of size of font.
    if (IsScalingOn())
    {
        fontspec.iHeight=iUtils->DoScaling(fontspec.iHeight, CMIDUtils::EVertical);
    }

    TCharFormat charFormat(fontspec.iTypeface.iName, fontspec.iHeight);
    TCharFormatMask charFormatMask;

    // For some reason TCharFormat does not get all the parameters from
    // TFontSpec, so check and fill those (underline/bold/italic) manually.
    charFormat.iFontPresentation.iUnderline = EUnderlineOff;

    if (aFont->IsUnderlined())
    {
        charFormat.iFontPresentation.iUnderline = EUnderlineOn;
    }

    charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);

    if (fontspec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
    {
        charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    }

    charFormat.iFontSpec.iFontStyle.SetPosture(EPostureUpright);

    if (fontspec.iFontStyle.Posture() == EPostureItalic)
    {
        charFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
    }

    charFormatMask.SetAll();

    // Get current foreground font color and apply it to the new char
    // format. Highlight color is currently handled in the custom draw.
    TRgb color;

    if (iTextEdwin->GetColor(EColorControlText, color))
    {
        charFormat.iFontPresentation.iTextColor = color;
    }

    // Apply character formats.
    CGlobalText* text = static_cast< CGlobalText* >(iTextEdwin->Text());
    text->ApplyCharFormatL(charFormat, charFormatMask, 0, ContentLength());
    iTextEdwin->HandleTextChangedL();

    // Resize the editor window if editor was created with row count parameter
    // AND the SetEditorSize method has not been called directly yet.
    if (iRowCountActive)
    {
        TSize size = EditorSize();
        TInt newEditorWindowHeight = iTextEdwin->EditorWindowHeight();
        if (size.iHeight != newEditorWindowHeight)
        {
            SetEditorSize(size.iWidth, newEditorWindowHeight);
        }
        // SetEditorSize method resets the flag, make sure it remains true
        // here.
        iRowCountActive = ETrue;
    }

    // Move caret to its position along with the view
    SetCursorPositionL(iTextEdwin->CursorPos());

    // Store a non-scaled version of font for posibility of use it during
    // orientation and fullscreen mode change.
    iNonScaledFont = aFont;

    DEBUG("CMIDTextEditor::SetFontL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetPreferredTouchInputMode
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
#ifdef RD_TACTILE_FEEDBACK
void CMIDTextEditor::SetPreferredTouchInputMode(TInt aInputMode)
#else
void CMIDTextEditor::SetPreferredTouchInputMode(TInt /*aInputMode*/)
#endif // RD_TACTILE_FEEDBACK
{
    DEBUG("CMIDTextEditor::SetPreferredTouchInputMode +");

#ifdef RD_TACTILE_FEEDBACK
    if (isConnected())
    {
        DEBUG_INT(
            "CMIDTextEditor::SetPreferredTouchInputMode, aInputMode=%d",
            aInputMode);

        // Casting is ok since values map directly to TextEditor's
        // S60 extension.
        iPenServer.SetPreferredUiMode(
            static_cast< TPluginInputMode >(aInputMode));
    }
#endif // RD_TACTILE_FEEDBACK

    DEBUG("CMIDTextEditor::SetPreferredTouchInputMode -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::PreferredTouchInputMode
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::PreferredTouchInputMode()
{
    DEBUG("CMIDTextEditor::PreferredTouchInputMode +");

    // By default, there are not input modes preferred.
    TInt preferredMode = 0;

#ifdef RD_TACTILE_FEEDBACK
    if (isConnected())
    {
        DEBUG("CMIDTextEditor::PreferredTouchInputMode, pen enabled");

        preferredMode = iPenServer.PreferredUiMode();
    }
#endif // RD_TACTILE_FEEDBACK

    DEBUG_INT("CMIDTextEditor::PreferredTouchInputMode -, preferredMode=%d",
              preferredMode);

    return preferredMode;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetDisabledTouchInputModes
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
#ifdef RD_TACTILE_FEEDBACK
void CMIDTextEditor::SetDisabledTouchInputModes(TInt aInputModes)
#else
void CMIDTextEditor::SetDisabledTouchInputModes(TInt /*aInputModes*/)
#endif // RD_TACTILE_FEEDBACK
{
    DEBUG("CMIDTextEditor::SetDisabledTouchInputModes +");

#ifdef RD_TACTILE_FEEDBACK
    if (isConnected())
    {
        DEBUG_INT(
            "CMIDTextEditor::SetDisabledTouchInputModes, aInputModes=%d",
            aInputModes);

        iPenServer.SetDisabledLayout(aInputModes);
    }
#endif // RD_TACTILE_FEEDBACK

    DEBUG("CMIDTextEditor::SetDisabledTouchInputModes -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::DisabledTouchInputModes
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDTextEditor::DisabledTouchInputModes()
{
    DEBUG("CMIDTextEditor::DisabledTouchInputModes +");

    // By default, all input modes are disabled.
    TInt disabled = KAllTouchModesDisabled;

#ifdef RD_TACTILE_FEEDBACK
    if (isConnected())
    {
        DEBUG("CMIDTextEditor::DisabledTouchInputModes, pen enabled");

        disabled = iPenServer.DisabledLayout();
    }
#endif // RD_TACTILE_FEEDBACK

    DEBUG_INT("CMIDTextEditor::DisabledTouchInputModes -, disabled=%d",
              disabled);

    return disabled;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetIndicatorVisibilityL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetIndicatorVisibilityL(TBool aVisible)
{
    DEBUG("CMIDTextEditor::SetIndicatorVisibilityL +");

    // Do not set the indicator's visibility if the editor is not visible.
    if (iTextEdwin->IsVisible())
    {
        DEBUG("CMIDTextEditor::SetIndicatorVisibilityL, \
controlling visibility");

        iEditingStateIndicator->SetVisibleL(aVisible);

        // Notify about state change so the indicator gets updated.
        // Otherwise the indicator's state may not update correctly when
        // set as visible.
        if (aVisible)
        {
            iTextEdwin->NotifyEditorStateObserverOfStateChangeL();
        }
    }

    DEBUG("CMIDTextEditor::SetIndicatorVisibilityL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetIndicatorPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetIndicatorPosition(TInt aX, TInt aY)
{
    DEBUG("CMIDTextEditor::SetIndicatorPosition +");

    iEditingStateIndicator->SetPosition(aX, aY);
    // Custom position set, state set to absolute.

    iEditingStateIndicator->SetEnabledState(
        CMIDEditingStateIndicator::EIndicatorStateAbsolute);

    // Disable the current editing state indicator when custom indicator
    // position has been specified by the client application.
    MAknEditingStateIndicator* indicator =
        CAknEnv::Static()->EditingStateIndicator();

    if (indicator)
    {
        indicator->SetState(EStateNone);
    }

    DEBUG("CMIDTextEditor::SetIndicatorPosition -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::IndicatorSize
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TSize CMIDTextEditor::IndicatorSize() const
{
    // The default size is (0,0) if indicators are in the status pane.
    TSize size;

    if (iEditingStateIndicator->EnabledState() !=
            CMIDEditingStateIndicator::EIndicatorStateDisabled)
    {
        size = iEditingStateIndicator->Size();
    }

    return size;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::SetDefaultIndicatorsL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::SetDefaultIndicatorsL()
{
    DEBUG("CMIDTextEditor::SetDefaultIndicatorsL +");

    if (iComponentContainer)
    {
        if (!iComponentContainer->IsFullScreen())
        {
            iEditingStateIndicator->SetEnabledState(
                CMIDEditingStateIndicator::EIndicatorStateDisabled);
        }
        else
        {
            iEditingStateIndicator->SetEnabledState(
                CMIDEditingStateIndicator::EIndicatorStateRelative);
        }
    }
    // Update indicator location.
    UpdateIndicatorPosition();

    // Notify about state change so the indicator gets updated.
    // Otherwise the indicator's state may not update correctly when
    // set as visible.
    iTextEdwin->NotifyEditorStateObserverOfStateChangeL();

    DEBUG("CMIDTextEditor::SetDefaultIndicatorsL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::Dispose
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::Dispose()
{
    DEBUG("CMIDTextEditor::Dispose +");

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iFocusState)
    {
        iTextEdwin->CloseVKB();
    }
#endif // RD_JAVA_S60_RELEASE_9_2
    delete this;
    DEBUG("CMIDTextEditor::Dispose -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::MdcContainerWindowRectChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//

void CMIDTextEditor::MdcContainerWindowRectChanged(const TRect& /*aRect*/)
{
    DEBUG("CMIDTextEditor::MdcContainerWindowRectChanged +");

    // Not used at the moment.

    DEBUG("CMIDTextEditor::MdcContainerWindowRectChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::MdcContainerVisibilityChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::MdcContainerVisibilityChanged(TBool /*aVisible*/)
{
    DEBUG("CMIDTextEditor::MdcContainerVisibilityChanged +");

    // Not used at the moment.

    DEBUG("CMIDTextEditor::MdcContainerVisibilityChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::MdcContentBoundsChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::MdcContentBoundsChanged(const TRect& /*aRect*/)
{
    DEBUG("CMIDTextEditor::MdcContentBoundsChanged +");

    CMIDEditingStateIndicator::TIndicatorState state =
        iEditingStateIndicator->EnabledState();

    // Check if the container has been set to full screen. If the indicator
    // position is not controlled by the user, the state is enabled and set
    // to relative. The indicator is drawn relative to the editor window.
    // Otherwise, if using default indicators, disable custom indicators so
    // that standard navi pane indicators are shown.

    if (iComponentContainer && iComponentContainer->IsFullScreen() &&
            state == CMIDEditingStateIndicator::EIndicatorStateDisabled)
    {
        DEBUG("CMIDTextEditor::MdcContentBoundsChanged, \
setting indicators relative");

        // Enable indicators and update position as relative to the editor.
        iEditingStateIndicator->SetEnabledState(
            CMIDEditingStateIndicator::EIndicatorStateRelative);

        // Make the indicator visible if the editor is visible and focused.
        if (iTextEdwin->IsVisible() && iTextEdwin->IsFocused())
        {
            iEditingStateIndicator->MakeVisible(ETrue);
        }
    }
    else if (iComponentContainer && (!iComponentContainer->IsFullScreen()) &&
             (state == CMIDEditingStateIndicator::EIndicatorStateRelative))
    {
        DEBUG(
            "CMIDTextEditor::MdcContentBoundsChanged, disabling indicators");

        // Disable indicators when relative and back to normal screen.
        iEditingStateIndicator->SetEnabledState(
            CMIDEditingStateIndicator::EIndicatorStateDisabled);
    }

    // Update position.
    UpdateIndicatorPosition();

    // Notify editor about state change.
    TRAP_IGNORE(iTextEdwin->NotifyEditorStateObserverOfStateChangeL());

    DEBUG("CMIDTextEditor::MdcContentBoundsChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::MdcItemContentRectChanged
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::MdcItemContentRectChanged(
    const TRect& /*aContentRect*/,
    const TRect& /*aScreenRect*/)
{
    DEBUG("CMIDTextEditor::MdcItemContentRectChanged +");

    // Not used at the moment.

    DEBUG("CMIDTextEditor::MdcItemContentRectChanged -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::MdcContainerDestroyed
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::MdcContainerDestroyed()
{
    DEBUG("CMIDTextEditor::MdcContainerDestroyed +");

    iDirectContainer = NULL;

    DEBUG("CMIDTextEditor::MdcContainerDestroyed -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::HandleControlEventL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::HandleControlEventL(
    CCoeControl* aControl,
    TCoeEvent aEventType)
{
    if (iObserver && aControl == iEditingStateIndicator &&
            aEventType == MCoeControlObserver::EEventStateChanged)
    {
        iObserver->NotifyInputAction(
            MMIDTextEditorObserver::EActionInputModeChange);
    }
}


void CMIDTextEditor::MdcAbortDSA()
{
    DEBUG("CMIDTextEditor::MdcAbortDSA +");

    // Not used at the moment.

    DEBUG("CMIDTextEditor::MdcAbortDSA -");
}


void CMIDTextEditor::MdcResumeDSA()
{
    DEBUG("CMIDTextEditor::MdcResumeDSA +");

    // Not used at the moment.

    DEBUG("CMIDTextEditor::MdcResumeDSA -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ProcessPointerEventL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::ProcessPointerEventL(const TPointerEvent& aPointerEvent)
{
    TSize size = EditorSize();
    TPoint position = iTextEdwin->Position();
    TRect rect = iTextEdwin->Rect();
    if (rect.Contains(aPointerEvent.iPosition))
    {
        // Pointer event inside the editor
        iTextEdwin->HandlePointerEventL(aPointerEvent);
    }
    else if (aPointerEvent.iPosition.iY < position.iY)
    {
        // If editor is scrolled, send scroll event to midlet
        if (iTextEdwin->CursorPos() == 0)
        {
            if (iObserver)
            {
                iObserver->NotifyInputAction(
                    MMIDTextEditorObserver::EActionScrollUp);
            }
        }
        else
        {
            iTextEdwin->HandlePointerEventL(aPointerEvent);
        }
    }
    else if (aPointerEvent.iPosition.iY > (position.iY + size.iHeight))
    {
        // If editor is scrolled, send scroll event to midlet
        if (iTextEdwin->CursorPos() == ContentLength())
        {
            if (iObserver)
            {
                iObserver->NotifyInputAction(
                    MMIDTextEditorObserver::EActionScrollDown);
            }
        }
        else
        {
            iTextEdwin->HandlePointerEventL(aPointerEvent);
        }
    }
    else
    {
        // Pointer event on the left or right side of editor
        iTextEdwin->HandlePointerEventL(aPointerEvent);
    }
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ConvertedContentLC
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
HBufC* CMIDTextEditor::ConvertedContentLC(const TDesC& aContent)
{
    DEBUG("CMIDTextEditor::ConvertedContentLC +");

    HBufC* content = aContent.AllocLC();
    TPtr contentPtr = content->Des();

    // Convert breaks if needed if constraints that do not allow line breaks.
    // Note that numeric, decimal, url, email address and phonenumber do not
    // allow line breaks and are checked before this so no need to check here
    // again. Note that conversion must be made here instead of after
    // ConvertToLocalicedLC because it performs some line break
    // transformations also.
    if ((!iTextEdwin->IsWrapEnabled()) ||
            (iConstraints & MMIDTextField::EPassword))
    {
        iEdwinUtils->CropToSingleLine(contentPtr);
    }
    else
    {
        // Convert legacy breaks to unicode breaks.
        iEdwinUtils->ConvertToUnicodeBreaks(contentPtr);
    }

    // Convert to language specific localized string.
    HBufC* buf = iEdwinUtils->ConvertToLocalizedLC(*content, iConstraints);
    TPtr ptr = buf->Des();

    // Pop content.
    CleanupStack::Pop(buf);
    CleanupStack::PopAndDestroy(content);
    CleanupStack::PushL(buf);

    // Perform number conversion if it is needed.
    if (iEdwinUtils->IsNumberConversionNeeded(iConstraints))
    {
        DEBUG("CMIDTextEditor::ConvertedContentLC, \
language specific number conversion");

        AknTextUtils::LanguageSpecificNumberConversion(ptr);
    }

    // Remove other than phone number characters if this is only a phone
    // number text editor.
    if (iTextEdwin->IsConstraintSet(MMIDTextField::EPhoneNumber))
    {
        DEBUG("CMIDTextEditor::ConvertedContentLC, \
remove non phone number chars");

        iEdwinUtils->RemoveNonPhoneNumberChars(buf);
    }

    DEBUG("CMIDTextEditor::ConvertedContentLC -");

    return buf;
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::UpdateIndicatorPosition
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::UpdateIndicatorPosition()
{
    DEBUG("CMIDTextEditor::UpdateIndicatorPosition +");

    CMIDEditingStateIndicator::TIndicatorState state =
        iEditingStateIndicator->EnabledState();

    if (state == CMIDEditingStateIndicator::EIndicatorStateRelative)
    {
        TSize indicatorSize = iEditingStateIndicator->Size();
        TSize editorSize = iTextEdwin->Size();
        TPoint editorPos = iTextEdwin->Position();

        // Adjust position according to the editor's size
        TInt x = editorPos.iX + editorSize.iWidth - indicatorSize.iWidth;
        TInt y = editorPos.iY - indicatorSize.iHeight;

        DEBUG_INT2(
            "CMIDTextEditor::UpdateIndicatorPosition, indicatorPos.X=%d, \
                indicatorPos.Y=%d", x, y);

        iEditingStateIndicator->SetPosition(x, y);

        // Update the indicator.
        if (iTextEdwin->IsVisible() && iTextEdwin->IsFocused())
        {
            if (iComponentContainer &&
                    iComponentContainer->IsFullScreen())
            {
                iEditingStateIndicator->MakeVisible(ETrue);
            }
            iEditingStateIndicator->Redraw();
        }
        else
        {
            iEditingStateIndicator->MakeVisible(EFalse);
        }
    }

    DEBUG("CMIDTextEditor::UpdateIndicatorPosition -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CMIDTextEditor
// (other items were commented in a header
// ---------------------------------------------------------------------------
//
TBool CMIDTextEditor::isConnected()
{
#ifdef RD_TACTILE_FEEDBACK
    if (!iPenInputServerConnected && AknLayoutUtils::PenEnabled())
    {
        TInt err = iPenServer.Connect();
        iPenInputServerConnected = (err == KErrNone);
    }
    return iPenInputServerConnected;
#else
    return EFalse;
#endif // RD_TACTILE_FEEDBACK

}

// ---------------------------------------------------------------------------
// CMIDTextEditor::CMIDTextEditor
// ---------------------------------------------------------------------------
//
CMIDTextEditor::CMIDTextEditor() : iNonScaledFont(NULL)
{
    // No implementation.
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::ConstructL
// (other items were commented in a header
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::ConstructL(const TCtorParams& aParams)
{
    DEBUG("CMIDTextEditor::ConstructL +");
    // Set default focus
    iFocusState = EFalse;

    // Create editor window utils for verifying text input.
    // Locale is used to get the decimal separator.
    TLocale locale;
    iEdwinUtils = CMIDEdwinUtils::NewL(NULL, locale.DecimalSeparator());

    // Create custom editing state indicator.
    iEditingStateIndicator = CMIDEditingStateIndicator::NewL();

    // Set the size of the indicator container.
    iEditingStateIndicator->SetSize(aParams.iWidth, -1);
    iEditingStateIndicator->SetObserver(this);

    // Create text editor component.
    iTextEdwin = new(ELeave) CMIDTextEditorEdwin(*iEdwinUtils);

    // Set custom editing state indicator.
    iTextEdwin->SetEditingStateIndicator(iEditingStateIndicator);

    // Editor width is measured in pixels and the editor must be resizable.
    TInt flags =
        CEikEdwin::EWidthInPixels |
        CEikEdwin::EResizable |
        CEikEdwin::EInclusiveSizeFixed;

    if (aParams.iHeightInRows)
    {
        // Height is specified using rows.
        iTextEdwin->ConstructL(flags, aParams.iWidth,
                               aParams.iMaxSize, aParams.iHeight);
        iRowCountActive = ETrue;
        iRowCount = aParams.iHeight;

        // Editor size is too wide when created with rows. Adjust the width
        // accordingly again after construction. Note that SetEditorSize()
        // must not be used since it makes the editor not to use row count.
        // CCoeControl::Size does not yet return valid values, so use
        // EditorWindowHeight() instead.
        iTextEdwin->SetSize(
            aParams.iWidth,
            iTextEdwin->EditorWindowHeight());
    }
    else
    {
        iTextEdwin->ConstructL(flags, aParams.iWidth, aParams.iMaxSize);

        // Set the size using width and heigth.
        SetEditorSize(aParams.iWidth, aParams.iHeight);
    }

    // Apply default colors.
    SetColorL(
        TRgb(
            KDefColorBgRed,
            KDefColorBgGreen,
            KDefColorBgBlue,
            KDefColorBgAlpha),
        MMIDTextEditor::EColorBackground);

    SetColorL(
        TRgb(
            KDefColorFgRed,
            KDefColorFgGreen,
            KDefColorFgBlue,
            KDefColorFgAlpha),
        MMIDTextEditor::EColorForeground);

    SetColorL(
        TRgb(
            KDefColorHlBgRed,
            KDefColorHlBgGreen,
            KDefColorHlBgBlue,
            KDefColorHlBgAlpha),
        MMIDTextEditor::EColorHighlightBackground);

    SetColorL(
        TRgb(
            KDefColorHlFgRed,
            KDefColorHlFgGreen,
            KDefColorHlFgBlue,
            KDefColorHlFgAlpha),
        MMIDTextEditor::EColorHighlightForeground);

    // TextEditor should defaultly receive pointer events when focused
    iTouchEnabled = ETrue;

#ifdef RD_TACTILE_FEEDBACK
    iPenInputServerConnected = EFalse;
#endif // RD_TACTILE_FEEDBACK

    iUtils = aParams.iUtils;

    DEBUG("CMIDTextEditor::ConstructL -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditor::HandleFullscreenModeChange
// (other items were commented in a header
// ---------------------------------------------------------------------------
//
void CMIDTextEditor::HandleFullscreenModeChange()
{
    if (iUtils && iUtils->IsScalingEnabled())
    {
        // We need reposition text editor.
        HandleChangeForScaling(EFullscreenChange);
    }
}

TBool CMIDTextEditor::IsScalingOn() const
{
    return iUtils && iComponentContainer && iUtils->IsScalingEnabled()
#ifdef RD_JAVA_S60_RELEASE_9_2
           && !iPartialVKBOpen
#endif // RD_JAVA_S60_RELEASE_9_2
           && iComponentContainer->IsFullScreen();
}

void CMIDTextEditor::HandleResourceChange(TInt aType)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    if ((aType == KAknSplitInputEnabled) ||
            (aType == KAknSplitInputDisabled))
    {
        iPartialVKBOpen = (aType == KAknSplitInputEnabled);
        if (iTextEdwin && iTextEdwin->IsFocused())
        {
            // Inform edwin about the event.
            iTextEdwin->HandleResourceChange(aType);
        }

        if (iUtils && iComponentContainer && iUtils->IsScalingEnabled() &&
                iComponentContainer->IsFullScreen())
        {
            // Reposition the text editor.
            HandleChangeForScaling(EPartialVKBChange);
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        if (iUtils && iComponentContainer && iUtils->IsScalingEnabled() &&
                iComponentContainer->IsFullScreen())
        {
            // Reposition the text editor.
            HandleChangeForScaling(EResolutionChange);
        }
    }
}

void CMIDTextEditor::HandleChangeForScaling(TChange aChange)
{
    // It is needed to store iRowCountActive, because SetEditorSize resets it.
    TBool rowCountActive = iRowCountActive;

    // Calling all functions which sets sizes and position of TextEditor.
    SetEditorSize(iNonScaledEditorSize.iWidth, iNonScaledEditorSize.iHeight);
    SetPosition(iNonScaledPosition.iX, iNonScaledPosition.iY);
    if (iNonScaledFont)
    {
        TRAPD(err, SetFontL(iNonScaledFont));
        if (err != KErrNone)
        {
            DEBUG_INT("CMIDTextEditor::HandleFullscreenModeChange - error %d", err);
        }
    }

    // restoring of iRowCountActive
    iRowCountActive = rowCountActive;

    if ((aChange == EFullscreenChange) || (aChange == EResolutionChange))
    {
        if (iUtils)
        {
            // It sets edwin variable necessary for correct clipping.
            iTextEdwin->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());

            // It sets indicator variable necessary for correct clipping.
            iEditingStateIndicator->SetCanvasRect(iUtils->GetOnScreenCanvasRect());
        }
        if (aChange == EFullscreenChange)
        {
            // If scaling is turn on or off, we need to inform edwin
            // and indicator about that.
            iTextEdwin->SetScaling(IsScalingOn());
            iEditingStateIndicator->SetScalingOn(IsScalingOn());
        }
    }

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aChange == EPartialVKBChange)
    {
        if (iPartialVKBOpen)
        {
            // When partial keyboard is opening, the scalingmust be stoped.
            // Setting edwin's variables, it is necessary for correct clipping.
            iTextEdwin->SetOnScreenCanvasRect(iComponentContainer->Control().Rect());
            iTextEdwin->SetScaling(EFalse);

            // Setting indicator's variables, it is necessary for correct clipping.
            iEditingStateIndicator->SetScalingOn(EFalse);
            iEditingStateIndicator->SetCanvasRect(iComponentContainer->Control().Rect());
        }
        else if (iUtils)
        {
            // When partial keybord is closing, the scaling needs to be restored.
            // Setting edwin's variables, it is necessary for correct clipping.
            iTextEdwin->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());
            iTextEdwin->SetScaling(IsScalingOn());

            // Setting indicator's variables, it is necessary for correct clipping.
            iEditingStateIndicator->SetScalingOn(IsScalingOn());
            iEditingStateIndicator->SetCanvasRect(iUtils->GetOnScreenCanvasRect());
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

// End of file
