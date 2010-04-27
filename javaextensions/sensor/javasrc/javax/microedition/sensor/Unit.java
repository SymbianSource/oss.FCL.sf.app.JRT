/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.sensor;

import java.util.Hashtable;

/**
 * Please refer JSR 256 for more details.
 */
public class Unit
{

    private final String symbol;
    private static Hashtable units = new Hashtable();

    /**
     * Please refer JSR 256 for more details.
     */
    private Unit(String symbol)
    {
        if (symbol == null)
            throw new NullPointerException();
        this.symbol = symbol;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public static Unit getUnit(String symbol)
    {

        if (symbol == null)
            throw new NullPointerException("Given symbol was null");

        if (symbol.equals(""))
            throw new IllegalArgumentException(
                "Given symbol was an empty string");

        if (units.containsKey(symbol))
            return (Unit) units.get(symbol);

        Unit newUnit = new Unit(symbol);
        units.put(symbol, newUnit);
        return newUnit;

    }

    /**
     * Please refer JSR 256 for more details.
     */
    public String toString()
    {
        return symbol;
    }

}
