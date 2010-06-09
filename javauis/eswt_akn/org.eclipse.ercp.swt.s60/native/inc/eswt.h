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


#ifndef ESWT_H
#define ESWT_H


#include <coedef.h>
#include <eikenv.h>
#include <w32std.h>


class _jweak;
class CCoeControl;
class CEikAppUi;
class CSwtJavaBufferDrawer;
class MSwtMenuArranger;
class MSwtCommandArranger;
class MSwtMobileDevice;
class MSwtComposite;
class MSwtControl;
class MSwtFactory;
class MSwtDevice;
class MSwtFont;
class MSwtImage;
class MSwtText;
class MSwtUiUtils;
class MSwtVerifyEventObserver;
class MSwtBrowser;
class MSwtMenu;


/*
 * SWT style constants
 */
const TInt KSwtStyleBar              = 1 << 1;
const TInt KSwtStyleDropDown         = 1 << 2;
const TInt KSwtStylePopUp            = 1 << 3;
const TInt KSwtStyleSeparator        = 1 << 1;
const TInt KSwtStyleToggle           = 1 << 1;
const TInt KSwtStyleArrow            = 1 << 2;
const TInt KSwtStylePush             = 1 << 3;
const TInt KSwtStyleRadio            = 1 << 4;
const TInt KSwtStyleCheck            = 1 << 5;
const TInt KSwtStyleCascade          = 1 << 6;
const TInt KSwtStyleMulti            = 1 << 1;
const TInt KSwtStyleSingle           = 1 << 2;
const TInt KSwtStyleReadOnly         = 1 << 3;
const TInt KSwtStyleWrap             = 1 << 6;
const TInt KSwtStyleSimple           = 1 << 6;
const TInt KSwtStylePassword         = 1 << 22;
const TInt KSwtStyleShadowIn         = 1 << 2;
const TInt KSwtStyleShadowOut        = 1 << 3;
const TInt KSwtStyleShadowEtchedIn   = 1 << 4;
const TInt KSwtStyleShadowEtchedOut  = 1 << 6;
const TInt KSwtStyleShadowNone       = 1 << 5;
const TInt KSwtStyleIndeterminate    = 1 << 1;
const TInt KSwtStyleTool             = 1 << 2;
const TInt KSwtStyleNoTrim           = 1 << 3;
const TInt KSwtStyleResize           = 1 << 4;
const TInt KSwtStyleTitle            = 1 << 5;
const TInt KSwtStyleClose            = 1 << 6;
const TInt KSwtStyleMenu             = KSwtStyleClose;
const TInt KSwtStyleMin              = 1 << 7;
const TInt KSwtStyleMax              = 1 << 10;
const TInt KSwtStyleHScroll          = 1 << 8;
const TInt KSwtStyleVScroll          = 1 << 9;
const TInt KSwtStyleBorder           = 1 << 11;
const TInt KSwtStyleClipChildren     = 1 << 12;
const TInt KSwtStyleClipSiblings     = 1 << 13;
const TInt KSwtStyleOnTop            = 1 << 14;
const TInt KSwtStyleModeless         = 0;
const TInt KSwtStylePrimaryModal     = 1 << 15;
const TInt KSwtStyleApplicationModal = 1 << 16;
const TInt KSwtStyleSystemModal      = 1 << 17;
const TInt KSwtStyleHideSelection    = 1 << 15;
const TInt KSwtStyleFullSelection    = 1 << 16;
const TInt KSwtStyleFlat             = 1 << 23;
const TInt KSwtStyleSmooth           = 1 << 16;
const TInt KSwtStyleNoBackground     = 1 << 18;
const TInt KSwtStyleNoFocus          = 1 << 19;
const TInt KSwtStyleNoRedrawResize   = 1 << 20;
const TInt KSwtStyleNoMergePaints    = 1 << 21;
const TInt KSwtStyleNoRadioGroup     = 1 << 22;
const TInt KSwtStyleLeftToRight      = 1 << 25;
const TInt KSwtStyleRightToLeft      = 1 << 26;
const TInt KSwtStyleMirrored         = 1 << 27;
const TInt KSwtStyleEmbedded         = 1 << 24;
const TInt KSwtStyleVirtual          = 1 << 28;
const TInt KSwtStyleUp               = 1 << 7;
const TInt KSwtStyleDown             = 1 << 10;
const TInt KSwtStyleLead             = 1 << 14;
const TInt KSwtStyleTrail            = 1 << 17;
const TInt KSwtStyleCenter           = 1 << 24;
const TInt KSwtStyleHorizontal       = 1 << 8;
const TInt KSwtStyleVertical         = 1 << 9;
const TInt KSwtStyleBeginning        = 1;
const TInt KSwtStyleFill             = 4;
const TInt KSwtStyleOpen             = 1 << 12;
const TInt KSwtStyleSave             = 1 << 13;


// Although the styles KSwtStyleShellTrim and KSwtStyleDialogTrim use
// the KSwtStyleNormalStatusPane in Mobile Extension, we still do not bitwise OR
// them with KSwtStyleNormalStatusPane, to keep the compatibility when Mobile
// Extensions is not used.
const TInt KSwtStyleShellTrim        = KSwtStyleTitle | KSwtStyleClose | KSwtStyleMin | KSwtStyleMax | KSwtStyleResize;
const TInt KSwtStyleDialogTrim       = KSwtStyleTitle | KSwtStyleClose | KSwtStyleBorder;


/**
 * Binary mask for the modality styles
 */
const TInt KSwtModalityMask = KSwtStylePrimaryModal | KSwtStyleApplicationModal | KSwtStyleSystemModal;


/**
 * Binary mask for the horizontal alignment styles
 */
const TInt KSwtAlignmentMask = KSwtStyleLead | KSwtStyleCenter | KSwtStyleTrail;


/**
 * Binary mask for the orientation styles
 */
const TInt KSwtOrientationMask = KSwtStyleLeftToRight | KSwtStyleRightToLeft;


/**
 * Type Ids of data contains in the clipboard
 */
const TInt KSwtClipboardTypeByte       = 1;
const TInt KSwtClipboardTypePlaintText = 2;


/*
 * Message box styles
 */
const TInt KSwtMsgBoxStyleError       = 1;       // Message box style for error icon behaviour
const TInt KSwtMsgBoxStyleInformation = 1 <<  1; // Message box style for an information icon
const TInt KSwtMsgBoxStyleQuestion    = 1 <<  2; // Message box style for a question icon
const TInt KSwtMsgBoxStyleWarning     = 1 <<  3; // Message box style for a warning icon
const TInt KSwtMsgBoxStyleWorking     = 1 <<  4; // Message box style for a "working" icon
const TInt KSwtMsgBoxStyleOk          = 1 <<  5; // Message box style for an OK button
const TInt KSwtMsgBoxStyleYes         = 1 <<  6; // Message box style for a YES button
const TInt KSwtMsgBoxStyleNo          = 1 <<  7; // Message box style for a NO button
const TInt KSwtMsgBoxStyleCancel      = 1 <<  8; // Message box style for a CANCEL button
const TInt KSwtMsgBoxStyleRetry       = 1 << 10; // Message box style for a RETRY button


/**
 * Binary mask for a message box's buttons
 */
const TInt KSwtMsgBoxButtonMask = KSwtMsgBoxStyleOk | KSwtMsgBoxStyleYes | KSwtMsgBoxStyleNo | KSwtMsgBoxStyleCancel | KSwtMsgBoxStyleRetry;


/**
 * Pre-combined message box buttons (the only legal combinations)
 */
enum TMsgBoxButtons
{
    ESwtMsgBoxBtnYesNo       = KSwtMsgBoxStyleYes   | KSwtMsgBoxStyleNo,
    ESwtMsgBoxBtnYesNoCancel = KSwtMsgBoxStyleYes   | KSwtMsgBoxStyleNo | KSwtMsgBoxStyleCancel,
    ESwtMsgBoxBtnOk          = KSwtMsgBoxStyleOk,
    ESwtMsgBoxBtnOkCancel    = KSwtMsgBoxStyleOk    | KSwtMsgBoxStyleCancel,
    ESwtMsgBoxBtnRetryCancel = KSwtMsgBoxStyleRetry | KSwtMsgBoxStyleCancel
};


/**
 * Default value of the ProgressBar
 */
const TInt KDefaultProgressBarSelectionValue= 0;
const TInt KDefaultProgressBarMinValue      = 0;
const TInt KDefaultProgressBarMaxValue      = 100;


/**
 * Retrieves the message box's button from it's style
 */
inline TMsgBoxButtons SwtMsgBoxButtons(TInt aMsgBoxStyle)
{
    return static_cast<TMsgBoxButtons>(aMsgBoxStyle & KSwtMsgBoxButtonMask);
}


/**
 * Greatest number of pages in a CSwtMultiPageDialog
 */
const TInt KMultiPageDialogNbPageMax = 5;


/**
 * SWT error code
 */
enum TSwtError
{
    ESwtErrorNone                =  0,
    ESwtErrorUnspecified         =  1,
    ESwtErrorNoHandles           =  2,
    ESwtErrorNoMoreCallbacks     =  3,
    ESwtErrorNullArgument        =  4,
    ESwtErrorInvalidArgument     =  5,
    ESwtErrorInvalidRange        =  6,
    ESwtErrorCannotBeZero        =  7,
    ESwtErrorCannotGetItem       =  8,
    ESwtErrorCannotGetSelection  =  9,
    ESwtErrorCannotGetItemHeight = 11,
    ESwtErrorCannotGetText       = 12,
    ESwtErrorCannotSetText       = 13,
    ESwtErrorItemNotAdded        = 14,
    ESwtErrorItemNotRemoved      = 15,
    ESwtErrorNotImplemented      = 20,
    ESwtErrorMenuNotDropDown     = 21,
    ESwtErrorThreadInvalidAccess = 22,
    ESwtErrorWidgetDisposed      = 24,
    ESwtErrorMenuItemNotCascade  = 27,
    ESwtErrorCannotSetSelection  = 28,
    ESwtErrorCannotSetMenu       = 29,
    ESwtErrorCannotSetEnabled    = 30,
    ESwtErrorCannotGetEnabled    = 31,
    ESwtErrorInvalidParent       = 32,
    ESwtErrorMenuNotBar          = 33,
    ESwtErrorCannotGetCount      = 36,
    ESwtErrorMenuNotPopUp        = 37,
    ESwtErrorUnsupportedDepth    = 38,
    ESwtErrorIO                  = 39,
    ESwtErrorInvalidImage        = 40,
    ESwtErrorUnsupportedFormat   = 42,
    ESwtErrorInvalidSubclass     = 43,
    ESwtErrorGraphicDisposed     = 44,
    ESwtErrorDeviceDisposed      = 45,
    ESwtErrorFailedExec          = 46,
    ESwtErrorFailedLoadLibrary   = 47
};


/*
 * SWT font style
 */
const TInt KSwtNormal = 0;
const TInt KSwtBold   = 1 << 0;
const TInt KSwtItalic = 1 << 1;


/**
 * SWT event type
 */
enum TSwtEventType
{
    // SWT events (constant values specifed in eSWT API)
    ESwtEventNone             =  0,
    ESwtEventKeyDown          =  1,
    ESwtEventKeyUp            =  2,
    ESwtEventMouseDown        =  3,
    ESwtEventMouseUp          =  4,
    ESwtEventMouseMove        =  5,
    ESwtEventMouseEnter       =  6,
    ESwtEventMouseExit        =  7,
    ESwtEventMouseDoubleClick =  8,
    ESwtEventPaint            =  9,
    ESwtEventMove             = 10,
    ESwtEventResize           = 11,
    ESwtEventDispose          = 12,
    ESwtEventSelection        = 13,
    ESwtEventDefaultSelection = 14,
    ESwtEventFocusIn          = 15,
    ESwtEventFocusOut         = 16,
    ESwtEventExpand           = 17,
    ESwtEventCollapse         = 18,
    ESwtEventIconify          = 19,
    ESwtEventDeiconify        = 20,
    ESwtEventClose            = 21,
    ESwtEventShow             = 22,
    ESwtEventHide             = 23,
    ESwtEventModify           = 24,
    ESwtEventVerify           = 25,
    ESwtEventActivate         = 26,
    ESwtEventDeactivate       = 27,
    ESwtEventHelp             = 28,
    ESwtEventDragDetect       = 29,
    ESwtEventArm              = 30,
    ESwtEventTraverse         = 31,
    ESwtEventMouseHover       = 32,
    ESwtEventHardKeyDown      = 33,
    ESwtEventHardKeyUp        = 34,
    ESwtEventMenuDetect       = 35,
    ESwtEventSetData          = 36,
    ESwtEventSettings         = 39, //added in eSWT 1.1
    // Custom events
    ESwtEventLocation         = 1000,
    ESwtEventProgress         = 1002,
    ESwtEventStatusText       = 1003,
    ESwtEventTitle            = 1004,
    ESwtEventExit             = 1005,
    ESwtEventForeground       = 1006,
    ESwtEventDialogStringResult = 1007,
    ESwtEventDialogIntegerResult = 1008,
    ESwtEventDialogRgbResult  = 1009,
    ESwtEventShowFocusedControl = 1010
};


/**
 * Traversal detail
 */
enum TSwtTraversal
{
    ESwtTraverseNone          = 0,
    ESwtTraverseEscape        = 1 << 1,
    ESwtTraverseReturn        = 1 << 2,
    ESwtTraverseTabPrevious   = 1 << 3,
    ESwtTraverseTabNext       = 1 << 4,
    ESwtTraverseArrowPrevious = 1 << 5,
    ESwtTraverseArrowNext     = 1 << 6,
    ESwtTraverseMnemonic      = 1 << 7,
    ESwtTraversePagePrevious  = 1 << 8,
    ESwtTraversePageNext      = 1 << 9,
    ESwtTraverseParent        = 1 << 10 // native internal traversal type
};
enum TSwtBrCallBackOperationType
{
    ESwtBrDoNothing     = 0,
    ESwtBrLoadUrl       = 1 << 1,
    ESwtBrGoBack        = 1 << 2,
    ESwtBrGoForward     = 1 << 3
};


/*
 * SWT IME constants
 */
const TInt KSwtImeDbcs     = 1 << 1; // Input Method Editor style constant for double byte input behaviour.
const TInt KSwtImeAlpha    = 1 << 2; // Input Method Editor style constant for alpha input behaviour.
const TInt KSwtImeNative   = 1 << 3; // Input Method Editor style constant for native input behaviour.
const TInt KSwtImePhonetic = 1 << 4; // Input Method Editor style constant for phonetic input behaviour.
const TInt KSwtImeRoman    = 1 << 5; // Input Method Editor style constant for romanicised input behaviour.


/*
 * SWT key modifiers
 */
const TInt KSwtModifierAlt     = 1 << 16;
const TInt KSwtModifierShift   = 1 << 17;
const TInt KSwtModifierControl = 1 << 18;
const TInt KSwtModifierCommand = 1 << 22;
const TInt KSwtModifierMask    = KSwtModifierAlt | KSwtModifierShift | KSwtModifierControl | KSwtModifierCommand;


/*
 * Mouse buttons
 */
const TInt KSwtButton1    = 1 << 19;
const TInt KSwtButton2    = 1 << 20;
const TInt KSwtButton3    = 1 << 21;
const TInt KSwtButtonMask = KSwtButton1 | KSwtButton2 | KSwtButton3;

const TInt KSwtMouseButton1 = 1;
const TInt KSwtMouseButton2 = 2;
const TInt KSwtMouseButton3 = 3;


/**
 * A constant known to be zero (0), used in operations which take bit
 *        flags to indicate that "no bits are set".
 */
const TInt KSwtNone = 0;


/**
 * Indicates that a user-interface component is being dragged, for
 *        example dragging the thumb of a scroll bar.
 */
const TInt KSwtDrag = 1;


/**
 * Indicates that a default should be used.
 */
const TInt KSwtDefault = -1;


/**
 * Accelerator constant used to differentiate a key code from a Unicode
 *        character.
 */
const TInt KSwtKeycodeBit = (1 << 24);


/**
 * Accelerator constant used to extract the key stroke portion of an
 *        accelerator.
 */
const TInt KSwtKeyMask = KSwtKeycodeBit + 0xFFFF;


/**
 * constant used to to differentiate  symbian scancode from to a Unicode
 * character.
 */
const TInt KSwtScanCodeBase = (1 << 25);





/**
 * SWT key code
 */
enum TSwtKey
{
    ESwtKeyArrowUp        = KSwtKeycodeBit +  1,
    ESwtKeyArrowDown      = KSwtKeycodeBit +  2,
    ESwtKeyArrowLeft      = KSwtKeycodeBit +  3,
    ESwtKeyArrowRight     = KSwtKeycodeBit +  4,
    ESwtKeyPageUp         = KSwtKeycodeBit +  5,
    ESwtKeyPageDown       = KSwtKeycodeBit +  6,
    ESwtKeyHome           = KSwtKeycodeBit +  7,
    ESwtKeyEnd            = KSwtKeycodeBit +  8,
    ESwtKeyInsert         = KSwtKeycodeBit +  9,
    ESwtKeyF1             = KSwtKeycodeBit + 10,
    ESwtKeyF2             = KSwtKeycodeBit + 11,
    ESwtKeyF3             = KSwtKeycodeBit + 12,
    ESwtKeyF4             = KSwtKeycodeBit + 13,
    ESwtKeyF5             = KSwtKeycodeBit + 14,
    ESwtKeyF6             = KSwtKeycodeBit + 15,
    ESwtKeyF7             = KSwtKeycodeBit + 16,
    ESwtKeyF8             = KSwtKeycodeBit + 17,
    ESwtKeyF9             = KSwtKeycodeBit + 18,
    ESwtKeyF10            = KSwtKeycodeBit + 19,
    ESwtKeyF11            = KSwtKeycodeBit + 20,
    ESwtKeyF12            = KSwtKeycodeBit + 21,
    ESwtKeyF13            = KSwtKeycodeBit + 22,
    ESwtKeyF14            = KSwtKeycodeBit + 23,
    ESwtKeyF15            = KSwtKeycodeBit + 24,
    ESwtKeyKeypadMultiply = KSwtKeycodeBit + 42,
    ESwtKeyKeypadAdd      = KSwtKeycodeBit + 43,
    ESwtKeyKeypadSubtract = KSwtKeycodeBit + 45,
    ESwtKeyKeypadDecimal  = KSwtKeycodeBit + 46,
    ESwtKeyKeypadDivide   = KSwtKeycodeBit + 47,
    ESwtKeyKeypad0        = KSwtKeycodeBit + 48,
    ESwtKeyKeypad1        = KSwtKeycodeBit + 49,
    ESwtKeyKeypad2        = KSwtKeycodeBit + 50,
    ESwtKeyKeypad3        = KSwtKeycodeBit + 51,
    ESwtKeyKeypad4        = KSwtKeycodeBit + 52,
    ESwtKeyKeypad5        = KSwtKeycodeBit + 53,
    ESwtKeyKeypad6        = KSwtKeycodeBit + 54,
    ESwtKeyKeypad7        = KSwtKeycodeBit + 55,
    ESwtKeyKeypad8        = KSwtKeycodeBit + 56,
    ESwtKeyKeypad9        = KSwtKeycodeBit + 57,
    ESwtKeyKeypadEqual    = KSwtKeycodeBit + 61,
    ESwtKeyKeypadCR       = KSwtKeycodeBit + 80,
    ESwtKeyHelp           = KSwtKeycodeBit + 81,
    ESwtKeyCapsLock       = KSwtKeycodeBit + 82,
    ESwtKeyNumLock        = KSwtKeycodeBit + 83,
    ESwtKeyScrollLock     = KSwtKeycodeBit + 84,
    ESwtKeyPause          = KSwtKeycodeBit + 85,
    ESwtKeyBreak          = KSwtKeycodeBit + 86,
    ESwtKeyPrintScreen    = KSwtKeycodeBit + 87
};


/**
 * Structure containing the needed data for an SWT key event
 */
struct TSwtKeyEventData
{
    TSwtEventType iType;
    TChar         iCharacter;
    TInt          iKeyCode;
    TInt          iStateMask;
};


/**
 * SWT Line style
 */
enum TSwtLineStyle
{
    ESwtLineSolid      = 1, // Line drawing style for solid lines
    ESwtLineDash       = 2, // Line drawing style for dashed lines
    ESwtLineDot        = 3, // Line drawing style for dotted lines
    ESwtLineDashDot    = 4, // Line drawing style for alternating dash-dot lines
    ESwtLineDashDotDot = 5  // Line drawing style for dash-dot-dot lines
};


/*
 * Draw constants
 */
const TInt KSwtDrawTransparent = 1 << 0; // Indicates whether the drawing operation should fill the background.
const TInt KSwtDrawDelimiter   = 1 << 1; // Indicates whether the string drawing operation should handle line-delimeters.
const TInt KSwtDrawTab         = 1 << 2; // Indicates whether the string drawing operation should expand TAB characters.
const TInt KSwtDrawMnemonic    = 1 << 3; // Indicates whether the string drawing operation should handle mnemonics.


/**
 * SWT image type
 */
enum TSwtImageType
{
    ESwtImageBmp    = 0, // Indicates a Windows BMP format image.
    ESwtImageBmpRle = 1, // Indicates a run-length encoded Windows BMP format image.
    ESwtImageGif    = 2, // Indicates a GIF format image.
    ESwtImageIco    = 3, // Indicates a ICO format image.
    ESwtImageJpeg   = 4, // Indicates a JPEG format image.
    ESwtImagePng    = 5, // Indicates a PNG format image.
    ESwtImageTiff   = 6  // Indicates a TIFF format image.
};

enum TSwtImageLayoutType
{
    ESwtListImage = 1,
    ESwtChoiceImage,
    ESwtAlertImage
};

/**
 * GIF image disposal method
 */
enum TSwtGifDisposal
{
    KSwtDisposalUnspecified    = 0x0, // Indicates that the disposal method is unspecified.
    KSwtDisposalFillNone       = 0x1, // Indicates that the disposal method is to do nothing.
    KSwtDisposalFillBackground = 0x2, // Indicates that the previous images should be covered with the background colour before displaying the next image.
    KSwtDisposalFillPrevious   = 0x3  // Indicates that the disposal method is to restore the previous picture
};


/**
 * System colour ID
 */
enum TSwtColorId
{
    // Default colours
    ESwtColorWhite       =  1, // Default colour white.
    ESwtColorBlack       =  2, // Default colour black.
    ESwtColorRed         =  3, // Default colour red.
    ESwtColorDarkRed     =  4, // Default colour dark red.
    ESwtColorGreen       =  5, // Default colour green.
    ESwtColorDarkGreen   =  6, // Default colour dark green.
    ESwtColorYellow      =  7, // Default colour yellow.
    ESwtColorDarkYellow  =  8, // Default colour dark yellow.
    ESwtColorBlue        =  9, // Default colour blue.
    ESwtColorDarkBlue    = 10, // Default colour dark blue.
    ESwtColorMagenta     = 11, // Default colour magenta.
    ESwtColorDarkMagenta = 12, // Default colour dark magenta.
    ESwtColorCyan        = 13, // Default colour cyan.
    ESwtColorDarkCyan    = 14, // Default colour dark cyan.
    ESwtColorGray        = 15, // Default colour gray.
    ESwtColorDarkGray    = 16, // Default colour dark gray.

    // System colours
    ESwtColorWidgetDarkShadow                = 17, // System colour used to paint dark shadow areas.
    ESwtColorWidgetNormalShadow              = 18, // System colour used to paint normal shadow areas.
    ESwtColorWidgetLightShadow               = 19, // System colour used to paint light shadow areas.
    ESwtColorWidgetHighlightShadow           = 20, // System colour used to paint highlight shadow areas.
    ESwtColorWidgetForeground                = 21, // System colour used to paint foreground areas.
    ESwtColorWidgetBackground                = 22, // System colour used to paint background areas.
    ESwtColorWidgetBorder                    = 23, // System colour used to paint border areas.
    ESwtColorListForeground                  = 24, // System colour used to paint list foreground areas.
    ESwtColorListBackground                  = 25, // System colour used to paint list background areas.
    ESwtColorListSelection                   = 26, // System colour used to paint list selection background areas.
    ESwtColorListSelectionText               = 27, // System colour used to paint list selected text.
    ESwtColorTitleForeground                 = 30, // System colour used to paint title text.
    ESwtColorTitleBackground                 = 31, // System colour used to paint title background areas.
    ESwtColorTitleBackgroundGradient         = 32, // System colour used to paint title background gradient.
    ESwtColorTitleInactiveForeground         = 33, // System colour used to paint inactive title text.
    ESwtColorTitleInactiveBackground         = 34, // System colour used to paint inactive title background areas.
    ESwtColorTitleInactiveBackgroundGradient = 35  // System colour used to paint inactive title background gradient.
};


/**
 * Orientation
 */
enum TSwtOrientation
{
    ESwtLeftToRight = KSwtStyleLeftToRight, // Left to right orientation
    ESwtRightToLeft = KSwtStyleRightToLeft  // Right to left orientation
};


/*
 * Image transparency type
 */
const TInt KSwtTransparencyNone  = 0x0;    // The image contains no transparency
const TInt KSwtTransparencyAlpha = 1 << 0; // The image contains alpha transparency information
const TInt KSwtTransparencyMask  = 1 << 1; // The image contains a transparency mask
const TInt KSwtTransparencyPixel = 1 << 2; // The image contains a transparent pixel


/**
 * Image Copy type
 */
enum TSwtImageCopyType
{
    /**
     * The Image constructor argument indicating that
     * the new image should be a copy of the image provided as
     * an argument  (value is 0).
     */
    ESwtImageCopy     = 0, // Simple image copy
    ESwtImageDisabled = 1, // The copy must look like the "disabled" original
    ESwtImageGray     = 2  // The copy must be a gray scaled version of the original
};


/**
 * Default width for a widget
 */
const TInt KSwtDefaultWidth  = 64;


/**
 * Default height for a widget
 */
const TInt KSwtDefaultHeight = 64;


/**
 * Window priorities. By default the CCoeControl window priority is 0.
 * The eSWT status pane must be always behind second level shells.
 * Second level shells must be always behind any other control.
 * ON_TOP second level shells must be always on top of any other
 * control and must never get faded inside the eSWT application.
 */
enum TSwtWinPriority
{
    ESwtWinPriorityInactiveTopShell = - 3,
    ESwtWinPriorityStatusPane = - 2,
    ESwtWinPriorityShell = -1,
    ESwtWinPriorityDefault = 0,
    ESwtWinPriorityMenu = 1
};


/**
 * Minimum distance between 2 lines of text. Used by Label and GC
 */
const TInt KSwtMinLinePadding = 1;


/**
 * Type of a pointer to a Java peer object
 */
typedef _jweak* TSwtPeer;


/**
 * Type of an array of Java peer objects
 */
typedef CArrayPtrFlat<_jweak> CSwtPeerArray;


/**
 * The window server events that the observer will receive.
 */
enum TSwtWsEvent
{
    SwtWsEventAppFocusGained,
    SwtWsEventAppFocusLost,
    SwtWsEventShutdown,
    SwtWsEventEndKeyPressed,
    SwtWsEventWindowVisibilityChanged,
    SwtWsEventUnknown
};


/**
 * Focus reasons
 */
const TInt KSwtFocusByApi = 0;
const TInt KSwtFocusByForce = 1;
const TInt KSwtFocusByPointer = 2;
const TInt KSwtFocusByKey = 3;


/**
 * eSWT specific physics action. They do not match native ones.
 */
const TInt KSwtPhysicsNoAction = 0;
const TInt KSwtPhysicsGrabbing = 1;
const TInt KSwtPhysicsPanning = 2;
const TInt KSwtPhysicsFlicking = 3;


/**
 * Interface class for resource change observers.
 */
class MSwtResourceChangeObserver
{
public:
    /**
     * Offer the resource change.
     * @param aType The type of resources that have changed.
     */
    virtual void OfferResourceChangeL(TInt aType) =0;
};


/**
 * Interface class for app focus change observers.
 */
class MSwtAppFocusObserver
{
public:
    /**
     * @param aFocused ETrue when the application is becoming focused.
     */
    virtual void HandleAppFocusChangeL(TBool aFocused) =0;
};


/**
 * Popup menu closed callback.
 */
class MSwtPopupMenuCallBack
{
public:
    virtual void HandlePopupMenuClosedL() =0;
};


/**
 * Interface class implemented by display for being notified of events received by AppUi.
 */
class MSwtAppUiObserver
{
public:
    /**
     * Offers the command received in HandleCommandL.
     * @param aCommand The AppUi command.
     */
    virtual void HandleCommandL(TInt aCommand) =0;

    /**
     * Forwards HandleResourceChangeL.
     * @param aType The type of resources that have changed.
     */
    virtual void HandleResourceChangeL(TInt aType) =0;
};


/**
 * Interface class for the org.eclipse.swt.widgets.Display utility class
 *
 * The following convention is used: event methods named SendXyz()
 * are <b>synchronous</b>, whereas methods named PostXyz() are
 * <b>asynchronous</b>.
 */
class MSwtDisplay
{
protected:
    inline virtual ~MSwtDisplay() {} // Made protected to prevent destruction through the interface

private:
    MSwtDisplay& operator=(const MSwtDisplay& aDisplay); // Made private to prevent copy

public:
    /**
     * Returns the device associated with this display.
     */
    virtual MSwtDevice& Device() =0;

    /**
     * Retrieves a reference to the Core Widgets factory.
     * This method must be useable by both threads.
     */
    virtual const MSwtFactory& Factory() const =0;

    /**
     * Returns the UI utility object.
     */
    virtual MSwtUiUtils& UiUtils() =0;

    /**
     * Returns the Menu arranger.
     */
    virtual MSwtMenuArranger& MenuArranger() =0;

    /**
     * Returns the Command arranger (may be NULL).
     */
    virtual MSwtCommandArranger* CommandArranger() =0;

    /**
     * Registers a resource change observer.
     */
    virtual void AddResourceChangeObserverL(MSwtResourceChangeObserver* aObserver) = 0;

    /**
     * Unregisters a resource change observer.
     */
    virtual void RemoveResourceChangeObserver(MSwtResourceChangeObserver* aObserver) = 0;

    /**
     * Registers app focus observer.
     * MSwtUiUtils and MSwtShell instances are already informed separately so don't add them.
     */
    virtual void AddAppFocusObserverL(MSwtAppFocusObserver* aObserver) = 0;

    /**
     * Unregisters app focus observer.
     */
    virtual void RemoveAppFocusObserver(MSwtAppFocusObserver* aObserver) = 0;

    /**
     * Posts an asynchronous traverse event without taking into account any key event.
     *
     * @param aControl  The Control the event originates from
     * @param aDetail   The traversal detail
     * @param aDoIt     Initial value for the doit field
     */
    virtual void ForceTraverseEventL(MSwtControl& aControl,
                                     TSwtTraversal aDetail, TBool aDoIt) =0;

    /**
     * Posts an asynchronous traverse event
     *
     * @param aControl  The Control the event originates from
     * @param aDetail   The traversal detail
     * @param aKeyEvent The key event's data
     * @param aType     The key event's type
     * @param aDoIt     Initial value for the doit field
     */
    virtual void PostTraverseEventL(MSwtControl& aControl, TSwtTraversal aDetail,
                                    const TKeyEvent& aKeyEvent, TEventCode aType, TBool aDoIt) =0;

    /**
     * Posts an asynchronous keyboard event
     * @param aControl  The Control the event originates from
     * @param aKeyEvent The key event's data
     * @param aType     The key event's type
     */
    virtual void PostKeyEventL(MSwtControl& aControl,
                               const TKeyEvent& aKeyEvent, TEventCode aType) =0;

    /**
     * Posts an asynchronous paint event.
     * @param aPeer      The Java object to send the event to
     * @param aRect      The area to be painted
     * @param aMergeable Whether this event can be merged with another paint
     *                   event for optimisation purposes
     */
    virtual void PostPaintEventL(TSwtPeer aSenderPeer, TSwtPeer aShellPeer,
                                 const TRect& aRect, TBool aMergeable) =0;

    /**
     * Posts an asynchronous keyboard event
     * @param aPeer      The Java object to send the event to
     * @param aType      The event type, must be one of ESwtEventMouseDown,
     *                   ESwtEventMouseUp, ESwtEventMouseMove,
     *                   ESwtEventMouseEnter, ESwtEventMouseExit
     *                   or ESwtEventMouseDoubleClick
     * @param aButton    The pressed or relased button, must be one of KSwtMouseButton1,
     *                   KSwtMouseButton2 or KSwtMouseButton3.
     * @param aPos       The mouse's position of client coordinates.
     * @param aStateMask The keyboard state mask
     */
    virtual void PostMouseEventL(TSwtPeer aPeer, TSwtEventType aType,
                                 TInt aButton, const TPoint& aPos, TInt aStateMask) =0;

    /**
     * Posts an asynchronous move event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostMoveEventL(TSwtPeer aPeer) =0;

    /**
     * Posts an asynchronous resize event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostResizeEventL(TSwtPeer aPeer) =0;

    /**
     * Posts an asynchronous scroll event.
     * @param aPeer   The Java object to send the event to
     * @param aDetail The value to place in the detail field of
     *                the Java Event class. Must be one of
     *                ESwtKeyArrowUp, ESwtKeyArrowDown,
     *                ESwtKeyPageUp,  ESwtKeyPageDown,
     *                ESwtKeyHome,    ESwtKeyEnd,
     *                KSwtDrag or 0.
     */
    virtual void PostScrollEventL(TSwtPeer aPeer, TInt aDetail) =0;

    /**
     * Posts an asynchronous selection event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostSelectionEventL(TSwtPeer aPeer) =0;

    /**
     * Posts an asynchronous selection event.
     * @param aPeer The Java object to send the event to
     * @param aText Text selected
     */
    virtual void PostSelectionEventL(TSwtPeer aPeer, const TDesC& aText) = 0;

    /**
     * Posts an asynchronous selection event.
     * @param aPeer     The Java object to send the event to
     * @param aDetail   The value to place in the detail field of
     *                  the Java Event class. Must be one of
     *                  ESwtStyleCheck or 0.
     * @param aItemPeer The item selected
     */
    virtual void PostSelectionEventL(TSwtPeer aPeer,
                                     TInt aDetail, TSwtPeer aItemPeer) =0;

    /**
     * Posts an asynchronous default-selection event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostDefaultSelectionEventL(TSwtPeer aPeer) =0;

    /**
     * Posts an asynchronous default-selection event.
     * @param aPeer     The Java object to send the event to
     * @param aDetail   The detail information about the event.
     * @param aItemPeer The item selected
     */
    virtual void PostDefaultSelectionEventL(TSwtPeer aPeer,
                                            TInt aDetail, TSwtPeer aItemPeer) =0;

    /**
     * Posts an asynchronous focus event.
     * @param aPeer The Java object to send the event to
     * @param aType The event type, must be one of ESwtEventFocusIn
     *              or ESwtEventFocusOut.
     */
    virtual void PostFocusEventL(TSwtPeer aPeer, TSwtEventType aType) =0;

    /**
     * Posts a shell event
     * @param aPeer The Java object to send the event to
     * @param aType The event type, must be one of ESwtEventActivated or
     *              ESwtEventDeactivated.
     */
    virtual void PostShellEventL(TSwtPeer aPeer, TSwtEventType aType) =0;

    /**
     * Posts a modify event
     * @param aPeer      The Java object to send the event to
     */
    virtual void PostModifyEventL(TSwtPeer aPeer) =0;

    /**
     * Posts a verify event
     * @param aWidget     The widget issuing the verify event
     * @param aPeer       The Java object to send the event to
     * @param aStart      The start of the range of text which is to be modified
     * @param aEnd        The end of the range of text which is to be modified
     * @param aText       The text that is to replace the specified range.
     */
    virtual void PostVerifyEventL(MSwtVerifyEventObserver& aWidget, TSwtPeer aPeer,
                                  TInt aStart, TInt aEnd, const TDesC& aText) =0;

    /**
     * Posts an asynchronous menu show event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostShowEventL(TSwtPeer aPeer) =0;

    /**
     * Posts an asynchronous menu hide event.
     * @param aPeer The Java object to send the event to
     */
    virtual void PostHideEventL(TSwtPeer aPeer) =0;

    /**
     * Posts a screen event
     * @param aPeer The Java object to send the event to
     * @param aType The event type.
    */
    virtual void PostScreenEventL(TSwtPeer aPeer, TInt aType) =0;

    /**
     * Posts a mobile device event
     * @param aPeer The Java object to send the event to
     * @param aEventType The event type.
     */
    virtual void PostMobileDeviceEventL(TSwtPeer aPeer, TInt aEventType) =0;

    /**
     * Posts an asynchronous selection event.
     * @param aPeer The Java object to send the event to
     * @param aType The event type, must be one of ESwtEventExpand
     *              or ESwtEventCollapse.
     * @param aItemHandle The native handle of the item
     */
    virtual void PostTreeEventL(TSwtPeer aPeer, TSwtEventType aType,
                                TInt aItemHandle) =0;

    /**
     * Posts a location changing event
     * @param aPeer The Java object to send the event to
     * @param aBrowser The browser which initiated location changing event
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop A flag indicating whether the location opens in the top frame or not.
     * @param aLocation Current location
     * @param aCallBackOperationType The loading operation type
     * @return void
     */
    virtual void PostLocationChangingEventL(TSwtPeer aPeer,
                                            MSwtBrowser& aBrowser, TBool aDoIt, TBool aTop, const TDesC& aLocation,
                                            TSwtBrCallBackOperationType aCallBackOperationType) = 0;

    /**
     * Posts a location changed event
     * @param aPeer The Java object to send the event to
     * @param aDoIt A flag indicating whether the location loading should be allowed
     * @param aTop A flag indicating whether the location opens in the top frame or not.
     * @param aLocation Current location
     * @return void
     */
    virtual void PostLocationChangedEventL(TSwtPeer aPeer, TBool aDoIt,
                                           TBool aTop, const TDesC& aLocation) = 0;

    /**
     * Posts a progress event
     * @param aPeer The Java object to send the event to
     * @param aCurrent The current value
     * @param aTotal The total value
     * @return void
     */
    virtual void PostProgressEventL(TSwtPeer aPeer,
                                    TInt aCurrent, TInt aTotal) = 0;

    /**
     * Posts a progress completed event
     * @param aPeer The Java object to send the event to
     * @param aCurrent The current value
     * @param aTotal The total value
     * @return void
     */
    virtual void PostProgressCompletedEventL(TSwtPeer aPeer,
            TInt aCurrent, TInt aTotal) = 0;

    /**
     * Posts a status text event
     * @param aPeer The Java object to send the event to
     * @param aStatusText The status text
     * @return void
     */
    virtual void PostStatusTextEventL(TSwtPeer aPeer,
                                      const TDesC& aStatusText) = 0;

    /**
     * Posts a title event
     * @param aPeer The Java object to send the event to
     * @param aTitle The title of the current document
     * @return void
     */
    virtual void PostTitleEventL(TSwtPeer aPeer, const TDesC& aTitle) = 0;
    
    /**
     * Posts a show focused control to a ScrolledComposite.
     */
    virtual void PostShowFocusedControlEventL(TSwtPeer aPeer) =0;
    
    /**
     * Get eSWT Java application allocated id
     * @return Java application allocated id
     */
    virtual TInt ApplicationUid() = 0 ;

    /**
     * Converts an SWT key code to a Symbian scan code
     */
    virtual TInt GetSymbianScanCode(const TInt aSwtKeyCode) const =0;

    /**
     * Converts an SWT key code to a Symbian key code
     */
    virtual TUint GetSymbianKeyCode(const TInt aSwtKeyCode) const =0;

    /**
     * Change the name of the Task in the Task List.
     * @param The name for the eSwt task in the Task List.
     *        If NULL the task is no visible.
     */
    virtual void SetNameInTaskListL(const TDesC* aNewName) =0;

    /**
     * For eRCP
     * Change the UID of the task in the task list.
     * @param the new UID
     */
    virtual void SetUIDInTaskList(TInt aNewUID) =0;

    /**
     * For eRCP
     * Hides/unhides the windowgroup owned by this display.
     * @param the new visibility.
     */
    virtual void SetAppVisible(TBool aVisible) =0;

    /**
     * Set the only Mobile Device instance.
     */
    virtual void SetMobileDevice(MSwtMobileDevice* aMobileDevice) =0;

    /**
     * Returns the Mobile Device instance (may be NULL).
     */
    virtual MSwtMobileDevice* MobileDevice() =0;

    /**
     * Returns the shared object taking care of drawing the Java content for all controls.
     */
    virtual CSwtJavaBufferDrawer& JavaBufferDrawer() =0;

#ifdef RD_SCALABLE_UI_V2
    /**
     * This method should be called after a pointer event has been handled by a control.
     * This method forwards the pointer events to the avkon long tap detector.
     * @param aPointerEvent     The symbian pointer event
     */
    virtual void TryDetectLongTapL(const TPointerEvent& aPointerEvent) =0;

    /**
     * Return the control upon which the long tap animation has started or is about to start.
     */
    virtual MSwtControl* LongTapAnimationControl() const =0;

    /**
     * Cancel ongoing long tap animation. Call before the long tap control is disposed.
     */
    virtual void CancelLongTapAnimation() =0;
#endif // RD_SCALABLE_UI_V2

    /**
     * Checks if the UI has started to be displayed on screen.
     */
    virtual TBool IsUiReady() const =0;

    /**
     * Whatever UI component is getting ready to be displayed should notify the display.
     * @param aFullScreenUi Specifies if the starting ui is full screen.
     */
    virtual void SetUiReady(TBool aFullScreenUi) =0;

    /**
     * The coding conventions suggest that CCoeEnv::Static() should be avoided.
     * Therefore call this instead to get a stored reference.
     */
    virtual CEikonEnv* CoeEnv() const =0;

    /**
     * Returns names of all fonts according to aScalable parameter.
     */
    virtual CDesC16ArrayFlat* GetFontNamesL(TBool aScalable) const =0;

    /**
     * Check if the current pointer event being handled is
     * intended to revert the state of the grabbing control.
     */
    virtual TBool RevertPointerEvent() const =0;

    /**
     * Set the current pointer event being handled as
     * intended to revert the state of the grabbing control.
     */
    virtual void SetRevertPointerEvent(TBool aStatus) =0;
};

#endif // ESWT_H
