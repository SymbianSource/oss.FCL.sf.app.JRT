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

public class BenchmarkRunner implements Runnable
{
    private Thread myThread;
    private RmsBenchmarkMidlet myMIDlet;
    private boolean runFullTests;

    public static final String TESTNAMES = "RMS,,,,\n";
    public static final String TESTS =
        "10 byte record create,10 byte enumerated read,10 byte random read,create enums,10 byte record set,open,close,10 byte record delete" +
        ",,100 byte record create,100 byte enumerated read,100 byte random read,create enums,100 byte record set,open,close,100 byte record delete" +
        ",,1000 byte record create,1000 byte enumerated read,1000 byte random read,create enums,1000 byte record set,open,close,1000 byte record delete" +
        ",,10000 byte record create,10000 byte enumerated read,10000 byte random read,create enums,10000 byte record set,open,close,10000 byte record delete" +
        ",,100000 byte record create,100000 byte enumerated read,100000 byte random read,create enums,100000 byte record set,open,close,100000 byte record delete\n";

    public static StringBuffer SCORES;

    BenchmarkRunner(RmsBenchmarkMidlet aMIDlet)
    {
        myMIDlet = aMIDlet;
        SCORES = new StringBuffer();
    }

    public void start()
    {
        runFullTests = false;
        System.out.println("BenchmarkRunner.start()");
        myThread = new Thread(this);
        myThread.start();
    }

    public void startFull()
    {
        runFullTests = true;
        System.out.println("BenchmarkRunner.startFull()");
        myThread = new Thread(this);
        myThread.start();
    }

    private void getScores(Benchmark benchmark)
    {
        StringBuffer score = new StringBuffer();
        while (benchmark.getScore(score))
        {
            SCORES.append(score.toString());
            SCORES.append(',');
            score.delete(0, score.length());
        }
    }

    public void run()
    {
        System.out.println("BenchmarkRunner.run()");
        SCORES.delete(0, SCORES.length());

        Benchmark benchmark;

        // RMS BENCHMARK
        myMIDlet.updateForm("RMS...");
        benchmark = new RMSBenchmark(runFullTests);
        benchmark.runTest();
        getScores(benchmark);
        benchmark = null;
        System.gc();

        SCORES.append('\n');   // EOL
        Printer printer = new Printer();
        printer.print();

        myMIDlet.testsCompleted();
        System.out.println("BenchmarkRunner.run() completed");
    }

}
