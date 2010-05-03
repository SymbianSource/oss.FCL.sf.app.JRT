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


final class OpenLcduiLocalization {
	
	public static String getMessage(String key) {
		
		 if (key == null) throw new IllegalArgumentException(
		            MsgRepository.TEXT_EXCEPTION_TXT_IS_NULL);
		
		if(key.equals("key_up")){
	   		return "Navigation key up";
	    }
	    
	    if(key.equals("key_down")){
	   		return "Navigation key down";
	    }
	    
	    if(key.equals("key_left")){
	    	return "Navigation key left";
	    }
	    
	    if(key.equals("key_right")){
	   		return "Navigation key right";
	    }
	    
	    if(key.equals("key_select")){
	   		return "Select";
	    }
	    
	    if(key.equals("key_stop")){
	    	return "Stop";
	    }
	  
	    if(key.equals("alert_error")){
	   		return "Error";
	    }
	     
	    if(key.equals("alert_warning")){
	   		return "Warning";
	    }
	     
	    if(key.equals("alert_info")){
	    	return "Information";
	    }
	     
	    if(key.equals("alert_conf")){
	    	return "OK";
	    }
	     
	    if(key.equals("alert_alarm")){
	   		return "Alarm";
	    }
	     
	    if(key.equals("alert_null")){
	   		return "Alert";
	    }
	     
	    if(key.equals("pb_fetch_number")){
	   		return "Add from Contacts";
	    }
	     
	    if(key.equals("pb_fetch_number_short")){
	   		return "Add";
	    }
	     
	    if(key.equals("pb_fetch_email")){
	    	return "Add from Contacts";
	    }
	      
	    if(key.equals("pb_fetch_email_short")){
	   		return "Add";
	    }
	      
	    if(key.equals("create_call")){
	   		return "Call";
	    }
	      
	    if(key.equals("create_call_short")){
	   		return "Call";
	    }
	    
	    if(key.equals("back")){
	   		return "Back";
	    }
	    
	    if(key.equals("close")){
	   		return "Close";
	    }
	    
	    if(key.equals("empty_list")){
	   		return "(no data)";
	    }
	    
	    if(key.equals("help")){
	   		return "Help";
	    }
        return key;
  	}
  
}