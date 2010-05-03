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
* Description:
*
*/

public interface Benchmark
{
    // Starts and runs the test
    public void runTest();

    // To be called as long as getScore() returns true.
    // If true, the StringBuffer score includes a test
    // score. Otherwise empty StringBuffer.
    public boolean getScore(StringBuffer score);
}
