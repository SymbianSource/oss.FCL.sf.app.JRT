/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SoundImpl instances can send events through this interface
*
*/



#ifndef MMIDEVENTSENDER_H
#define MMIDEVENTSENDER_H

/**
*  SoundImpl instances cand send events through this interface.
*  This interface has only one method, SendEvent(), which can be used
*  for sending all types of sound events.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(MMIDEventSender)
{

public: // New functions

    /**
    * Sends event to Java SoundListener.
    * @since 3.0
    * @param aEventType Type of the event to send
    */
    virtual void SendEvent(TInt aEventType) = 0;

protected:

    /**
    * C++ default constructor.
    */
    MMIDEventSender() { }

private:

    // Prohibit copy constructor if not deriving from CBase.
    MMIDEventSender(const MMIDEventSender&) { }
    // Prohibit assigment operator if not deriving from CBase.
    MMIDEventSender& operator=(const MMIDEventSender&)
    {
        return *this;
    }

};

#endif // MMIDEVENTSENDER_H

// End of File
