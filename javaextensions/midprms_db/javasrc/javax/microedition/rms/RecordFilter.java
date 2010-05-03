/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RecordFilter
*
*/


package javax.microedition.rms;

/**
 * Record filter interface can be used to control which subset of
 * records should be included in enumeration.
 *
 * @see RecordEnumeration
 */
public interface RecordFilter
{
    /**
     * Returns true if the candidate matches the implemented criterion.
     *
     * @param aCandidate the record to consider.
     * @return true if the candidate matches the implemented criterion
     */
    public boolean matches(byte[] aCandidate);
}
