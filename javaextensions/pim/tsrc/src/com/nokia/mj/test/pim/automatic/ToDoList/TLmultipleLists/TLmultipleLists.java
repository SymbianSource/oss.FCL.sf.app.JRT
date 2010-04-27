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
* Description:
*
*/


//PACKAGE
package com.nokia.mj.test.pim.automatic.ToDoList.TLmultipleLists;
//IMPORTS
import javax.microedition.pim.PIM;
import com.nokia.mj.test.pim.automatic.*;
// CLASS DEFINITION
/**
 * See OpenMultipleLists.java.
 */
public class TLmultipleLists extends OpenMultipleLists
{

    public TLmultipleLists(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    protected int listType()
    {
        return PIM.TODO_LIST;
    }
}
