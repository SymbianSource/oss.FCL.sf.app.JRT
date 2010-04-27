/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.location.*;
import java.util.*;

public class ViperTest extends MIDlet implements CommandListener,
        LocationListener
{
    private Command iExitCmd = new Command("Quit", Command.EXIT, 1);

    private Command iSelectCmd = new Command("Choose Provider", Command.SCREEN,
            1);

    private Command iGetLocationCmd = new Command("Get location",
            Command.SCREEN, 1);

    private Command iGetLastKnownLocationCmd = new Command(
        "Last known location", Command.SCREEN, 1);

    private Command iStartTrackingCmd = new Command("Start tracking",
            Command.SCREEN, 1);

    private Command iStopTrackingCmd = new Command("Stop tracking",
            Command.SCREEN, 1);

    private Command iGetStatusCmd = new Command("Get status", Command.SCREEN, 1);

    private Command iRunTestCmd1 = new Command("Run tests", Command.SCREEN, 1);

    private Command iRunTestCmd2 = new Command("Run LP tests", Command.SCREEN,
            1);

    private Command iPreferencesCmd = new Command("Set preferences",
            Command.SCREEN, 1);

    private Display iDisplay;

    private Form iForm;

    private ViperPreferences iPrefs;

    private ProviderSelecter iSelecter = new ProviderSelecter();

    private LocationProvider iLocationProvider = null;

    private static int iN = 0;

    private long iLastUpdate;

    private long iLastTimestamp;

    public ViperTest()
    {
    }

    protected void destroyApp(boolean unconditional)
    {
        System.out.println("ViperTest.destroyApp");
        iLocationProvider = null;
        iForm = null;
        // iPrefs = null;
        iDisplay = null;
    }

    protected void pauseApp()
    {
        System.out.println("pauseApp");
        iDisplay.setCurrent(null);
    }

    protected void startApp()
    {
        iForm = new Form("ViperTest");
        iForm.addCommand(iExitCmd);
        iForm.addCommand(iRunTestCmd1);
        iForm.addCommand(iRunTestCmd2);
        iForm.addCommand(iSelectCmd);
        iForm.addCommand(iGetLocationCmd);
        iForm.addCommand(iGetLastKnownLocationCmd);
        iForm.addCommand(iGetStatusCmd);
        iForm.addCommand(iStartTrackingCmd);
        iForm.addCommand(iPreferencesCmd);
        iForm.append("LAPI version: "
                     + System.getProperty("microedition.location.version"));
        iDisplay = Display.getDisplay(this);
        iDisplay.setCurrent(iForm);
        iForm.setCommandListener(this);
        iPrefs = new ViperPreferences(iDisplay, iForm);
    }

    public void commandAction(Command c, Displayable d)
    {
        try
        {
            handleCommand(c);
        }
        catch (Exception e)
        {
            iForm.append("Exception: " + e);
        }
    }

    public void locationUpdated(LocationProvider provider, Location location)
    {
        if (provider != iLocationProvider)
        {
            System.out.println("provider = " + provider);
            System.out.println("iLocationProvider = " + iLocationProvider);
        }

        long now = System.currentTimeMillis();
        long timeDiff = now - iLastUpdate;
        iLastUpdate = now;

        iForm.deleteAll();

        iForm.append("Tracking #" + (++iN) + "\n");
        showLocation(location);

        iForm.append("Last update = " + timeDiff + " (ms) ago\n");

        System.out.println("------ locationUpdated #" + iN + " ------");
        System.out.println("Time of fix: " + time(now));
        System.out.println("Last update = " + timeDiff + " (ms) ago");

        long ts = location.getTimestamp();
        System.out.println("Timestamp: " + time(ts));
        if (location.isValid())
        {
            QualifiedCoordinates qc = location.getQualifiedCoordinates();
            System.out.println("Lat: " + format(qc.getLatitude()) + ", Lon: "
                               + format(qc.getLongitude()));
            System.out.println("Timestamp diff = " + (ts - iLastTimestamp)
                               + " (ms)");
        }
        else
        {
            System.out.println("Invalid location!");
        }
        iLastTimestamp = ts;
    }

    public void providerStateChanged(LocationProvider provider, int newState)
    {
        iForm.deleteAll();
        iForm.append("State changed\n");
        showState(newState);
    }

    private void handleCommand(Command c) throws Exception
    {
        iForm.deleteAll();

        if (c == iExitCmd)
        {
            notifyDestroyed();
        }
        else if (c == iSelectCmd)
        {
            System.out.println("Choose a LocationProvider");
            iDisplay.setCurrent(iSelecter);
        }
        else if (c == iPreferencesCmd)
        {
            iDisplay.setCurrent(iPrefs);
        }
        else if (c == iRunTestCmd1)
        {
            iForm.append("Running unit tests\n");
            Thread t = new Thread()
            {
                public void run()
                {
                    iForm.append(ViperUnitTest.run1() + "\n");
                }
            };
            t.start();
        }
        else if (c == iRunTestCmd2)
        {
            iForm.append("Running LocationProvider tests\n");
            Thread t = new Thread()
            {
                public void run()
                {
                    iForm.append(ViperUnitTest.run2(iDisplay) + "\n");
                }
            };
            t.start();
        }
        else if (c == iGetLastKnownLocationCmd)
        {
            iForm.append("Last known location\n");
            Location loc = LocationProvider.getLastKnownLocation();
            showLocation(loc);
        }
        else
        {

            if (iLocationProvider == null)
            {
                iLocationProvider = LocationProvider.getInstance(null);
                if (iLocationProvider == null)
                {
                    iForm.append("No location provider meets criteria\n");
                    return;
                }
            }

            if (c == iGetStatusCmd)
            {
                iForm.append("Get status\n");
                showState(iLocationProvider.getState());
            }
            else if (c == iGetLocationCmd)
            {
                iForm.append("Get location\n");
                Location loc = iLocationProvider.getLocation(iPrefs.iTimeout);
                showLocation(loc);
            }
            else if (c == iStartTrackingCmd)
            {
                iForm.append("Start tracking\n");
                int interval = iPrefs.iInterval;
                int timeout = iPrefs.iTimeout;
                int maxage = iPrefs.iMaxAge;

                iLastUpdate = System.currentTimeMillis();
                iLocationProvider.setLocationListener(this, interval, timeout,
                                                      maxage);
                iForm.addCommand(iStopTrackingCmd);
                iForm.removeCommand(iStartTrackingCmd);
            }
            else if (c == iStopTrackingCmd)
            {
                iForm.append("Stop tracking\n");
                iLocationProvider.setLocationListener(null, 0, 0, 0);
                iForm.addCommand(iStartTrackingCmd);
                iForm.removeCommand(iStopTrackingCmd);
            }
        }
    }

    private void showLocation(Location loc)
    {
        if (loc == null)
        {
            iForm.append("Location is null\n");
            return;
        }
        else if (!loc.isValid())
        {
            iForm.append("Location is not valid\n");
            return;
        }

        QualifiedCoordinates qc = loc.getQualifiedCoordinates();

        iForm.append("Lat = " + format(qc.getLatitude()) + "\n");
        iForm.append("Lon = " + format(qc.getLongitude()) + "\n");
        iForm.append("Altitude = " + qc.getAltitude() + "\n");
        iForm.append("Hacc = " + qc.getHorizontalAccuracy() + ", ");
        iForm.append("Vacc = " + qc.getVerticalAccuracy() + "\n");

        iForm.append("Speed = " + loc.getSpeed() + ", ");
        iForm.append("Course = " + loc.getCourse() + "\n");
        iForm.append("Method = " + lm(loc.getLocationMethod()) + "\n");

        // DateField dateFld = new DateField("Time of fix",
        // DateField.DATE_TIME);
        // dateFld.setDate(new Date(loc.getTimestamp()));
        // iForm.append(dateFld);
        iForm.append("Timestamp = " + time(loc.getTimestamp()) + "\n");

        AddressInfo addrInfo = loc.getAddressInfo();
        if (addrInfo != null)
        {
            printAddress(addrInfo);
        }

        String extra = loc.getExtraInfo("application/X-jsr179-location-nmea");
        if (extra != null)
        {
            System.out.println("Extra info:\n" + extra);
        }
    }

    private String format(double aCoord)
    {
        // if (iPrefs.iCoordFormat > 0) {
        // return Coordinates.convert(aCoord, iPrefs.iCoordFormat);
        // } else {
        return Double.toString(aCoord);
        // }
    }

    private String lm(int aMethod)
    {
        String s = "";
        if ((aMethod & Location.MTE_SATELLITE) > 0)
        {
            s = "Sat,";
        }
        if ((aMethod & Location.MTY_NETWORKBASED) > 0)
        {
            s += "Netw";
        }
        if ((aMethod & Location.MTY_TERMINALBASED) > 0)
        {
            s += "Term";
        }
        if ((aMethod & Location.MTA_ASSISTED) > 0)
        {
            s += ",assist";
        }
        if ((aMethod & Location.MTA_UNASSISTED) > 0)
        {
            s += ",unassist";
        }
        if ((aMethod & ~(Location.MTE_SATELLITE | Location.MTY_NETWORKBASED
                         | Location.MTY_TERMINALBASED | Location.MTA_ASSISTED | Location.MTA_UNASSISTED)) > 0)
        {
            s = "Illegal";
        }
        return s;
    }

    private String time(long aTimestamp)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(new Date(aTimestamp));
        int h = cal.get(Calendar.HOUR_OF_DAY);
        int m = cal.get(Calendar.MINUTE);
        int s = cal.get(Calendar.SECOND);
        int ms = cal.get(Calendar.MILLISECOND);
        return (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m + ":"
               + (s < 10 ? "0" : "") + s + "." + ms;
    }

    private void showState(int aState)
    {
        switch (aState)
        {
        case LocationProvider.AVAILABLE:
            iForm.append("Available");
            break;
        case LocationProvider.TEMPORARILY_UNAVAILABLE:
            iForm.append("Temp. unavailable");
            break;
        case LocationProvider.OUT_OF_SERVICE:
            iForm.append("Out of service");
            break;
        default:
            iForm.append("Unknown state");
            break;
        }
    }

    private void printAddress(AddressInfo a)
    {
        System.out.println("--- AddressInfo ---");
        System.out.println("Floor: " + a.getField(AddressInfo.BUILDING_FLOOR));
        System.out.println("Room: " + a.getField(AddressInfo.BUILDING_ROOM));
        System.out.println("B name: " + a.getField(AddressInfo.BUILDING_NAME));
        System.out.println("B zone: " + a.getField(AddressInfo.BUILDING_ZONE));
        System.out.println("City: " + a.getField(AddressInfo.CITY));
        System.out.println("Country: " + a.getField(AddressInfo.COUNTRY));
        System.out.println("C code: " + a.getField(AddressInfo.COUNTRY_CODE));
        System.out.println("County: " + a.getField(AddressInfo.COUNTY));
        System.out.println("Xing1: " + a.getField(AddressInfo.CROSSING1));
        System.out.println("Xing2: " + a.getField(AddressInfo.CROSSING2));
        System.out.println("District: " + a.getField(AddressInfo.DISTRICT));
        System.out.println("Extension: " + a.getField(AddressInfo.EXTENSION));
        System.out.println("Phone: " + a.getField(AddressInfo.PHONE_NUMBER));
        System.out.println("P Code: " + a.getField(AddressInfo.POSTAL_CODE));
        System.out.println("Street: " + a.getField(AddressInfo.STREET));
        System.out.println("State: " + a.getField(AddressInfo.STATE));
        System.out.println("URL: " + a.getField(AddressInfo.URL));
    }

    private class ProviderSelecter extends Form implements CommandListener
    {
        private Command iOkCmd = new Command("OK", Command.OK, 1);

        private Command iDefaultCmd = new Command("Null Criteria",
                Command.SCREEN, 1);

        private Command iNullCmd = new Command("Dereference", Command.SCREEN, 1);

        private Command iGcCmd = new Command("Garbage collect", Command.SCREEN,
                                             1);

        private TextField iHaccField = new TextField("Horizontal Accuracy",
                "0", 5, TextField.NUMERIC);

        private TextField iVaccField = new TextField("Vertical Accuracy", "0",
                5, TextField.NUMERIC);

        private TextField iRespTimeField = new TextField("Response time", "0",
                5, TextField.NUMERIC);

        private ChoiceGroup iPowerChoice = new ChoiceGroup("Power consumption",
                Choice.POPUP, new String[] { "No requirement", "Low", "Medium",
                                             "High"
                                           }, null);

        private ChoiceGroup iChoices = new ChoiceGroup("Choices",
                Choice.MULTIPLE, new String[] { "AddressInfo", "Cost allowed",
                                                "Altitude", "Speed + Course"
                                              }, null);

        ProviderSelecter()
        {
            super("Criteria");
            addCommand(iOkCmd);
            addCommand(iDefaultCmd);
            addCommand(iNullCmd);
            addCommand(iGcCmd);
            setCommandListener(this);
            append(iHaccField);
            append(iVaccField);
            append(iRespTimeField);
            append(iPowerChoice);
            append(iChoices);
        }

        public void commandAction(Command c, Displayable d)
        {
            if (c == iOkCmd || c == iDefaultCmd)
            {
                iForm.deleteAll();

                try
                {
                    LocationProvider l;
                    if (c == iOkCmd)
                    {
                        l = LocationProvider.getInstance(getCriteria());
                    }
                    else
                    {
                        l = LocationProvider.getInstance(null);
                    }
                    if (l == null)
                    {
                        iForm.append("Criteria too restrictive!\n");
                    }
                    else if (l != iLocationProvider)
                    {
                        iForm.append("New provider selected\n");
                        iLocationProvider = l;
                    }
                }
                catch (Exception e)
                {
                    iForm.append("Exception: " + e);
                }
            }
            else if (c == iNullCmd)
            {
                System.out.println("Setting LocationProvider to null");
                iLocationProvider = null;
            }
            else if (c == iGcCmd)
            {
                System.out.println("Calling System.gc()");
                System.gc();
            }
            iDisplay.setCurrent(iForm);
        }

        private Criteria getCriteria()
        {
            Criteria cr = new Criteria();
            int hacc = Integer.parseInt(iHaccField.getString());
            cr.setHorizontalAccuracy(hacc);

            int vacc = Integer.parseInt(iVaccField.getString());
            cr.setVerticalAccuracy(vacc);

            int resp = Integer.parseInt(iRespTimeField.getString());
            cr.setPreferredResponseTime(resp);

            int power = iPowerChoice.getSelectedIndex();
            cr.setPreferredPowerConsumption(power);

            boolean[] flags = new boolean[4];
            int numselected = iChoices.getSelectedFlags(flags);
            cr.setAddressInfoRequired(flags[0]);
            cr.setCostAllowed(flags[1]);
            cr.setAltitudeRequired(flags[2]);
            cr.setSpeedAndCourseRequired(flags[3]);

            print(cr);
            return cr;
        }

        void print(Criteria cr)
        {
            System.out.println("\n-- Criterias --");
            int hacc = cr.getHorizontalAccuracy();
            if (hacc != Criteria.NO_REQUIREMENT)
            {
                System.out.println("Required horizontal accuracy = " + hacc);
            }
            int vacc = cr.getVerticalAccuracy();
            if (vacc != Criteria.NO_REQUIREMENT)
            {
                System.out.println("Required vertical accuracy = " + vacc);
            }
            int resp = cr.getPreferredResponseTime();
            if (resp != Criteria.NO_REQUIREMENT)
            {
                System.out.println("Preferred response time = " + resp);
            }

            switch (cr.getPreferredPowerConsumption())
            {
            case Criteria.POWER_USAGE_LOW:
                System.out.println("Low power usage wanted");
                break;
            case Criteria.POWER_USAGE_MEDIUM:
                System.out.println("Medium power usage acceptable");
                break;
            case Criteria.POWER_USAGE_HIGH:
                System.out.println("High power usage acceptable");
                break;
            }

            if (cr.isAddressInfoRequired())
            {
                System.out.println("AddressInfo required");
            }
            if (!cr.isAllowedToCost())
            {
                System.out.println("Cost is not allowed");
            }
            if (cr.isAltitudeRequired())
            {
                System.out.println("Altitude required");
            }
            if (cr.isSpeedAndCourseRequired())
            {
                System.out.println("Speed and course info required");
            }
            System.out.println("---------------");
        }

    }

}
