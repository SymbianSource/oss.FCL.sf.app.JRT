/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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

import org.eclipse.swt.internal.extension.OS;

final class SystemPropertyUtil {
	
	 /**
     * Constant for Soft Key Label Height
     */
   public static final int LABEL_HEIGHT = 30;
   
   private static final int HbKeyboardNone = 0x00000000;
   private static final int HbKeyboard12Key = 0x00000001;
   private static final int HbKeyboardQwerty = 0x00000001|0x00010000; 
	
   /*
	* This method checks if the current Displayable exists
	* If yes, then the current Displayable is asked for the property
	* If no, the null is returned
	*/
   final static String getSoftKeyLabelLocationImpl(String softKeyId)
	{
	   String SoftKeyLabelLocation = null;

	   Displayable disp = javax.microedition.lcdui.Display.getDisplay().getCurrent();
	   
	   if((disp != null) && (disp instanceof Canvas))
	   {
		   if(((Canvas) disp).IsFullScreenMode())
		   {
			   
			   if("com.nokia.softkey1.label.location".equals(softKeyId))
			   {
				   SoftKeyLabelLocation = "0," + String.valueOf(((Canvas) disp).getHeight()) + ","
				   						+ String.valueOf((((Canvas) disp).getWidth())/2) + ","
				   						+ String.valueOf(LABEL_HEIGHT);
			   }
			   else if("com.nokia.softkey2.label.location".equals(softKeyId))
			   {
				   SoftKeyLabelLocation = String.valueOf((((Canvas) disp).getWidth())/2) + ","
				   		+ String.valueOf(((Canvas) disp).getHeight()) + ","
				   		+ String.valueOf((((Canvas) disp).getWidth())/2) + ","
				   		+ String.valueOf(LABEL_HEIGHT);
			   }
		   }
	   }
	   return SoftKeyLabelLocation;
	}

   /*
    * This method checks if the current Displayable exists
    * If yes, then the current Displayable is asked for the property
    * If no, the null is returned
    */
   final static String getSoftKeyLabelAnchorImpl(String softKeyId)
   {
	   String SoftKeyLabelAnchor = null;
	   
	   Displayable disp = javax.microedition.lcdui.Display.getDisplay().getCurrent();
	   
	   if((disp != null) && (disp instanceof Canvas))
	   {
		   if(null == ((Canvas) disp).getCanvasKeypad())
		   {
			   SoftKeyLabelAnchor = String.valueOf(Graphics.TOP | Graphics.HCENTER);
		   }
		   else
		   {
			   if("com.nokia.softkey1.label.anchor".equals(softKeyId))
			   {
				   SoftKeyLabelAnchor = String.valueOf(Graphics.TOP | Graphics.LEFT);
			   }
			   else if("com.nokia.softkey2.label.anchor".equals(softKeyId))
			   {
				   SoftKeyLabelAnchor = String.valueOf(Graphics.TOP | Graphics.RIGHT);
			   }
		   }
	   }

	   return SoftKeyLabelAnchor;
   }
	    
   final static String doGetDynamicProperty(String key)
   {
	   if("com.nokia.key.scancode".equals(key))
	   {
		   return String.valueOf(ESWTUIThreadRunner.getLastKeyScancode());
	   }
	   else if("com.nokia.key.modifier".equals(key))
	   {
		   return String.valueOf(ESWTUIThreadRunner.getLastKeyModifier());
	   }
	   else if("com.nokia.keyboard.type".equals(key))
	   {
		   // TODO: eSWT support required
		   /*
	            No keypad (for example a touch device without keypad)   - None
	            Standard ITU-T keypad (Phone keypad with 12 keys)       - PhoneKeypad
	            QWERTY (4x12 layout)                                    - FullKeyboard
	            QWERTY (limited, 4x10 layout)                           - LimitedKeyboard4x10
	            QWERTY (limited, 3x11 layout)                           - LimitedKeyboard3x11
	            Half QWERTY layout (aka Compact QWERTY keyboard).       - HalfKeyboard
	            Custom QWERTY layout                                    - Custom
	            Unknown layout                                          - Unknown
		    */
		   String keyBoard="None";
		   int type = OS.HbInputSettingProxy_availableHwKeyboard();
		   
		   switch(type){
		   case HbKeyboardNone:
			   keyBoard="None";
			   break;
		   case HbKeyboard12Key:
			   keyBoard="PhoneKeypad";
			   break;
		   case HbKeyboardQwerty:
			   keyBoard="FullKeyboard";
			   break;	
		   default:
			   keyBoard = "None";
		   }
		   return keyBoard;
	   }
	   else if("com.nokia.softkey1.label.location".equals(key))
	   {
		   return getSoftKeyLabelLocationImpl(key);
	   }
	   else if("com.nokia.softkey1.label.anchor".equals(key))
	   {
		   return getSoftKeyLabelAnchorImpl(key);
	   }
	   else if("com.nokia.softkey2.label.location".equals(key))
	   {
		   return getSoftKeyLabelLocationImpl(key);
	   }
	   else if("com.nokia.softkey2.label.anchor".equals(key))
	   {
		   return getSoftKeyLabelAnchorImpl(key);
	   }
	   return "key not supported";
   }
}
