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
* Description:  JSR-179 Location API ProximityListener class
 *
*/


// PACKAGE
package javax.microedition.location;

// CLASS DESCRIPTION
/**
 * J2ME JSR-179 Location API 1.0.1 complying ProximityListener class
 */
public interface ProximityListener
{
    public void proximityEvent(Coordinates coordinates, Location location);

    public void monitoringStateChanged(boolean isMonitoringActive);
}

// End of file
