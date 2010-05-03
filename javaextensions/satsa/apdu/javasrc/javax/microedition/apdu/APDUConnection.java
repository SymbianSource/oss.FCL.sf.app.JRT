/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for smart card communication
*
*/



package javax.microedition.apdu;

import java.io.IOException;
import javax.microedition.io.Connection;

/**
 * Refer JSR-177 Specification for details
 */
public interface APDUConnection
            extends Connection
{

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] changePin(int aPinID)
    throws IOException;

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] disablePin(int aPinID)
    throws IOException;

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] enablePin(int aPinID)
    throws IOException;

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] enterPin(int aPinID)
    throws IOException;

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] exchangeAPDU(byte[] aCommandAPDU)
    throws IOException;

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] getATR();

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract byte[] unblockPin(int aBlockedPinID, int aUnblockingPinID)
    throws IOException;
}
