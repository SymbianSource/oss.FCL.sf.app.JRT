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
import java.util.Random;

public class Utils
{
    public Utils()
    {
    }

    public static void initRandom()
    {
        rand = new Random();
    }

    public static int random(int int1, int int2)
    {
        if (int1 == int2)
            return int1;
        int min = Math.min(int1, int2);
        int max = Math.max(int1, int2);
        int i = rand.nextInt();
        if (i < 0)
            i = -i;
        return i % (max - min) + min;
    }

    public static void startTiming()
    {
        startTime = System.currentTimeMillis();
    }

    public static void stopTiming()
    {
        stopTime = System.currentTimeMillis();
    }

    public static String getTime()
    {
        return (new Long(stopTime - startTime)).toString();
    }

    public static long getDiff()
    {
        return stopTime - startTime;
    }

    public static String getTime(long aDiff)
    {
        return (new Long(aDiff)).toString();
    }

    private static long startTime;
    private static long stopTime;
    private static Random rand;
}
