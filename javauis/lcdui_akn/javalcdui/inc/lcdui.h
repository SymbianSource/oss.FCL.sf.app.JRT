/*
* Copyright (c) 1999 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef LCDUI_H
#define LCDUI_H


/**
 * @file
 * @publishedPartner
 * @released
 *
 * This header defines the interfaces that must be implemented for LCDUI.
 * It should be read in conjunction with the MIDP 2.0 specification which
 * will contain more information about the enumerations, for example.
 *
 * The MMIDComponentFactory interface, defined at the end of this file, is
 * the logical starting point to read this file.
 *
 * Most methods can be assumed to be invoked in the "server thread" where a
 * UI environment and its active scheduler will be present. Any method
 * marked as "client side" may also be invoked from a thread that does not have
 * a UI environment or activescheduler. These methods should ensure any shared
 * data strucutures that could be accessed by both the server thread and client
 * thread(s) are protected by suitable locking or atomic update strategies.
 *
 * "Framework" refers to the underlying Java Runtime Environment which invokes
 * the methods defined in these interfaces at the appropriate time and which the
 * the implementor of these interfaces need have no knowledge of.
 * "Implementation" refers to the implementation of these interfaces in the client DLL.
 */

#include <e32def.h>
#include <badesca.h>
#include <gdi.h>
#include <w32std.h>

#ifdef RD_JAVA_NGA_ENABLED
#include <EGL/egltypes.h>
#include "monitor.h"
#endif // RD_JAVA_NGA_ENABLED

class MMIDDisplayable;
class MMIDAlert;
class MMIDForm;
class MMIDList;
class MMIDTextBox;

class MMIDItem;
class MMIDImageItem;
class MMIDStringItem;
class MMIDDateField;
class MMIDGauge;
class MMIDTextField;
class MMIDChoiceGroup;

class MMIDTicker;
class MMIDCommand;

class MMIDUtils;
class MMIDEnv;
class MMIDBufferProcessor;
class MMIDFont;
class MMIDImage;

class CFont;
class CCoeControl;

class MMIDToLcduiObserver;
class MDirectContainer;
class MUiEventConsumer;

class MMIDTextEditor;
class MMIDCanvasGraphicsItem;
class MMIDCanvasGraphicsItemPainter;

/**
 * @internalComponent
 */
typedef TAny* TJavaPeer;



/**
 * Namespaces including constant definitions for MIDlet suite attribute names and attributes values.
 */
namespace LcduiMidletAttributes
{
_LIT(KAttribMIDletName,                      "MIDlet-Name");
_LIT(KAttribMIDletVersion,                   "MIDlet-Version");
_LIT(KAttribMIDletVendor,                    "MIDlet-Vendor");
_LIT(KAttribUIEnhancement,                   "Nokia-UI-Enhancement");
_LIT(KAttribOrigDisplaySize,                 "Nokia-MIDlet-Original-Display-Size");
_LIT(KAttribTargetDisplaySize,               "Nokia-MIDlet-Target-Display-Size");
_LIT(KAttribNoExitKey,                       "Nokia-MIDlet-No-Exit");
_LIT(KAttribFlipClose,                       "Nokia-MIDlet-Flip-Close");
_LIT(KAttribBackgroundEvent,                 "Nokia-MIDlet-Background-Event");
_LIT(KAttribAllowScalingOnOrientationSwitch, "Nokia-MIDlet-Canvas-Scaling-Orientation-Switch");
_LIT(KAttribS60SelectionKeyCompatibility,    "Nokia-MIDlet-S60-Selection-Key-Compatibility");
_LIT(KAttribAppOrientation,                  "Nokia-MIDlet-App-Orientation");
_LIT(KAttribPointerEventSuppressorValues,    "Nokia-MIDlet-Tap-Detection-Options");
_LIT(KAttribOskSoftkeysInQwerty,             "Nokia-MIDlet-On-Screen-Softkeys-Position");
}

namespace LcduiMidletAttributeValues
{
_LIT(KUIEnhMediaKeys,          "musickeyssupported");
_LIT(KUIEnhCanvasBackground,   "canvashasbackground");
_LIT(KUIEnhPopUpTextBox,       "popuptextbox");
_LIT(KUIEnhFullScreenTextBox,  "fullscreentextbox");
_LIT(KTrueValue,               "true");
_LIT(KPauseValue,              "pause");
_LIT(KRunValue,                "run");
_LIT(KPortraitValue,           "portrait");
_LIT(KLandscapeValue,          "landscape");
_LIT(KPositionBottom,          "bottom");
_LIT(KPositionRight,           "right");
}



/**
 *
 * Base class of all LCDUI objects. Concrete classes always implement a
 * subclass of this interface.
 *
 */
class MMIDComponent
{
    friend class CMIDToolkit;
public:
    /**
     * Enumeration of the component type
     */
    enum TType
    {
        EUnknown,
        EImageItem,
        EStringItem,
        EGauge,
        ETextField,
        EChoiceGroup,
        EDateField,
        ECustomItem,
        ESpacer,
        ECanvas,
        EGameCanvas,
        EGraphics,
        EAlert,
        EForm,
        EList,
        ETextBox,
        ETicker,
        EFont,
        ECmd,
        EDisplayableContainer,
        EImage,
        ESprite,
        ETiledLayer,
        EDefaultBackground,
        ECanvasTextEditor,
        ECanvasGraphicsItem,
        ECanvasGraphicsItemPainter,
        ENumTypes   // Add types before ENumTypes
    };

public:
    /**
     * Invoked by the framework to clean up any  resources used by the
     * implementation. Typically, the implementation will do "delete this"
     * but is clearly not restricted to this.
     */
    virtual void Dispose() = 0;

    /**
     * This is an aid to the implementation - it is not used by the framework.
     * Not particularly good O-O, but the example implementation makes
     * considerable use of this method to good simplification.
     */
    virtual TType Type() const
    {
        return EUnknown;
    }

    /**
     * Returns buffer processor associated with this component (if any).
     */
    virtual MMIDBufferProcessor* Processor()
    {
        return NULL;
    }

private:
    /**
     * Inaccessible to the implementation, used by the framework for keeping
     * track of the associated Java object.
     */
    TJavaPeer iPeer;
};

#ifdef RD_JAVA_NGA_ENABLED
/**
 * NGA tension to the Component interface
 * @since S60 9.2
 */
class MMIDComponentNgaExtension : public MMIDComponent
{
public:

    /**
     * Update EGL content. Called by M3G to request LCDUI canvas to update
     * EGL surface with 2D content.
     * @since S60 9.2
     */
    virtual void UpdateEglContent() = 0;

    /**
     * Checks if EGL is available.
     * @return <code>ETrue</code> if EGL exists.
     * @since S60 9.2
     */
    virtual TBool IsEglAvailable() = 0;

    /**
     * Gets and binds EGL surface.
     * @return surface handle. <code>EGL_NO_SURFACE</code> is
     * returned if binding is not possible.
     * @since S60 9.2
     * @see MMIDComponentNga#ReleaseEglSurface
     */
    virtual EGLSurface BindEglSurface() = 0;

    /**
     * Release EGL surface
     * @since S60 9.2
     */
    virtual void ReleaseEglSurface() = 0;

    /**
     * Update offscreen bitmap i.e. canvas frame buffer
     * @param aForced   If true, off-screen bitmap is updated without
     *                  checking Canvas type (Canvas/GameCanvas)
     * @since S60 9.2
     */
    virtual void UpdateOffScreenBitmapL(TBool aForced) = 0;

    /**
     * Fills EGL surface
     * @param aRect The area to be filled
     * @param aColor The fill color
     * @since S60 9.2
     */
    virtual TBool FillEglSurface(const TRect& aRect, const TRgb& aColor) = 0;

};
#endif // RD_JAVA_NGA_ENABLED

/**
 *
 * Interface to Command
 *
 */
class MMIDCommand : public MMIDComponent
{
public:
    /**
     * This is the type passed into the plugin factory CreateCommand method
     */
    enum TCommandType
    {
        EScreen=1,
        EBack,
        ECancel,
        EOk,
        EHelp,
        EStop,
        EExit,
        EItem
    };

    /**
     * Native command peer identifiers.
     */
    enum TCommandID
    {
        ENullCommand=0,             // null
        EAlertDismissCommand=1,     // Alert.DISMISS_COMMAND
        EListSelectCommand=2,       // List.SELECT_COMMAND
        EUserCommandBase=4          // Base id for application commands.
    };
public:
    /**
     * Returns the human readable displayable text associated with the command.
     * @return  A descriptor containing the text.
     */
    virtual const TDesC& Label() const = 0;
    virtual TCommandType CommandType() const = 0;
    virtual TInt Priority() const = 0;
    virtual TInt Id() const = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ECmd;
    }
};

/**
 *
 * Base class of all Items.
 *
 * MMIDForm::RefreshItemL() is generally called following any method calls
 * which result in the item changing its appearance, e.g. size, label, layout or
 * content changed which may mean that the Item's min size has changed. Hence
 * the concrete implementation of Item does not need to refresh the Form's
 * appearance itself.
 *
 */
class MMIDItem : public MMIDComponent
{
public:
    /**
     * Used to describe the positioning of Item within the form.
     * Used by the form layout.
     */
    enum TLayout
    {
        EDefault = 0,
        ELeft,
        ERight,
        ECenter,
        //
        ETop = 0x10,
        EBottom = 0x20,
        EVerticalCenter = 0x30,
        //
        ENewLineBefore = 0x100,
        ENewLineAfter = 0x200,
        //
        EShrink = 0x400,
        EExpand = 0x800,
        EVerticalShrink = 0x1000,
        EVerticalExpand = 0x2000,
        ELayout2 = 0x4000
    };
    /**
     * Used only in StringItem & ImageItem
     */
    enum TAppearance
    {
        EPlain,     /** Item has normal appearance */
        EHyperLink, /** Item appears as a hyperlink */
        EButton     /** Item appears as a button */
    };
public:
    /**
     * Since the following methods are followed by MMIDForm::RefreshItemL(),
     * a reasonable implementation strategy could be to just cache the values
     * and calculate/redisplay on the refresh. These methods are invoked server
     * side, nonetheless, so the alternative strategy can be followed.
     */

    /**
     * Sets the Item's label.
     * @param   aLabel
     *          A descriptor containing the label text.
     */
    virtual void SetLabelL(const TDesC& aLabel) = 0;

    /**
     * Sets the Item's layout.
     * @param   aLayout
     *          A TLayout enumeration describing the desired layout.
     */
    virtual void SetLayoutL(TLayout aLayout) = 0;

    /**
     * Commands are always referred to by their index. This is their position in
     * an array where 0 is the first command added and (number of commands -1)
     * is the last added. Removing commands has the obvious re-numbering of commands
     * which follow in order. In effect, the implementation should store them in a
     * similar structure regardless of how they are ordered on the display.
     *
     * If the item is actually visible on the display (e.g. CBA button), and
     * this call affects the set of visible commands, the implementation
     * should update the display as soon as it is feasible to do so.
     */

    /**
     * Adds a command to the list at the specified index. If aIndex is zero,
     * the command is added at the head of the list.
     * @param   aIndex
     *          The position at which to insert the command.
     * @param   aCommand
     *          The command to add.
     */
    virtual void AddCommandL(MMIDCommand* aCommand) = 0;

    /**
     * Removes the command specified by the given index.
     * @param   aIndex
     *          The position from which to remove the command.
     */
    virtual void RemoveCommand(MMIDCommand* aCommand) = 0;

    /**
     * Sets the default command to be aIndex, where aIndex is the index of a command
     * which has already been added via AddCommandL(). If aIndex = -1, then
     * there should be no default command set.
     * @param   aIndex
     *          The index of the command to set.
     */
    virtual void SetDefaultCommand(MMIDCommand*) = 0;

    /**
     * aSize could have either/both iWidth and iHeight = -1.
     * This means that the width/height should be computed based
     * on the item's contents .
     * @param   aSize
     *          Structure representing the preferred size.
     */
    virtual void SetPreferredSizeL(const TSize& aSize) = 0;

    /*
     * Return the sizes as per MIDP 2.0 spec
     */

    /**
     * Returns the Item's preferred size.
     * @return  The preferred size.
     */
    virtual TSize PreferredSize() const = 0;

    /**
     * Returns the Item's minimum size.
     * @return  The minimum size.
     */
    virtual TSize MinimumSize() const = 0;
};

/**
 * Displayable container window created before the actual content (e.g. List/Canvas etc)
 * is called before CreateListL() in the factory).
 *
 * It is envisioned that the concrete implementation of MMIDDisplayable
 * is a container for the actual Displayable in which to hang commands, title bar
 * etc.
 *
 * Do not subclass from both MMIDDisplayable and one of the content types (e.g. MMIDList, MMIDForm)
 * in the same implementation class.
 *
 */
class MMIDDisplayable : public MMIDComponent
{
public:
    /**
     * Sets the Displayable's title.
     * @param   aTitle
     *          A descriptor containing the title text, may be NULL indicating
     *          that the displayable should hide the titlebar.
     */
    virtual void SetTitleL(const TDesC* aTitle) = 0;

    /**
     * Sets the Displayable's ticker, replacing any previous ticker.
     * The Ticker could be null, in which case no ticker is displayed.
     * @param   aTicker
     *          The ticker to associate with the Displayable.
     */
    virtual void SetTickerL(MMIDTicker* aTicker) = 0;

    /**
     * Returns the current ticker, or null if there is none set
     * @return  The ticker.
     */
    virtual MMIDTicker* Ticker() const = 0;

    /*
     * Commands are always referred to by their index. This is their position in
     * an array. The Commands will be inserted into the array in order of priority.
     * Removing commands has the obvious re-numbering of commands which follow in
     * order. In effect, the implementation should store them in a similar structure.
     *
     * If the item is actually visible on the display (e.g. CBA button), and
     * this call affects the set of visible commands, the implementation
     * should update the display as soon as it is feasible to do so.
     *
     * Adds a command to the list at the specified index. If aIndex is zero,
     * the command is added at the head of the list.
     * @param   aIndex
     *          The position at which to insert the command.
     * @param   aCommand
     *          The command to add.
     */
    virtual void AddCommandL(MMIDCommand* aCommand) = 0;

    /**
     * Removes the command specified by the given index.
     * @param   aIndex
     *          The position from which to remove the command.
     */
    virtual void RemoveCommand(MMIDCommand* aCommand) = 0;

    /**
     * Returns the size of the displayable content.
     * @return  The size of the displayable content.
     */
    virtual TSize ContentSize() const = 0;

    /**
     * Called when the receiving object has either just become or just
     * ceased to be the current Displayable. The receiving object is
     * expected to do the following:
     *
     * @param   aCurrent
     * If aCurrent is ETrue, the receiving object should allocate any
     * resources it needs to display its content and commands. It should
     * also enable event sources.
     * Enabling event sources may include adding a control to the control
     * stack, creating menus or toolbars to generate command events  and
     * making a control visible.
     * If this method leaves the framework will abort making the
     * receiving object current.
     * If aCurrent is EFalse, the receiving object should disable events
     * and make itself invisible. Leaves are ignored by the framework when
     * aCurrent is EFalse.
     * Regardless of the value of aCurrent, the receiving object is responsible
     * for returning itself to a consistent state should this method leave.
     *
     */
    virtual void HandleCurrentL(TBool aCurrent) = 0;

    /**
     * Set the component that is the displayable content.
     * The component here is the actual displayable which resides within this
     * container.
     * @param   aComponent
     *          Reference to the component to set.
     */
    virtual void SetComponentL(MMIDComponent& aComponent) = 0;

    /**
     * Get the component that is the displayable content.
     * The component here is the actual displayable which resides within this
     * container.
     * @return  A pointer to the returned component.
     */
    virtual MMIDComponent* Component() const = 0;

    /**
     * Controls whether the displayable should be in full-screen mode or in normal mode.
     * Only gets called for Canvases. Typically, the implementation would remove the
     * title bar in full screen mode.
     * @param   aFullScreen
     *          If ETrue, sets the Displayable to full screen mode, otherwise
     *          sets it to normal mode.
     */
    virtual void SetFullScreenModeL(TBool aFullScreen) = 0;

    /**
     * Controls whether the displayable has or has no CommandListener.
     * Only gets called for Canvases. Typically, the implementation sets a bool.
     * @param   aExistence
     *          If ETrue, The CommandListener is set, otherwise there is no
     *          CommandListener
     */
    virtual void SetCommandListenerExistence(TBool /*aExistence*/)
    {
        ;
    }

    /**
       * Gets a soft key label location and size for displayable, which does support it
       * @param   aSoftKeyId - a constant identifying the softkey
       * @param   aPosition - returned top left position of soft key label
       * @param aSize - returned size of soft ket label
       * @return  false if soft key label is not available, otherwise true
       * @since S60 5.0
       */
    virtual TBool SoftKeyLabelLocation(TInt aSoftKeyId, TPoint& aPosition, TSize& aSize) = 0;

    /**
     * Gets a soft key label anchor
     * @param   aSoftKeyId - a constant identifying the softkey
     * @return sk label anchor, an existence of sk can be checked by SoftKeyLabelLocation()
     * @since S60 5.0
     */
    virtual TInt SoftKeyLabelAnchor(TInt aSoftKeyId) = 0;

    /** Get a rect of a canvas without Keypad rect
     * @since S60 TB9.2
     */
    virtual TRect GetCanvasRectFromLaf() = 0;

    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EDisplayableContainer;
    }

    /**
     * @ DEPRECATED
     */
    virtual void SetHasCommandListener(TBool /*aHasCommandListener*/)
    {
        ;
    }
    /**
    * Returns boolean value indicating whether this displayable is pop-up TextBox.
    *
    * @return TBool
    *
    * @since S60 5.0
    */
    virtual TBool IsPopupTextBox() = 0;
    /*
     *@ DEPRECATED
    virtual CCoeControl& ContentWindow() = 0;
    */

    /**
     * Information about destroying old full screen Displayable.
     * @since Java 2.0
     */
    virtual void DisplayableBehindPopupIsDestroyed() = 0;

    /**
     * Force sync draw.
     */
    virtual void DrawNow() = 0;

    /**
       * Changes osk background state
       * @param   aOSKBackgroundState - osk background state
       */
    virtual void ChangeOSKBackgroundState(TBool aOSKBackgroundState) = 0;
};


/**
*
*
*   Interface for Tactile Feedback Component
*
*/
#ifdef RD_TACTILE_FEEDBACK
class MMIDTactileFeedbackComponent
{
public:
    virtual void UpdateTactileFeedback() = 0;
    virtual void RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle) = 0;
    virtual void UnregisterFeedbackArea(TInt aId) = 0;
    virtual void UnregisterFeedbackForControl() = 0;
    virtual void MoveAreaToFirstPriority(TInt aId) = 0;
};
#endif


/**
 * Interface to Canvas
 */
#ifdef RD_JAVA_NGA_ENABLED
class MMIDCanvas : public MMIDComponentNgaExtension
#else // !RD_JAVA_NGA_ENABLED
class MMIDCanvas : public MMIDComponent
#endif // RD_JAVA_NGA_ENABLED
{
public:
    /**
     * Specifies how the drawing of the canvas will take place.
     */
    enum TDrawOp
    {
        /**
         * Draw the entire canvas to the screen.
         * This command has no data.
         */
        EDrwOpcBitBlt = 0,

#ifdef RD_JAVA_NGA_ENABLED
        /**
         * Draw a rectangular region of the canvas to the screen. The rectangle to be drawn
         * is provided with the command as (x1,y1)-(x2,y2) inclusive-exclusive integer
         * coordinates following the command header.
         */
        EDrwOpcBitBltRect = 1,

        /**
         * Marks m3g content start point.
         * @since S60 9.2
         */
        EDrwOpcM3GContentStart = 2
#else // !RD_JAVA_NGA_ENABLED
        /**
         * Draw a rectangular region of the canvas to the screen. The rectangle to be drawn
         * is provided with the command as (x1,y1)-(x2,y2) inclusive-exclusive integer
         * coordinates following the command header.
         */
        EDrwOpcBitBltRect = 1
#endif // RD_JAVA_NGA_ENABLED
    };
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    virtual TType Type() const
    {
        return ECanvas;
    }

    /**
     * Returns a reference to the CCoeControl corresponding to this canvas.
     * @return  Reference to the CCoeControl corresponding to this canvas.
     */
    virtual CCoeControl&    Control() = 0;

    /**
     * Returns the frame buffer. Double buffering must be supported so this method
     * must not return NULL. There is no ownership transfer.
     * @return  A pointer to the frame buffer.
     */
    virtual CFbsBitmap* FrameBuffer() const = 0;

    /**
     * Returns the size of the canvas content.
     * @return  The size of the displayable content.
     */
    virtual TSize ContentSize() const = 0;

    /**
     * Direct screen access support.
     *
     * This method should be called to disable direct screen access when
     * a popup menu is about to be displayed. This is a workaround for
     * menu's failing to correctly draw their shadows on Techview and
     * other UI's that have floating menubars.
     */
    virtual void PauseDirectAccess() = 0;

    /**
     * Re-enables direct access. To be called when the menu has disappeared.
     */
    virtual void ResumeDirectAccess() = 0;

    /**
     * Switch off key event posting. Game keys will still be tracked, but
     * keyPressed()/keyReleased() callbacks will not be called.
     */
    virtual void    SuppressKeys() = 0;

    /**
     * Returns the latched state of the game action keys.
     * @return  The latched state of the game action keys.
     */
    virtual TUint32 GameActions() = 0;

public:
    /**
     *@return A container on which video can be rendered
     */
    virtual MDirectContainer& DirectContainer() = 0;

    /**
     * Draw a background image.
     * @param   aGc Target context
     * @param   aPosition Position
     * @param   aSize Size
     * @since S60 5.0
     */
    virtual void DrawBackground(CBitmapContext& aGc, const TPoint& aPosition, const TSize& aSize) = 0;

    /**
     * Returns the type of Canvas object
     * @return  <code>ETrue</code> if the type is <code>MMIDComponent::EGameCanvas</code> and
     *          <code>EFalse</code> when the type is <code>MMIDComponent::ECanvas</code>.
     * @since S60 5.0
     */
    virtual TBool IsGameCanvas() const = 0;

    /**
       * Gets a network indicator location and size on fullscreen Canvas
       * @param aPosition - returned top left position of nw indicator
       * @param aSize - returned size of nw indicator
       * @return false if Canvas is not able to display nw indicator (not fullscreen), otherwise true
       * @since S60 5.0
       */
    virtual TBool NetworkIndicatorLocation(TPoint& aPosition, TSize& aSize) const = 0;

#ifdef RD_TACTILE_FEEDBACK
    virtual  MMIDTactileFeedbackComponent* TactileFeedbackComponent() = 0;
#endif

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Notifies Canvas about areas that have been updated with 2D drawing.
     * Used for optimising texture upload to OpenGL when EGL surface is used.
     * @param aRect
     *      - the area that has been updated in canvas coordinates.
     * @since S60 9.2
     */
    virtual void UpdateRect(const TRect& aRect) = 0;
#endif // RD_JAVA_NGA_ENABLED
    
    virtual TBool ReadyToBlit() const = 0;
};

/**
 *
 * Interface to Alert
 *
 */
class MMIDAlert : public MMIDComponent
{
public:
    /**
     * Enumeration of the different Alert behaviours.
     */
    enum TAlertType
    {
        ENone = -1,
        EAlarm,
        EConfirmation,
        EError,
        EInfo,
        EWarning,
    };

    /**
     * Enumeration used by SetTimeoutL()/DefaultTimeout();
     * @see void SetTimeoutL(TInt aTime);
     * @see TInt DefaultTimeout();
     */
    enum
    {
        EForever = -2
    };
public:

    /**
     * Sets the Alert's text.
     * Framework does not call SetModalL if there are now scrollbars.
     * @param   aString
     *          A descriptor containing the text. May be empty, to display no text.
     */
    virtual void SetStringL(const TDesC& aString) = 0;

    /**
     * Sets the Alert's image.
     * Framework does not call SetModalL if there are now scrollbars
     * @param   aImage
     *          A pointer to the image. May be NULL, to display no image.
     */
    virtual void SetImageL(MMIDImage* aImage) = 0;

    /**
     * Sets the Alert's gauge.
     * Framework does not call SetModalL if there are now scrollbars
     * @param   aGauge
     *          A pointer to the gauge. May be NULL, to display no gauge.
     */
    virtual void SetIndicatorL(MMIDGauge* aGauge) = 0;

    /**
     * Set the behavioural type of the Alert.
     * @param   aType
     *          Enumeration of the desired behaviour type
     */
    virtual void SetTypeL(TAlertType aType) = 0;

    /**
     * Return the default timeout.
     * @return  The default timeout in milliseconds, or EForever.
     */
    virtual TInt DefaultTimeout() = 0;

    /**
     * Set the display timeout for the Alert.
     * If Alert is non-modal, the implementation is responsible for setting
     * a timer and should post an event when the timer completes.
     * @param   aTime
     *          The timeout in milliseconds, or EForever for modal alerts.
     */
    virtual void SetTimeoutL(TInt aTime) = 0;

    /**
     * Used to set the Alert's modal property.
     * Called by the framework when two or more Commands are added.
     */
    virtual void SetModalL() = 0;

    /**
     * Return if modal or not.
     * @return  ETrue or EFalse.
     */
    virtual TBool IsModal() = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EAlert;
    }
};

/**
 *
 * Interface to Form
 *
 */
class MMIDForm : public MMIDComponent
{
public:
    /**
     * Adds a list of Items to the Form, replacing the previous contents.
     * @param   aItems
     *          Reference to an array containing the Items to add.
     */
    virtual void SetAllItemsL(const RArray<MMIDItem*>& aItems) = 0;

    /**
     * Sets the Form Item referenced by aIndex to the specified Item,
     * replacing the previous Item.
     * @param   aItem
     *          Reference to the new Item to add.
     * @param   aIndex
     *          The index representing the old Item on the Form.
     */
    virtual void SetItemL(MMIDItem& aItem,TInt aIndex) = 0;

    /**
     * Adds the given Item to the form before the specified index.
     * @param   aItem
     *          Reference to the Item to add.
     * @param   aIndex
     *          The index before which to insert the Item.
     */
    virtual void InsertItemL(MMIDItem& aItem,TInt aIndex) = 0;

    /**
     * Removes the Item with the given index from the Form.
     * @param   aIndex
     *          The index representing the Item to remove.
     */
    virtual void DeleteItemL(TInt aIndex) = 0;

    /**
     * Removes all the Items from the Form.
     */
    virtual void DeleteAllItemsL() = 0;

    /**
     * Called following any Item method calls which result in the item
     * changing its appearance in some way, e.g. size, label and content
     * changed which may mean that the Item's min size has changed.
     * @param   aIndex
     *          The index representing the item on the form.
     */
    virtual void RefreshItemL(TInt aIndex) = 0;

    /**
     * Returns whether the item is actually visible to the user. Must therefore take into
     * account the visibility of the entire form, and whether the specific item is scrolled
     * in/out of the viewable area.
     * @param   aIndex
     *          The index representing the item on the form.
     * @return  ETrue or EFalse
     */
    virtual TBool IsItemVisible(TInt aIndex) = 0;

    /**
     * This will be called prior to the Form itself being made the current Displayable.
     * It provides an opportunity to prepare the Form such that this item will
     * be visible and focused when the form is eventually made current.
     * @param   aIndex
     *          The index representing the item on the form.
     */
    virtual void SetCurrentItemL(TInt aIndex) = 0;

    /**
     * Return width of displayable area available for items.
     * @return  Width of displayable area.
     */
    virtual TInt Width() = 0;

    /**
     * Return height of displayable area available for items.
     * @return  Height of displayable area.
     */
    virtual TInt Height() = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EForm;
    }
};


/**
 *
 * Interface for List.
 *
 * Implicit lists should notify the java side when the select operation
 * has been performed by calling MMIDEnv::PostJavaEvent with a reference
 * to the MMIDList implementing class and TSourceType == EDisplayable.
 */
class MMIDList : public MMIDComponent
{
public:
    /**
     * Insert a List item before the specified index.
     * @param   aIndex
     *          The index representing insertion position.
     * @param   aText
     *          Desriptor reference containing the text.
     * @param   aImage
     *          A pointer to an associated image. NULL if there
     *          is no image associated with this list element.
     */
    virtual void InsertElementL(TInt aIndex,const TDesC& aText,MMIDImage* aImage) = 0;

    /**
     * Replace the List item at the specified index with the new text.
     * @param   aIndex
     *          The index representing the item to replace.
     * @param   aText
     *          Desriptor reference containing the new text.
     * @param   aImage
     *          A pointer to the new associated image. NULL if there
     *          is no image associated with this list element.
     */
    virtual void SetElementL(TInt aIndex,const TDesC& aText,MMIDImage* aImage) = 0;

    /**
     * Delete the List item at the specified index.
     * @param   aIndex
     *          The index representing the item to delete.
     */
    virtual void DeleteElementL(TInt aIndex) = 0;

    /**
     * Delete the all the items in the List.
     */
    virtual void DeleteAllL() = 0;

    /**
     * Set the List item at the specified index to selected or unselected.
     * @param   aIndex
     *          The index representing the item.
     * @param   aSelected
     *          ETrue to set the item selected, EFalse to deselect the item.
     */
    virtual void SelectElementL(TInt aIndex,TBool aSelected) = 0;

    /**
     * Returns whether the item specified by index is selected or unselected.
     * @param   aIndex
     *          The index representing the item.
     * @return  ETrue if the item is selected, EFalse otherwise.
     */
    virtual TBool IsSelected(TInt aIndex) = 0;

    /**
     * Set the font for the item represented by the given index.
     * @param   aIndex
     *          The index representing the item.
     * @param   aFont
     *          Pointer to the font to use.
     */
    virtual void SetFontL(TInt aIndex, MMIDFont* aFont) = 0;

    /**
     * Sets the application's preferred policy for fitting element
     * contents to the available screen space.
     * @param   aFitPolicy
     *          The fit policy to use.
     */
    virtual void SetFitPolicyL(TInt aFitPolicy) = 0;

    /**
     * Notifies this MMIDList that the select command has changed.
     * This is only called on IMPLICIT Lists.
     *
     * This is only relevant to MMIDList implementing classes that provide a
     * visual means of invoking the select operation, e.g. a soft key.
     * If this is the case, the visual means should be removed when the select
     * command is set to null or to an application provided command.
     *
     * @param   aCommandID
     *      One of:
     *          The ID of a command that has been added to the list via AddCommandL
     *          - in this case the List should look up the command in its collection
     *            and use the label for its selection mechanism.
     *
     *          MMIDCommand::EListSelectCommand
     *          - in this case the List should use the default label for its
     *            selection mechanism.
     *
     *          MMIDCommand::ENullCommand
     *
     *          - in this case the list should not support selection. This value will
     *          be passed in response to the application calling:
     *              List.setSelectCommand(null);
     *
     */
    virtual void SetSelectCommand(TInt aCommandID) = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EList;
    }
};

/**
 *
 * Interface for TextBox
 *
 */
class MMIDTextBox : public MMIDComponent
{
public:
    /**
     * Deletes the given number of characters starting at the specified offset.
     * @param   aOffset
     *          The offset from which to start the deletion.
     * @param   aLength
     *          The number of characters to delete.
     */
    virtual void DeleteTextL(TInt aOffset,TInt aLength) = 0;

    /**
     * Sets the text to that supplied, replacing the previous contents.
     * @param   aText
     *          Reference to a descriptor containing the new text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;

    /**
     * Inserts the supplied text starting at the given position.
     * @param   aText
     *          Reference to a descriptor containing the new text.
     * @param   aPosition
     *          The positon at which to insert the new text.
     */
    virtual void InsertTextL(const TDesC& aText,TInt aPosition) = 0;

    /**
     * Sets the input constraints of the TextBox.
     * @param   aConstraints
     *          The new constraints.
     */
    virtual void SetConstraintsL(TUint aConstraints) = 0;

    /**
     * Sets the maximum number of characters that can be stored in
     * this texbox. Returns assigned maximum capacity.
     * The max size returned by this method will be used for exception checking
     * on the java side.
     * @param   aMaxSize
     *          The maximum number of characters.
     * @return  The assigned maximum number of characters.
     */
    virtual TInt SetMaxSizeL(TInt aMaxSize) = 0;

    /**
     * Returns the maximum number of characters that can be stored in
     * this texbox. Called once by the framework after construction.
     * The max size returned by this method will be used for exception checking
     * on the java side.
     * @return  The maximum number of characters.
     */
    virtual TInt GetMaxSize() = 0;

    /**
     * Gets the number of characters that are currently stored in this TextBox.
     * @return  The number of characters.
     */
    virtual TInt Size() = 0;

    /**
     * Gets the current input position.
     * @return  The current input position.
     */
    virtual TInt GetCaretPosition() = 0;

    /**
     * Gets the contents of the TextBox.
     * @return  The text is returned as a heap cell, and ownership is transferred.
     */
    virtual HBufC* GetTextL() = 0;

    /**
     * Sets a hint to the implementation as to the input mode that should
     * be used when the user initiates editing of this TextBox.
     * @param   aCharacterSubset
     *          The unicode character subset.
     */
    virtual void SetInitialInputModeL(const TDesC& aCharacterSubset) = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ETextBox;
    }
};


/**
 *
 * Interface to ImageItem
 *
 */
class MMIDImageItem : public MMIDItem
{
public:
    /**
     * Sets the Image contained within the ImageItem.
     * Snap shot of image has already been taken java side.
     * @param   aImage
     *          A pointer to the image to use.
     */
    virtual void SetImageL(MMIDImage* aImage) = 0;

    /**
     * Sets the text string to be used if the image exceeds the device's
     * capacity to display it.
     * @param   aAltText
     *          Descriptor reference containing the text.
     */
    virtual void SetAltTextL(const TDesC& aAltText) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EImageItem;
    }
};

/**
 *
 * Interface to Spacer.
 *
 * The Item methods setLabel(String aLabel), addCommand(Command aCommand)
 * and setDefaultCommand(Command aCommand) will never be called
 *
 */
class MMIDSpacer : public MMIDItem
{
public:
    /**
     * Sets the minimum size for this spacer.
     * @param   aSize
     *          The size to use.
     */
    virtual void SetMinimumSizeL(const TSize& aSize) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ESpacer;
    }
};

/**
 *
 * Interface to StringItem. The default font, i.e the one that should be used if
 * no font is set, should be the one returned by Font.getDefaultFont(), i.e. with
 * attributes ESystem,EPlain,EMedium
 *
 */
class MMIDStringItem : public MMIDItem
{
public:
    /**
     * Sets the text contents of the StringItem.
     * @param   aText
     *          Descriptor reference containing the text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;

    /**
     * Sets the preferred font for rendering this StringItem.
     * @param   aFont
     *          A pointer to the font to use.
     */
    virtual void SetFontL(MMIDFont* aFont) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EStringItem;
    }
};

/**
 *
 * Interface to DateField. This should be constructed in a "not initialized state" which
 * should be clearly identifiable. Only after user interaction or programmatically with
 * setDate() should it change.
 *
 */
class MMIDDateField : public MMIDItem
{
public:
    /**
     * Enumeration to indicate whether the DataField object contains
     * information about the time, the date, or both.
     */
    enum TInputMode
    {
        EDate = 1,
        ETime,
        EDateTime
    };

    /**
     * Compatability typedef.
     * @deprecated
     */
    typedef TInputMode TFieldType;
public:
    /**
     * Returns the current value as a date or time or both.
     * @return  The date/time
     */
    virtual TTime Date() const = 0;

    /**
     * Sets a new value for this field.
     * @param   aTime
     *          The new value.
     */
    virtual void SetDate(const TTime& aTime) = 0;

    /**
     * This method is called in response to DateField.setDate(null). The implementation
     * should respond by presenting the control as clearly being in a non-intialized state
     */
    virtual void SetUninitialized() = 0;

    /**
     * Change the date field type
     */
    virtual void SetInputModeL(TInputMode aInputMode) = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EDateField;
    }
};

/**
 *
 * Interface to Gauge.
 *
 */
class MMIDGauge : public MMIDItem
{
public:
    /**
     * Enumeration of various gauge behaviour patterns.
     */
    enum
    {
        EIndefinite = -1,
        EContinuousIdle,
        EIncrementalIdle,
        EContinuousRunning,
        EIncrementalUpdating
    };
public:
    /**
     * Sets the current value of this Gauge object.
     * If the gauge is in the indefinite mode, then aValue will take one of the values
     * EContinuousIdle, EIncrementalIdle, EContinuousRunning or EIncrementalUpdating.
     * @param   aValue
     *          The new value.
     */
    virtual void SetValueL(TInt aValue) = 0;

    /**
     * Get the current value of this Gauge object.
     * @return  The current value.
     */
    virtual TInt GetValue() = 0;

    /**
     * Sets the maximum value of this Gauge object.
     * aValue could take the special value EIndefinite in order to indicate that the Gauge
     * has indefinite range.
     * @param   aValue
     *          The maximum value.
     */
    virtual void SetMaxValueL(TInt aValue) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EGauge;
    }
};

/**
 *
 * Interface for TextField
 *
 */
class MMIDTextField : public MMIDItem
{
public:
    enum
    {
        /**
         * Constraints
         */
        EAny,
        EMailAddr,
        ENumeric,
        EPhoneNumber,
        EUrl,
        EDecimal,
        /**
         * Modifiers
         */
        EPassword=0x10000,
        EUneditable=0x20000,
        ESensitive=0x40000,
        ENonPredictive=0x80000,
        EInitialCapsWord=0x100000,
        EInitialCapsWordSentence=0x200000,
        EConstraintMask=0xFFFF
    };
public:
    /**
     * Deletes the given number of characters starting at the specified offset.
     * @param   aOffset
     *          The offset from which to start the deletion.
     * @param   aLength
     *          The number of characters to delete.
     */
    virtual void DeleteTextL(TInt aOffset,TInt aLength) = 0;

    /**
     * Sets the text to that supplied, replacing the previous contents.
     * @param   aText
     *          Reference to a descriptor containing the new text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;

    /**
     * Inserts the supplied text starting at the given position.
     * @param   aText
     *          Reference to a descriptor containing the new text.
     * @param   aPosition
     *          The positon at which to insert the new text.
     */
    virtual void InsertTextL(const TDesC& aText,TInt aPosition) = 0;

    /**
     * Sets the input constraints of the TextBox.
     * @param   aConstraints
     *          The new constraints.
     */
    virtual void SetConstraintsL(TUint aConstraints) = 0;

    /**
     * Returns assigned maximum capacity. The max size returned by this
     * method will be used for exception checking on the java side.
     */
    virtual TInt SetMaxSizeL(TInt aMaxSize) = 0;

    /**
     * Returns the maximum number of characters that can be stored in
     * this texbox. Called once by the framework after construction.
     * The max size returned by this method will be used for exception checking
     * on the java side.
     * @return  The maximum number of characters.
     */
    virtual TInt GetMaxSize() = 0;

    // client side method

    /**
     * Gets the number of characters that are currently stored in this TextBox.
     * @return  The number of characters.
     */
    virtual TInt Size() = 0;

    /**
     * Gets the current input position.
     * @return  The current input position.
     */
    virtual TInt GetCaretPosition() = 0;

    /**
     * Gets the contents of the TextBox.
     * @return  The text is returned as a HBufC*, and ownership is transferred.
     */
    virtual HBufC* GetTextL() = 0;

    /**
     * Sets a hint to the implementation as to the input mode that should
     * be used when the user initiates editing of this TextBox.
     * @param   aCharacterSubset
     *          The unicode character subset.
     */
    virtual void SetInitialInputModeL(const TDesC& aCharacterSubset) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ETextField;
    }
};

/**
 *
 * Interface for ChoiceGroup
 *
 */
class MMIDChoiceGroup : public MMIDItem
{
public:
    /**
     * Enueration of selection behaviour types.
     */
    enum TChoiceType
    {
        EExclusive=1,
        EMultiple,
        EImplicit,
        EPopup
    };
    /**
     * Enueration of display behaviour types.
     */
    enum TFitPolicy
    {
        EWrapDefault = 0,
        EWrapOn,
        EWrapOff
    };
public:
    /**
     * Insert a ChoiceGroup item before the specified index.
     * @param   aIndex
     *          The index representing insertion position.
     * @param   aText
     *          Desriptor reference containing the text.
     * @param   aImage
     *          A pointer to an associated image. NULL if there
     *          is no image associated with this element.
     */
    virtual void InsertElementL(TInt aIndex,const TDesC& aText,MMIDImage* aImage) = 0;

    /**
     * Delete the ChoiceGroup item at the specified index.
     * @param   aIndex
     *          The index representing the item to delete.
     */
    virtual void DeleteElementL(TInt aIndex) = 0;

    /**
     * Delete the all the items in the ChoiceGroup.
     */
    virtual void DeleteAllL() = 0;

    /**
     * Replace the ChoiceGroup item at the specified index with the new text.
     * @param   aIndex
     *          The index representing the item to replace.
     * @param   aText
     *          Desriptor reference containing the new text.
     * @param   aImage
     *          A pointer to the new associated image. NULL if there
     *          is no image associated with this list element.
     */
    virtual void SetElementL(TInt aIndex,const TDesC& aText,MMIDImage* aImage) = 0;

    /**
     * Set the ChoiceGroup item at the specified index to selected or unselected.
     * @param   aIndex
     *          The index representing the item.
     * @param   aSelected
     *          ETrue to set the item selected, EFalse to deselect the item.
     */
    virtual void SelectElementL(TInt aIndex,TBool aSelected) = 0;

    /**
     * Returns whether the item specified by index is selected or unselected.
     * @param   aIndex
     *          The index representing the item.
     * @return  ETrue if the item is selected, EFalse otherwise.
     */
    virtual TBool IsSelected(TInt aIndex) = 0;

    /**
     * Set the font for the item represented by the given index.
     * @param   aIndex
     *          The index representing the item.
     * @param   aFont
     *          Pointer to the font to use.
     */
    virtual void SetFontL(TInt aIndex, MMIDFont* aFont) = 0;

    /**
     * Sets the application's preferred policy for fitting element
     * contents to the available screen space.
     * @param   aFitPolicy
     *          The fit policy to use. One of the values of TFitPolicy.
     */
    virtual void SetFitPolicyL(TInt aFitPolicy) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EChoiceGroup;
    }
};


/**
 *
 * Interface for CustomItem
 *
 */
class MMIDCustomItem : public MMIDItem
{
public:
    /**
     * Enumeration of various behaviour types relating to
     * support for traversal, pointer events and keypresses.
     */
    enum TInteractionMode
    {
        ENone,
        ETraverseHorizontal,
        ETraverseVertical,
        EKeyPress=4,
        EKeyRelease=8,
        EKeyRepeat=0x10,
        EPointerPress=0x20,
        EPointerRelease=0x40,
        EPointerDrag=0x80
    };
public:
    /**
     * Gets the available interaction modes.
     * Client side method returning combination of flags supported
     * by device.
     * This will be called by a java thread and as such it must
     * not call any FS, FBS, CONE, UIKON or WSERV API's.
     * @return  The interaction modes.
     */
    virtual TInt InteractionModes() const = 0;

    /**
     * Sets the focus as specified.
     * @param   aFocus
     * @param   aScroll
     * @param   aDirection
     */
    virtual void SetFocusAndScroll(TBool aFocus,const TRect* aScroll,TInt aDirection) = 0;

    /**
     * Signals that the CustomItem's size and traversal location need to be updated.
     * Updates CustomItem with new MinContentSize and PrefContentSize received
     * from CustomItem.
     * @param   aMinSize
     *          The new minimum content size.
     * @param   aPrefSize
     *          The new preferred content size.
     */
    virtual void Invalidate(const TSize& aMinSize, const TSize& aPrefSize) = 0;

    /**
     * Returns the backing bitmap onto which Graphics may draw.
     * Does not transfer ownership.
     * @returns A pointer to the bitmap.
     */
    virtual CFbsBitmap* FrameBuffer() const = 0;

public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ECustomItem;
    }

    /**
     *@return A container on which video can be rendered
     */
    virtual MDirectContainer& DirectContainer() = 0;

#ifdef RD_TACTILE_FEEDBACK
    virtual  MMIDTactileFeedbackComponent* TactileFeedbackComponent() = 0;
#endif
};


/**
 *
 * Interface to Font. The font returned by Font.getDefaultFont() has attributes
 * ESystem,EPlain,EMedium
 *
 */
class MMIDFont : public MMIDComponent
{
public:
    /**
     * Font face
     */
    enum TFace
    {
        ESystem=0,
        EMonospaced=32,
        EProportional=64
    };
    /**
     * Font style
     */
    enum TStyle
    {
        EPlain=0,
        EBold=0x1,
        EItalic=0x2,
        EUnderlined=0x4,
    };
    /**
     * Font size
     */
    enum TSize
    {
        ESmall=8,
        EMedium=0,
        ELarge=16
    };
    /**
     * Font specifier
     */
    enum TFontSpecifier
    {
        EStaticText,
        EInputText
    };
public:
    /**
     * Returns a pointer to a font having the face, style, and size of this Font.
     *
     * Special note about Nokia scaling jad attribute: Nokia-MIDlet-Original-Display-Size
     * Returns a pointer to a font which is scaled according to the jad attribute.
     * When called with parameter ETrue scaling attribute is not taken into account.
     *
     * @return A pointer to the font.
     */
    virtual CFont* Font(TBool aHighLevelComponent = EFalse) = 0;

    /**
     * Returns the height in pixels of text drawn with this Font.
     * @return The height.
     */
    virtual TInt Height() = 0;

    /**
     * Gets the distance in pixels from the top of the text to the text's baseline.
     * @return The distance in pixels from the top of the text to the text's baseline.
     */
    virtual TInt Baseline() = 0;

    /**
     * Gets the total advance width for showing the specified string using this Font.
     * @param   aString
     *          Reference to a descriptor containing the text.
     * @return  The display width.
     */
    virtual TInt Width(const TDesC& aString) = 0;

    /**
     * Returns whether text draw using this Font is underlined.
     * @return ETrue or EFalse
     */
    virtual TBool IsUnderlined() const = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return EFont;
    }
};

/**
 * Structure combining the various font attributes.
 */
struct SFontSpec
{
    MMIDFont::TStyle iStyle;
    MMIDFont::TFace iFace;
    MMIDFont::TSize iSize;
};

/**
 *
 * Interface to Ticker
 *
 */
class MMIDTicker : public MMIDComponent
{
public:
    /**
     * Sets the text to that supplied, replacing the previous contents.
     * @param   aText
     *          Reference to a descriptor containing the new text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;
public:
    /**
     * Returns the component type.
     * @return  The component type as a MMIDComponent::TType enum.
     */
    TType Type() const
    {
        return ETicker;
    }
};

/**
 *
 * Class for general utilities. Gives the framework general query methods
 * for querying the implementation
 *
 */
class MMIDUtils
{
public:
    enum
    {
        EActionUp=1,
        EActionDown=6,
        EActionLeft=2,
        EActionRight=5,
        EActionFire=8,
        EActionGameA=9,
        EActionGameB=10,
        EActionGameC=11,
        EActionGameD=12
    };
    enum TImageType
    {
        EListImage = 1,
        EChoiceImage,
        EAlertImage
    };
    enum TColorType
    {
        EColorBackground=0,
        EColorForeground=1,
        EColorHighlightedBackground=2,
        EColorHighlightedForeground=3,
        EColorBorder=4,
        EColorHighlightedBorder=5
    };
    enum TGraphicsType
    {
        ESolid=0,
        EDotted=1
    };
public:

    /**
     * Alert the user by playing the sound for this AlertType.
     * @param   aType
     *          The alert type for which to play the sound.
     * @return ETrue if the user was alerted, EFalse otherwise.
     */
    virtual TBool PlaySound(TInt aType) = 0;

    /**
     * Return whether the framework should attempt to translate
     * the scancode and post an event up to Java.
     * This method may be unneccessary as the filtering
     * can be done in MapScanCode(MapSpecialKey)below.
     * @param   aScanCode
     *          The scancode to be translated.
     * @return  ETrue or EFalse
     */
    virtual TBool IsJavaKey(TInt aScanCode) = 0;

    /**
     * Translate scan code to MIDP key code. Only called for
     * 'special' codes that could not be mapped to unicode
     * characters. Must return a negative key code or zero
     * if no key event should be sent to Java.
     * @param   aScanCode
     *          The scancode to be translated.
     * @return  The MIDP key code.
     */
    virtual TInt MapNonUnicodeKey(TUint aScanCode) = 0;

    /**
     * Gets an informative key string for aKeyCode
     * @param   aText
     *          Descriptor references to receive the key string.
     * @param   aKeyCode
     *          The keycode to retrieve.
     */
    virtual void GetKeyName(TDes& aText,TInt aKeyCode) = 0;

    /**
     * Return the MIDP Canvas game action corresponding to
     * MIDP key code aKeyCode, or zero if no game action is
     * associated with this key code.
     * @param   aKeyCode
     *          The keycode to retrieve.
     * @return  The MIDP Canvas game action.
     */
    virtual TInt GetGameAction(TInt aKeyCode) = 0;

    /**
     * Return the cannonical key code corresponding to the
     * Canvas game action aGameAction.
     * Must provide a mapping for every game action.
     * Return zero if aGameAction is not a valid game action.
     * @param   aGameAction
     *          The game action.
     * @return  The keycode corresponding to the game action.
     */
    virtual TInt GetKeyCode(TInt aGameAction) = 0;

    /**
     * Map one of the standard font types INPUT/STATIC to a font specifier.
     * This mapping is platform dependent.
     *
     * An INPUT Text Specifier will use:-
     *      Face: EMonospaced, Style: EBold, Size: EMedium
     * A STATIC Text Specifier will use:-
     *      Face: EProportional, Style: EPlain, Size: ESmall
     * Otherwise a default specifer will be chosen:-
     *      Face: ESystem, Style: EPlain, Size: ESmall
     * @param   aSpecifier
     *          The font type
     * @return  The font specifier
     */
    virtual SFontSpec FontSpecifierSpecs(MMIDFont::TFontSpecifier aSpecifier) = 0;

    /**
     * Requests a flashing effect for the device's backlight.
     * @param   aDuration
     *          The length of time to flash.
     * @return  ETrue if the request was satisfied, EFalse otherwise.
     */
    virtual TBool FlashBacklightL(const TTimeIntervalMicroSeconds32& aDuration) = 0;

    /**
     * Requests operation of the device's vibrator.
     * @param   aDuration
     *          The length of time to vibrate.
     * @return  ETrue if the request was satisfied, EFalse otherwise.
     */
    virtual TBool Vibrate(const TTimeIntervalMicroSeconds32& aDuration) = 0;

    /**
     * Mapping Uikon TLogical Color to Java ColorSpecifier
     * @param   aColorSpecifier
     *          The Java colour specifier.
     * @return  The Uikon colour.
     */
    virtual TInt Color(TColorType aColorSpecifier) = 0;

    /**
     * Returns if the platform supports pointer press and release events.
     * @return  ETrue or EFalse.
     */
    virtual TBool HasPointerEvents() = 0;

    /**
     * Returns if the platform supports pointer motion events (pointer dragged).
     * @return  ETrue or EFalse.
     */
    virtual TBool HasPointerMotionEvents() = 0;

    /**
     * Returns if the platform generates repeat events when key is kept down.
     * @return  ETrue or EFalse.
     */
    virtual TBool HasRepeatEvents() = 0;

    /**
     * Returns the stroke style used for border drawing depending
     * on the state of the component (highlighted/non-highlighted).
     * @param   aHighlighted
     *          If ETrue, return highlighted stroke style, otherwise non-highlighted.
     * @return  The bstroke style.
     */
    virtual TGraphicsType BorderStyle(TBool aHighlighted) = 0;

    /**
     * Returns the Best image size for a given image type.
     * @param   aImageType
     *          The image type.
     * @return  The best image size.
     */
    virtual TSize BestImageSize(TImageType aImageType) const = 0;

    /**
     * Method fills aText parameter with String value of actual keyboard layout.
     * @param   aText
     *          String value of actual keyboard layout.
     */
    virtual void GetKeyboardTypeName(TDes* aText) = 0;

    /**
     * Method returns value of the scan code of the latest key event.
     * @return   Integer value of the scan code of the latest key event.
     */
    virtual TInt GetKeyScanCode() = 0;

    /**
     * Update with last key event.
     * @return   Integer value of the scan code of the latest key event.
     */
    virtual void SetLastKeyEvent(const TKeyEvent& aEvent) = 0;

    /**
     * Method returns value of the modifier keys state.
     * @return   Integer value of the scan code of the latest key event.
     */
    virtual TInt GetKeyModifier() = 0;

    /**
     * Map key event data with ITI Engine.
     */
    virtual void MappingDataForKey(TKeyEvent& aEvent, TEventCode aType) = 0;

    /**
     * Update Utils implementation with resoruce change.
     */
    virtual void HandleResourceChangedL() = 0;

    /**
     * Update Utils implementation with foreground change.
     */
    virtual void HandleForegroundL(TBool aForeground) = 0;

    virtual void Dispose() = 0;
};

/**
 *
 * Definition of events which can be posted to Java
 *
 */
enum TSourceType
{
    EItem=3,
    EDisplayable,
    EMIDlet,
    ECanvasGraphicsItemPainterEvent=7 //number 6 is used for deprecated item on Java side
};

/**
 * Various event types used throught the VM.
 */
enum TEventType
{
    ENoType=0,
    // Canvas & CustomItem
    EPaint=10,
    EKeyPressed,
    EKeyReleased,
    EKeyRepeated,
    EPointerPressed,
    EPointerReleased,
    EPointerDragged,
    // Displayable
    ESizeChanged,
    // MIDlet
    EExit=18,
    //posted by the framework when the MIDlet switches to the foreground
    EForeground,
    //posted by the framework when the MIDlet switches to the background
    EBackground,
    EPause,
    EStart,
    EDestroy,
    // Displayable
    ECommand,
    // CustomItem
    EVisible,
    EFocusTraversal,
    ERefresh,               // deprecated
    // Display
    EMakeItemCurrent,       // deprecated
    ESetCurrent,
    ESerial,
    //CanvsGraphicsItem events
    ECanvasGraphicsItemRepaint,
    // Alert
    EDismiss = 0,           // Futuredev: value
    // Form/Item
    EItemChanged = 0,       // Futuredev: value
    // EList
    ESelect = 0             // Futuredev: value
};

/**
 * Reusable notify priority event used to unblock a Java thread
 * waiting for completion of a native async operation.
 */
class MMIDNotifyEvent
{
public:
    /**
     * Set the value passed into the VM as the async operation result/error code.
     * @param   aResult
     *          The result/error code.
     */
    virtual void SetResult(TInt aResult) = 0;
    /**
     * Destroy the event. Caller must guarantee that the event is not
     * enqueued.
     */
    virtual void Dispose() = 0;
};

/**
 * MIDP Key Event.
 *
 * Encapsulates key event information required to translate and post
 * key events.
 *
 * Must be translated from a WSERV TKeyEvent via a call to
 * MMIDEnv::TranslateKey() before calling MMIDEnv::PostKeyEvent()
 *
 */
struct TMIDKeyEvent
{
    enum TEvent
    {
        EPressed =0x01,
        EReleased=0x02,
        ERepeated=0x04
    };

    TUint32 iEvents;
    TInt    iKeyCode;
    TInt    iRepeats;
};

/**
 *
 * A MIDlet environment observer for receiving general events about the environment (i.e. that
 * releate to the entire MIDlet as opposed to a particular Displayable etc.). Foreground/
 * SwitchOn events are useful, for example, for suspending animations which would otherwise keep
 * the machine alive
 *
 */
class MMIDEnvObserver
{
public:
    /**
     * HandleSwitchOnL(ETrue) is called in these circumstances:
     * - When the device is switched on.
     * - If the MIDlet has been paused due to a CSaveNotifier event,
     *   HandleSwitchOnL(ETrue) is called when the MIDlet is brought to the foreground.
     *
     * HandleSwitchOnL(EFalse) is called in these circumstances:
     * - The framework uses the CSaveNotifier framework to provide MIDlet state changes.
     *   When the following events are received ESaveAll,ESaveQuick,ESaveData,EReleaseRAM
     *   the MIDlet is paused and HandleSwitchOnL(EFalse) is called.
     */
    virtual void HandleSwitchOnL(TBool aSwitchOn) = 0;

    /**
     * Handles the case when the MIDlet is brought to the foreground.
     */
    virtual void HandleForegroundL(TBool aForeground) = 0;

    /**
     * Handles a change to resources which are shared accross the environment.
     */
    virtual void HandleResourceChangeL(TInt aType) = 0;
};

/**
 *
 * A MIDlet environment, one per MIDlet, providing utility methods for the MIDP implementation.
 * A concrete implementation is provided by the framework, passed in through
 * MMIDComponentFactory::ConstructL() where the implementation can cache it and use it as
 * necessary.
 *
 */
class MMIDEnv
{
public:
    /**
     * Use these methods to disptach events back into Java
     */
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TEventType aEventType,TInt aEventData,TInt aEventData1,TInt aEventData2) = 0;
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TEventType aEventType=ENoType,TInt aEventData=0) = 0;
    virtual TBool PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TInt aEventData) = 0;
    virtual TBool PostMidletEvent(TEventType aEventType) = 0;

    /**
     * Create a notify priority event to send up to the java peer to
     * aSource.
     */
    virtual MMIDNotifyEvent* NewNotifyL(MMIDComponent& aSource) = 0;

    /**
     * Post a notify priority event to unblock a java thread.
     * MMIDNotifyEvent instances are 'reusable' events. That is the event
     * queue does not delete the instance after dispatch. Due to the intrusive
     * queue structure used, a resuable event cannot be present on the queue
     * twice.
     * The client is responsible for ensuring that the event does not get
     * posted on to the queue unless there are no pending dispatches for that
     * event. i.e. unless the event is not already on the queue.
     *
     * This is usually acheived with an object monitor in java protecting the
     * native method(s) that required an async notification.
     */
    virtual TBool    PostJavaNotify(MMIDNotifyEvent* aEvent) = 0;

    /**
     * Key translation. Maps OS key event to MIDP key event
     *
     * @param aEvent
     * @param aKeyEvent
     * @param aType
     * @return
     *
     * @since S60
     */
    virtual TBool TranslateKeyL(TMIDKeyEvent& aEvent, const TKeyEvent& aKeyEvent, TEventCode aType) = 0;
    virtual TBool   PostKeyEvent(MMIDComponent& aSource, TMIDKeyEvent& aEvent) = 0;
    virtual void    ResetKeys() = 0;

    /**
     * Returns the Displayable which has last been made current, or null if none has
     */
    virtual MMIDDisplayable* Current() = 0;
    /**
     * MIDlet properties
     */
    virtual TPtrC MidletName() const = 0;
    virtual TUid MidletUid() const = 0;
    virtual TPtrC MidletHome() const = 0;

#ifdef RD_SCALABLE_UI_V2
    // This function can be moved out from RD_SCALABLE_UI_V2 flag if needed.
    // It is behind this flag because currently it is used only by Touch.
    /**
     * Returns MIDlet suite uid of this midlet.
     * The midlet suite uid is same for all midlets inside one midlet suite.
     *
     * @return  MIDlet suite uid
     * @since S60 5.0
     */
    virtual TUid MidletSuiteUid() = 0;
#endif //RD_SCALABLE_UI_V2

    /**
     *@return The number of colors available in the
     * displaymode used for Images and Graphics.
     */
    virtual TInt NumColors() = 0;

    /**
     *@return The SymbianOS display mode that the VM will use for
     * Images, Canvases and CustomItems.
     */
    virtual TDisplayMode DisplayMode() = 0;

    /**
     * Sets the zoomed size of the canvas.
     * May be set to TSize(0,0) if no zooming.
     */
    virtual void  SetCanvasZoomSize(const TSize& aSize) = 0;

    /**
     * Returns the zoomed size of the canvas paintable area.
     * May return TSize(0,0) if no zooming is in effect.
     */
    virtual TSize CanvasZoomSize() = 0;

    /**
     * Returns assumed canvas size as specified in jad, or TSize(0,0)
     * if none was specified. This determines the size of the canvas in
     * pixels that is paintable by the java peer. Zooming may alter the
     * apparent size on screen.
     */
    virtual TSize CanvasAssumedSize() = 0;

    /**
     * Add/remove observers for receiving events about the environment. It is important that
     * observers are removed before being deleted.
     */
    virtual void AddObserverL(MMIDEnvObserver& aObserver) = 0;
    virtual void RemoveObserver(MMIDEnvObserver& aObserver) = 0;

    /**
     * Gets a MIDlet Suite attribute value. MIDlet Suite attributes are specified in the
     * MIDlet Suite manifest and jad.
     *
     * @param   aAttributeName
     *          The name of the attribute to get.
     * @param   aAttributeValue
     *          On return, this will be set to the attribute value.
     * @return  KErrNone if aAttributeValue has successfully been set.
     *          KErrNotFound if there is no attribute whose name is aAttributeName.
     */
    virtual TInt MidletAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue)  = 0;

    /**
     * Checks if the MIDlet Suite attribute is set to certain value.
     *
     * @param   aAttributeName
     *          The name of the attribute whose content is checked.
     * @param   aAttributeValue
     *          The value the of the attribute. Method checks whether the attribute
     *          specifed by aAttributeName is set to this value. Comparison is case
     *          insensitive but otherwise the string much match exactly (e.g. no spaces allowed)
     * @return  ETrue if aAttribute is set to value aAttributeValue, EFalse otherwise
     *
     * @see MMIDEnv::MidletAttribute()
     */
    virtual TBool MidletAttributeIsSetToVal(const TDesC& aAttributeName, const TDesC& aAttributeValue) = 0;

    /**
     * Checks if the MIDlet Suite attribute contains a certain value. If a jad attribute contains several
     * values, they must be separated with commas only, spaces between commas and value string are not allowed.
     *
     * @param   aAttributeName
     *          The name of the attribute whose content is checked.
     * @param   aAttributeValue
     *          The value the of the attribute. Method checks whether the attribute
     *          specifed by aAttributeName contains this value. Comparison is case
     *          insensitive.
     * @return  ETrue if aAttribute contains value aAttributeValue and attribute syntax is correct,
     *          EFalse otherwise
     *
     * @see MMIDEnv::MidletAttribute()
     */
    virtual TBool MidletAttributeContainsVal(const TDesC& aAttributeName, const TDesC& aAttributeValue) = 0;

    /**
     * Change the default screen size.
     *
     * @param   aSize
     *          The size which the screen will become.
     */
    virtual void SetCanvasAssumedSize(const TSize& aSize) = 0;

    /**
     * Reserve a native frame buffer for a <code>MMIDCanvas</code> object.
     * MMIDCanvas's counterpart in java side could be <code>Canvas</code> or
     * <code>GameCanvas</code>. A MIDlet's <code>Canvas</code> objects should share the
     * same native side frame buffer but <code>GameCanvas</code> objects must use their
     * own frame buffers.
     * @param   aCanvas Target Canvas
     * @param   aSz Frame buffer size
     * @return  Frame buffer.
     * @since S60 5.0
     */
    virtual CFbsBitmap* ReserveCanvasFrameBufferL(MMIDCanvas& aCanvas, const TSize& aSz) = 0;

    /**
     * Release a frame buffer.
     * @param   aCanvas Target canvas
     * @param   aFrameBuffer If <code>aCanvas</code> object's counterpart in java side is
                <code>GameCanvas</code> then the <code>aFrameBuffer</code> is just deleted.
                However if the counterpart is <code>Canvas</code> then an internal reference
                count is decreased. If the reference count goes to zero then the frame buffer
                (that was shared shared by multiple <code>MMIDCanvas</code> objects) is going
                to be deleted.
     * @since S60 5.0
     */
    virtual void ReleaseCanvasFrameBuffer(MMIDCanvas& aCanvas, CFbsBitmap*& aFrameBuffer) = 0;

    /**
     * A Canvas object's background image (e.g image/theme) might be shown or not.
     * A backgound image can be shown if the <code>Nokia-UI-Enhancement</code> attribute
     * is <code>CanvasHasBackground</code>. The attribute may be placed in the JAD or the manifest.
     * @param   aCanvas Target canvas
     * @param   Return <code>ETrue</code> if a background image can be shown.
     * @since S60 5.0
     */
    virtual TBool CanvasHasBackground(const MMIDCanvas& aCanvas) const = 0;

#ifdef RD_JAVA_NGA_ENABLED

    /**
     * Hardware accelarator types
     * @since S60 9.2
     */
    typedef enum
    {
        EHardware3D = (1 << 1),
        EHardware2D = (1 << 2)
    } THardwareType;

    /**
     * Checks if hardware acceleration is available
     * @param aHardwareType Defines the type
     * @return True if specified hw exists.
     * @since S60 9.2
     */
    virtual TBool IsHardwareAcceleratedL(
        MMIDEnv::THardwareType aHardwareType) = 0;
#endif // RD_JAVA_NGA_ENABLED

    /**
     * Maps low level key event
     * @param    aMidKeyEvent key event output data
     * @param    aEvent key event input data
     * @since S60 3.2.3
     */
    virtual void MappingDataForKey(TKeyEvent& aEvent, TEventCode aType) = 0;

    /**
     * Update with last key event.
     * @param aEvetn key event
     * @since S60 3.2.3
     */
    virtual void SetLastKeyEvent(const TKeyEvent& aEvent) = 0;

    /**
     * Gets an instance of ToLcduiObserver.
     *
     * @since S60 5.0
     * @return Pointer to ToLcduiObserver instance
     */
    virtual MMIDToLcduiObserver& ToLcduiObserver() = 0;

    /**
     * Inform CMIDEnv about deleting of object implementing MMIDDisplayable.
     *
     * @param displayable deleting displayble
     * @since Java 2.0
     */
    virtual void DisplayableIsDestructed(const MMIDDisplayable* aDisplayable) = 0;
};

/**
 *
 * Interface class for provider of asynchronous calls to lcdui
 * which may origin in non-lcdui thread.
 *
 * Some events may invoke a callback into specified callback interface.
 * The receiver of this callback must not be deleted before the event
 * containing it is processed.
 *
 * The callback receiver can be for example implemented
 * its instance deletion via event using this observer,
 * so this deletion event will be the last one for that receiver.
 *
 * @since  S60 v5.0
 */
class MMIDToLcduiObserver
{
public:
    /**
     * Allows a control to be used during event processing.
     *
     * @since S60 5.0
     * @param aControl Control to be registered.
     * @param aCallbackContainer MDirectContainer that is nofied in LCDUI thread
     *                           about added MDirectContent. NULL, if notification
     *                           is not needed.
     */
#ifdef RD_JAVA_NGA_ENABLED
    virtual void RegisterControl(CCoeControl& aControl,
                                 MDirectContainer* aCallbackContainer = NULL) = 0;
#else
    virtual void RegisterControl(CCoeControl& aControl) = 0;
#endif

    /**
     * Removes a control from the list of controls allowed
     * to be used in event processing.
     * Events which works with this control will be ignored.
     *
     * @since S60 5.0
     * @param aControl Control to be unregistered.
     */
    virtual void UnregisterControl(CCoeControl& aControl) = 0;

    /**
     * Flushes the control's graphics content on screen.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aControl Control used to get DSA resources from.
     * @param aRect An area of the control to flush.
     */
    virtual void FlushControl(
        CCoeControl& aControl,
        const TRect &aRect) = 0;

    /**
     * Invokes aConsumer->MdcDSAResourcesCallback from LCDUI ES thread.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aControl Control used to get DSA resources from.
     * @param aConsumer Consumer of the callback. The instance
     *                  must not be deleted before the event processed.
     */
    virtual void InvokeDSAResourcesCallback(
        CCoeControl& aControl,
        MUiEventConsumer& aConsumer) = 0;

    /**
     * Invokes aConsumer->MdcUICallback( aCallbackId ) from LCDUI ES thread.
     * The call may origin in other than LCDUI ES thread.
     *
     * @since S60 5.0
     * @param aConsumer Consumer of the callback. The instance
     *                  must not be deleted before the event processed.
     * @param aCallbackId Id which is provided to the callback
     */
    virtual void InvokeUICallback(
        MUiEventConsumer& aConsumer,
        TInt aCallbackId) = 0;
};

/**
 *
 * Interface class for constructing native peers to LCDUI objects
 *
 * Canvas and Graphics do not have create methods since they
 * are platform independent.
 *
 */
class MMIDComponentFactory
{
public:
    virtual void ConstructL(MMIDEnv& aEnv) = 0;

    /**
     * Classes derived from Displayable
     */
    virtual MMIDDisplayable* CreateDisplayableL() = 0;

    virtual MMIDCanvas* CreateCanvasL(MMIDDisplayable& aDisplayable, MMIDComponent::TType aCanvasType) = 0;

    virtual MMIDAlert* CreateAlertL(MMIDDisplayable& aDisplayable,MMIDAlert::TAlertType aType,const TDesC& aString,MMIDImage* aPicture) = 0;
    virtual MMIDForm* CreateFormL(MMIDDisplayable& aDisplayable) = 0;
    virtual MMIDList* CreateListL(TInt aType,MMIDDisplayable& aDisplayable,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray) = 0;
    virtual MMIDTextBox* CreateTextBoxL(TInt aConstraints,TInt aMaxSize,const TDesC& aText,MMIDDisplayable& aDisplayable) = 0;

    /**
     * Classes derived from Item
     */
    virtual MMIDImageItem* CreateImageItemL(const TDesC& aLabel,MMIDImage* aImage,MMIDItem::TLayout aLayout,MMIDItem::TAppearance aAppearance,const TDesC& aAltText) = 0;
    virtual MMIDStringItem* CreateStringItemL(const TDesC& aLabel,const TDesC& aText,MMIDItem::TAppearance aAppearance) = 0;
    virtual MMIDDateField* CreateDateFieldL(const TDesC& aLabel, MMIDDateField::TInputMode aInputMode) = 0;

    /**
     * aMaxValue could take the special value MMIDGauge::EIndefinite in order to indicate that the Gauge
     * has indefinite range. If this is the case, then aInitialValue will take one of the values
     * MMIDGauge::EContinousIdle,MMIDGauge::EContinousRunning, MMIDGauge::EIncrementalIdle or
     * MMIDGauge::EIncrementalRunning
     */
    virtual MMIDGauge* CreateGaugeL(const TDesC& aLabel,TBool aInteractive,TInt aMaxValue,TInt aInitialValue) = 0;

    virtual MMIDTextField* CreateTextFieldL(const TDesC& aLabel,const TDesC& aText,TInt aConstraints,TInt aMaxSize) = 0;
    virtual MMIDChoiceGroup* CreateChoiceGroupL(const TDesC& aLabel,TInt aType,RArray<TPtrC>& aStrings, RArray<MMIDImage*>& aImages) = 0;
    virtual MMIDSpacer* CreateSpacerL(const TSize& aMinimumSize) = 0;
    virtual MMIDCustomItem* CreateCustomItemL(const TDesC& aLabel) = 0;

    /**
     * Other UI components
     */
    virtual MMIDFont* CreateFontL(TUint aStyle,TUint aSize,TInt aFace,TBool aIsFreeSizeFont) = 0;
    virtual MMIDTicker* CreateTickerL(const TDesC& aText) = 0;
    virtual MMIDCommand* CreateCommandL(const TDesC& aShortLabel,const TDesC& aLongLabel,MMIDCommand::TCommandType aCommandType,TInt aPriority,TInt aCommandID) = 0;

    /**
     * Create a Utility Class object.
     */
    virtual MMIDUtils* CreateUtilsL() = 0;

    /**
     * Destroy the component factory and free the resources.
     */
    virtual void Dispose() = 0;

    /**
     * Creates a new text editor component. Note that text editor is current a custom
     * component and not a standard LCDUI UI component.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aMaxSize The maximum size of the editor's content.
     * @param aWidth The width of the editor in pixels.
     * @param aHeight The height of the editor in pixels.
     * @param aHeightInRows Defines whether the height of the editor should be
     *        represented as in rows or in pixels.
     */
    virtual MMIDTextEditor* CreateTextEditorL(
        TInt aMaxSize,
        TInt aWidth,
        TInt aHeight,
        TBool aHeightInRows) = 0;

    /**
     * Creates a new canvas graphics item component. Note that canvas
     * graphics item is currently a custom component and not a standard
     * LCDUI UI component.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aItemPainter Pointer to its painter control.
     * @since S60 5.0
     */
    virtual MMIDCanvasGraphicsItem* CreateCanvasGraphicsItemL(
        MMIDCanvasGraphicsItemPainter* aItemPainter) = 0;

    /**
     * Creates a new canvas graphics item painter control.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aWidth The width of the editor in pixels.
     * @param aHeight The height of the editor in pixels.
     * @since S60 5.0
     */
    virtual MMIDCanvasGraphicsItemPainter* CreateCanvasGraphicsItemPainterL(
        TInt aWidth,
        TInt aHeight) = 0;
};


inline TInt MIDHandle(MMIDComponent* aComponent)
{
    return TInt(reinterpret_cast<TUint32>(aComponent) >> 2);
}

inline MMIDComponent* MIDUnhandComponent(TInt aHandle)
{
    ASSERT(aHandle >= 0);
    return reinterpret_cast<MMIDComponent*>(aHandle << 2);
}

template <class T>
inline T*   MIDUnhand(TInt aHandle)
{
    return static_cast<T*>(MIDUnhandComponent(aHandle));
}

#define MIDUnhandObject MIDUnhand

/**
 * Buffered operations passed to MMIDBufferProcessor interfaces
 */
struct TMIDBufferOp
{
public:
    /**
     *@return The opcode written into the buffer by the
     * Java peer. Each buffer processor class has its own
     * independent opcode space.
     */
    inline TInt         OpCode() const
    {
        ASSERT(!IsActivate());
        return TInt(Header() >> 16);
    }

    /**
     *@return size of operation in words including the header.
     */
    inline TInt         Size() const
    {
        return TInt(Header() & 0x0000FFFF);
    }

    /**
     * Debug builds will panic if this method is called on an opcode with no data.
     *
     *@return Pointer to start of opcode data.
     */
    inline const TAny*  Data() const
    {
        ASSERT(Size() > 1);
        return (const TAny*)&(iHeader[1]);
    }

    /**
     * Used by framework to track changes of processor. Processors
     * should never receive an TMIDBufferOp for which IsActivate
     * returns ETrue.
     *
     *@return ETrue if this command is an activation command.
     */
    inline TBool    IsActivate() const
    {
        return (Header() >> 31);
    }

    /**
     * Used by framework.
     *
     *@return op header.
     */
    inline TUint32  Header() const
    {
        return iHeader[0];
    }

private:
    TUint32 iHeader[1];
};

/**
 * Interface for buffered data processors.
 *
 * Any MMIDComponent that returns a non-NULL MMIDBufferProcessor pointer
 * from MMIDComponent::Processor() may receive buffered data from its
 * Java peer.
 *
 * Note: The MMIDComponent is considered to own the MMIDBufferProcessor it
 * returns and is therefore responsible for cleaning up any resources used
 * by the processor.
 */
class MMIDBufferProcessor
{
public:
    enum
    {
        KMinOpCycles = 1,
        KMaxOpCycles = 1024
    };

    /**
     * Mask of valid opcode bits. Top 4 bits of each opcode must be clear.
     */
    enum
    {
        KOpMask = 0x0FFF
    };

    /**
     * Process a block of commands, updating read pointer as you go.
     *
     * aBegin is the address of the first command in the buffer for this processor.
     *
     * aEnd is the address of the command *after* the last command in the buffer for this processor,
     * hence (aEnd-aBegin) is the total number of words of opcodes and data.
     * Classes that implement this interface must not call any of the TMIDBufferOp
     * methods on aEnd - it is merely a sentinel indicating the end of the valid
     * range, and intended to be used in loops of the form:
     * <CODE>
     * TMIDBufferOp& op = aBegin;
     * while( op < aEnd )
     *  {
     *      DecodeOp(op);
     *      op += op->Size();
     *  }
     * </CODE>
     * aCycles is the amount of 'compute effort' remaining in the current AO
     * RunL. The processor should decrement this value by an amount that refelects
     * the amount of work it expects each operation it performs to have consumed.
     * The estimate need not be accurate - only give an indication, to improve
     * concurrency. Failure to decrement aCycles by a sensible amount can lead to
     * the VM threads being starved of events such as key presses.
     *
     * For example simple graphics primitives like DrawLine are fast so would decrement
     * by a small amount, whilst complex graphics primitives, such as drawing a
     * rotated image with an alpha channel, would decrement by a large amount.
     *
     * If before commencing an operation, aCycles is less than that required to
     * complete the operation, the processor should defer, by returning immediately
     * leaving aBegin pointing at the deferred operation.
     *
     * returns ETrue if processing will be async.
     *
     * @param aMonitor  Monitor that needs to be notified by MMIDBufferProcessor
     *                  after async processing is finished.
     *
     */
#ifdef RD_JAVA_NGA_ENABLED
    virtual TBool ProcessL(
        const TMIDBufferOp*& aBegin, const TMIDBufferOp* aEnd,
        TInt& aCycles, java::util::Monitor* aMonitor) = 0;
#else
    virtual TBool ProcessL(
        const TMIDBufferOp*& aBegin, const TMIDBufferOp* aEnd,
        TInt& aCycles, TRequestStatus* aStatus = NULL) = 0;
#endif

    /**
     * Called by framework if the processor is doing some async operation
     * when the MIDlet is closed down. Processor should cancel the async
     * operation and clean up any resources.
     */
    virtual void  AbortAsync() = 0;
};

#endif // LCDUI_H
