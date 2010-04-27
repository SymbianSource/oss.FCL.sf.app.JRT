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

/**
* Please refer JSR 256 for more details.
*/
public interface Condition
{
    /**
     * Please refer JSR 256 for more details.
     */
    public static final String OP_EQUALS = "eq";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String OP_GREATER_THAN = "gt";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String OP_GREATER_THAN_OR_EQUALS = "ge";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String OP_LESS_THAN = "lt";

    /**
     * Please refer JSR 256 for more details.
     */
    public static final String OP_LESS_THAN_OR_EQUALS = "le";

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(double doubleValue);

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(Object value);



}