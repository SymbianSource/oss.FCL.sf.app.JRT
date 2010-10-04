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

public class RMSBenchmark implements Benchmark
{
    private int NUMBER_OF_GETSCORE_REQUESTS = 0;
    private StringBuffer myScores[];

    private static final int[] quickTest = {100};
    private static final int[] fullTest = {10, 100, 1000, 10000, 100000};
    private int[] testSet;

    private int NUMBER_OF_TESTS = 12;
    private int NUMBER_OF_SCORES;// = NUMBER_OF_TESTS * testSet.length;

    RMSBenchmark(boolean aRunFullTests)
    {
        if (aRunFullTests)
        {
            testSet = new int[fullTest.length];
            System.arraycopy(fullTest, 0, testSet, 0, fullTest.length);
        }
        else
        {
            testSet = new int[quickTest.length];
            System.arraycopy(quickTest, 0, testSet, 0, quickTest.length);
        }
        NUMBER_OF_SCORES = NUMBER_OF_TESTS * testSet.length;

        myScores = new StringBuffer[NUMBER_OF_SCORES];
        for (int i = 0; i < myScores.length; i++)
        {
            myScores[i] = new StringBuffer();
        }
    }

    public void runTest()
    {
        System.out.println("RMSBenchmark.runTest()");

        for (int i = 0; i < testSet.length; i++)
        {
            System.out.println("loop start: "+i);
            Storage storage = new Storage(testSet[i]);
            myScores[i*NUMBER_OF_TESTS + 0].append(storage.createRecord());
            myScores[i*NUMBER_OF_TESTS + 1].append(storage.readEnumRecord());
            myScores[i*NUMBER_OF_TESTS + 2].append(storage.readRandomRecord());
            myScores[i*NUMBER_OF_TESTS + 3].append(storage.createEnum());
            myScores[i*NUMBER_OF_TESTS + 4].append(storage.setRecord());
            myScores[i*NUMBER_OF_TESTS + 5].append(storage.openStore());
            myScores[i*NUMBER_OF_TESTS + 6].append(storage.closeStore());
            myScores[i*NUMBER_OF_TESTS + 7].append(storage.deleteRecord());
            myScores[i*NUMBER_OF_TESTS + 8].append(storage.openMultipleStores());
            myScores[i*NUMBER_OF_TESTS + 9].append(storage.closeMultipleStores());
            myScores[i*NUMBER_OF_TESTS + 10].append(storage.openCloseMultipleStores());
            myScores[i*NUMBER_OF_TESTS + 11].append(""); // spacer
            storage.clean();
            System.out.println("loop done: "+i);
        }

        System.out.println("RMSBenchmark.runTest() completed");
    }

    public boolean getScore(StringBuffer score)
    {
        if (NUMBER_OF_GETSCORE_REQUESTS < NUMBER_OF_SCORES)
        {
            score.append(myScores[NUMBER_OF_GETSCORE_REQUESTS].toString());
            NUMBER_OF_GETSCORE_REQUESTS++;
            return true;
        }
        else
        {
            return false;
        }
    }
}
