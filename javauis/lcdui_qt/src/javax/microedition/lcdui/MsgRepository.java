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
* Description:
*
*/
package javax.microedition.lcdui;

import org.eclipse.swt.SWT;
/**
 * Repository for text messages used in OpenLCDUI. String constants in this
 * class should begin with prefix "CLASSNAME_" where CLASSNAME is the name of
 * the class which is using the constant.
 */
final class MsgRepository
{
    
    // Command labels:
    
    // SCREEN command
    // for Options menu position
    public static final String COMMAND_LABEL_SCREEN =
        OpenLcduiLocalization.getMessage("opt_screen_cmd_select");

    // for context menu position        
    public static final String COMMAND_LABEL_SCREEN_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_screen_cmd_select");

    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_SCREEN_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_screen_cmd_select_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_SCREEN_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_screen_cmd_select_2");

    // BACK command
    // for Options menu positions
    public static final String COMMAND_LABEL_BACK =
        OpenLcduiLocalization.getMessage("opt_cmd_back");
    
    // for context menu position    
    public static final String COMMAND_LABEL_BACK_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_cmd_back");
        
    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_BACK_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_cmd_back_1");
    
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_BACK_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_cmd_back_2");
        
    // CANCEL command    
    // for Options menu position
    public static final String COMMAND_LABEL_CANCEL =
        SWT.getMessage("opt_cmd_cancel");
        
    // for context menu position
    public static final String COMMAND_LABEL_CANCEL_CTX_MENU =
        SWT.getMessage("menu_cmd_cancel");

    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_CANCEL_BUTTON_1 =
        SWT.getMessage("button_cmd_cancel_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_CANCEL_BUTTON_2 =
        SWT.getMessage("button_cmd_cancel_2");

    // OK command
    // for Options menu position
    public static final String COMMAND_LABEL_OK =
        SWT.getMessage("opt_cmd_ok");
        
    // for context menu position
    public static final String COMMAND_LABEL_OK_CTX_MENU =
        SWT.getMessage("menu_cmd_ok");
        
    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_OK_BUTTON_1 =
        SWT.getMessage("button_cmd_ok_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_OK_BUTTON_2 =
        SWT.getMessage("button_cmd_ok_2");

    // HELP command
    // for Options menu position
    public static final String COMMAND_LABEL_HELP =
        OpenLcduiLocalization.getMessage("opt_cmd_help");
        
    // for context menu position
    public static final String COMMAND_LABEL_HELP_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_cmd_help");
    
    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_HELP_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_cmd_help_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_HELP_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_cmd_help_2");

    // STOP command
    // for Options menu position
    public static final String COMMAND_LABEL_STOP =
        OpenLcduiLocalization.getMessage("opt_cmd_stop");
        
    // for context menu position
    public static final String COMMAND_LABEL_STOP_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_cmd_stop");
        
    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_STOP_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_cmd_stop_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_STOP_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_cmd_stop_2");
   
    // EXIT command
    // for Options menu position
    public static final String COMMAND_LABEL_EXIT =
        OpenLcduiLocalization.getMessage("opt_cmd_close");
        
    // for context menu position
    public static final String COMMAND_LABEL_EXIT_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_cmd_close");
        
    // for button position (one button/softkey only)    
    public static final String COMMAND_LABEL_EXIT_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_cmd_close_1");
        
    // for button position (two buttons/softkeys)
    public static final String COMMAND_LABEL_EXIT_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_cmd_close_2");
   
    // ITEM command
    // for Options menu position (Note: should not be used, as ITEM type commands 
    // should never be mapped to Options menu!)
    public static final String COMMAND_LABEL_ITEM =
        OpenLcduiLocalization.getMessage("opt_item_cmd_select");
        
    // for context menu position
    public static final String COMMAND_LABEL_ITEM_CTX_MENU =
        OpenLcduiLocalization.getMessage("menu_item_cmd_select");
        
    // for button position (one button/softkey only)
    public static final String COMMAND_LABEL_ITEM_BUTTON_1 =
        OpenLcduiLocalization.getMessage("button_item_cmd_select_1");
        
    // for button position (two buttons/softkeys) 
    public static final String COMMAND_LABEL_ITEM_BUTTON_2 =
        OpenLcduiLocalization.getMessage("button_item_cmd_select_2");
 
    // Alert texts:
    public static final String ALERT_DEFAULT_TEXT_ERROR =
        OpenLcduiLocalization.getMessage("info_alert_error");

    public static final String ALERT_DEFAULT_TEXT_WARNING =
        OpenLcduiLocalization.getMessage("info_alert_warning");

    public static final String ALERT_DEFAULT_TEXT_INFO =
        OpenLcduiLocalization.getMessage("info_alert_information");

    public static final String ALERT_DEFAULT_TEXT_CONFIRMATION =
        OpenLcduiLocalization.getMessage("info_alert_confirmation");

    public static final String ALERT_DEFAULT_TEXT_ALARM =
        OpenLcduiLocalization.getMessage("info_alert_alarm");
    
    // AlertType.NULL
    public static final String ALERT_DEFAULT_TEXT_ALERT =
        OpenLcduiLocalization.getMessage("info_alert");

    

    // Exceptions below - dont need translation !!!

    public static final String COMMAND_EXCEPTION_LABEL_IS_NULL =
        "Short label cannot be null";

    public static final String COMMAND_EXCEPTION_INVALID_TYPE =
        "Invalid command type";


    // Display:
    public static final String DISPLAY_EXCEPTION_MIDLET_IS_NULL =
        "Midlet cannot be null";

    public static final String DISPLAY_EXCEPTION_NEGATIVE_DURATION =
        "Duration cannot be negative.";

    public static final String DISPLAY_EXCEPTION_INVALID_COLOR_IDENTIFIER =
        "Color identifier is incorrect";

    public static final String DISPLAY_EXCEPTION_ALERT_IS_NULL =
        "Alert cannot be null";

    public static final String DISPLAY_EXCEPTION_NEXTDISP_IS_NULL =
        "Next displayable cannot be null";

    public static final String DISPLAY_EXCEPTION_NEXTDISP_IS_ALERT =
        "Next displayable cannot be an Alert";

    public static final String DISPLAY_EXCEPTION_ITEM_IS_NULL =
        "Item cannot be null.";

    public static final String DISPLAY_EXCEPTION_ITEM_HAS_NO_PARENT =
        "Item is not owned by a container.";


    // Displayable:
    public static final String DISPLAYABLE_EXCEPTION_NULL_PARAMETER =
        "Parameter was null";


    // Alert:
    public static final String ALERT_EXCEPTION_INVALID_TIMEOUT =
        "Invalid timeout value";

    public static final String ALERT_EXCEPTION_INVALID_INDICATOR =
        "Indicator violates restrictions";



    // AlertType:
    public static final String ALERTTYPE_EXCEPTION_PLAYSOUND_NULL_POINTER =
        "Parameter display cannot be null";


    // Textbox & TextField:
    public static final String TEXT_EXCEPTION_WRONG_TEXT =
        "Input txt doesn't meet constraints";

    public static final String TEXT_EXCEPTION_MAX_SIZE_EXCEEDED =
        "Text length exceeds maximum size";

    public static final String TEXT_EXCEPTION_INVALID_MAX_SIZE =
        "Maximum size is 0 or negative";

    public static final String TEXT_EXCEPTION_INVALID_CONSTRAINTS =
        "Invalid constraints";

    public static final String TEXT_EXCEPTION_TXT_IS_NULL =
        "Text must not be null";

    public static final String TEXT_EXCEPTION_ARRAY_IS_NULL =
        "Character data must not be null";

    public static final String TEXT_EXCEPTION_ARRAY_IS_TOO_SHORT =
        "Character data length is too short";

    public static final String TEXT_EXCEPTION_ARRAY_INDEX_OUT_OF_BOUNDS =
        "Offset and length do not specify a valid range";

    public static final String TEXT_EXCEPTION_INVALID_SIZE =
        "Invalid size";


    // Item:
    public static final String ITEM_EXCEPTION_INVALID_LAYOUT =
        "New layout value was not combination of layout directives";

    public static final String ITEM_EXCEPTION_NULL_COMMAND_ADDED =
        "Unable to add null-command to item";

    public static final String ITEM_EXCEPTION_INVALID_DIMENSION =
        "Width and height cannot be less than -1";

    public static final String ITEM_EXCEPTION_NOT_OWNED_BY_FORM =
        "Item is not owned by a form";

    public static final String ITEM_EXCEPTION_OWNED_BY_ALERT =
        "Not allowed when item is owned by an Alert";


    // StringItem:
    public static final String STRINGITEM_EXCEPTION_INVALID_APPMODE =
        "Invalid appearance mode";


    // ImageItem:
    public static final String IMAGEITEM_EXCEPTION_INVALID_APPMODE =
        "Invalid apperance mode";


    // Gauge:
    public static final String GAUGE_EXCEPTION_INVALID_VALUE =
        "Invalid value";

    public static final String GAUGE_EXCEPTION_INVALID_MAXVALUE =
        "Invalid maximum value";


    // List & ChoiceGroup
    public static final String LIST_EXCEPTION_INVALID_TYPE =
        "List type is invalid";

    public static final String CHOICEGROUP_EXCEPTION_INVALID_TYPE =
        "ChoiceGroup type is invalid";

    public static final String CHOICE_EXCEPTION_INVALID_FIT_POLICY =
        "Invalid fit policy";

    public static final String CHOICE_EXCEPTION_INVALID_ARRAY_LENGTHS =
        "Array lengths mismatch";

    public static final String CHOICE_EXCEPTION_INVALID_ITEM_INDEX =
        "Index of item is invalid";

    public static final String CHOICE_EXCEPTION_ITEM_NULL =
        "Null element with index";

    public static final String CHOICE_EXCEPTION_STRING_NULL =
        "Parameter text cannot be null";

    public static final String CHOICE_EXCEPTION_ARRAY_NULL =
        "Parameter array cannot be null";

    public static final String CHOICE_EXCEPTION_TEXT_ARRAY_NULL =
        "Parameter text array cannot be null";

    public static final String CHOICE_EXCEPTION_INVALID_ARRAY_SIZE =
        "Parameter array size must match choice size";


    // Spacer:
    public static final String SPACER_EXCEPTION_INVALID_STATE_LABEL =
        "Spacer cannot have a label";

    public static final String SPACER_EXCEPTION_INVALID_STATE_COMMAND =
        "Spacer cannot have a command";

    public static final String SPACER_EXCEPTION_INVALID_WIDTH_HEIGHT =
        "Width and height cannot be negative";


    // DateField:
    public static final String DATEFIELD_EXCEPTION_INVALID_MODE =
        "Invalid mode.";


    // Form:
    public static final String FORM_EXCEPTION_STRING_NULL_POINTER =
        "Parameter text cannot be null";

    public static final String FORM_EXCEPTION_IMAGE_NULL_POINTER =
        "Parameter img cannot be null";

    public static final String FORM_EXCEPTION_ITEM_NULL_POINTER =
        "Parameter item cannot be null";

    public static final String FORM_EXCEPTION_ITEM_OWNED_BY_CONTAINER =
        "Item is alread owned by container";

    public static final String FORM_EXCEPTION_INVALID_ITEM_INDEX =
        "Index of item is incorrect";


    // Font
    public static final String FONT_EXCEPTION_INVALID_SPECIFIER =
        "Invalid specifier parameter";

    public static final String FONT_EXCEPTION_INVALID_FACE =
        "Invalid face parameter";

    public static final String FONT_EXCEPTION_INVALID_STYLE =
        "Invalid style parameter";

    public static final String FONT_EXCEPTION_INVALID_SIZE =
        "Invalid size parameter";

    public static final String FONT_EXCEPTION_NULL_STRING =
        "String parameter is null";


    // Canvas
    public static final String CANVAS_EXCEPTION_INVALID_KEY_CODE =
        "Key code is invalid";

    public static final String CANVAS_EXCEPTION_INVALID_GAME_ACTION =
        "Game action is invalid";

    public static final String CANVAS_EXCEPTION_FRAMEBUFFER_NULL =
        "Framebuffer is null";


    // Graphics:
    public static final String GRAPHICS_EXCEPTION_ILLEGAL_STROKE_STYLE =
        "Stroke style is invalid";

    public static final String GRAPHICS_EXCEPTION_INVALID_ANCHOR =
        "Invalid anchor value";

    public static final String GRAPHICS_EXCEPTION_STRING_IS_NULL =
        "String is null";

    public static final String GRAPHICS_EXCEPTION_ARRAY_OUT_OF_BOUNDS =
        "Array bounds invalid";

    public static final String GRAPHICS_EXCEPTION_ARRAY_IS_NULL =
        "Char array is null";

    public static final String GRAPHICS_EXCEPTION_SAME_SOURCE_AND_DESTINATION =
        "Source and destination can not be the same";

    public static final String GRAPHICS_EXCEPTION_DESTINATION_IS_SCREEN =
        "Destination can not be screen device.";


    // DirectGraphics:
    public static final String DIRECTGRAPHICS_EXCEPTION_INVALID_MANIPULATION =
        "Invalid manipulation";

    public static final String DIRECTGRAPHICS_EXCEPTION_INVALID_FORMAT =
        "Invalid or unsupported format";

    public static final String DIRECTGRAPHICS_EXCEPTION_INVALID_WIDTH_HEIGHT =
        "Invalid width or height";
    // Image:
    public static final String IMAGE_EXCEPTION_FILE_NAME_IS_NULL =
        "File name specified is null";

    public static final String IMAGE_EXCEPTION_IO_ERROR =
        "I/O error loading image";

    public static final String IMAGE_EXCEPTION_IMMUTABLE =
        "Immutable image";

    public static final String IMAGE_EXCEPTION_FILE_STREAM_IS_NULL =
        "Stream specified is null";

    public static final String IMAGE_EXCEPTION_DATA_IS_NULL =
        "Image data is null";

    public static final String IMAGE_EXCEPTION_INVALID_BOUNDS =
        "Invalid image array bounds";

    public static final String IMAGE_EXCEPTION_IS_NULL =
        "Image is null";

    public static final String IMAGE_EXCEPTION_INVALID_DATA =
        "Image data is invalid";

    public static final String IMAGE_EXCEPTION_INVALID_DIMENSIONS =
        "Invalid image dimensions";

    public static final String IMAGE_EXCEPTION_INVALID_TRANSFORM =
        "Invalid transform value";

    public static final String IMAGE_EXCEPTION_INVALID_REGION =
        "Invalid image region specified";

    public static final String IMAGE_EXCEPTION_INVALID_SCANLENGTH =
        "Invalid length value";


    public static final String TICKER_EXCEPTION_NULL_STRING =
        "Text parameter cannot be null.";


    /**
     * Private Constructor.
     */
    private MsgRepository()
    {
    }
}
