package javax.microedition.io;

import java.io.*;
import com.nokia.mj.impl.gcf.utils.ProtocolBase;


public class Connector
{
    public final static int READ  = 1;
    public final static int WRITE = 2;
    public final static int READ_WRITE = (READ|WRITE);

    private Connector() {}

    public static Connection open(String name) throws IOException
    {
        return open(name, READ_WRITE);
    }

    public static Connection open(String name, int mode) throws IOException
    {
        return open(name, mode, false);
    }

    public static Connection open(String name, int mode, boolean timeouts) throws IOException
    {

        if (name == null)
        {
            throw new IllegalArgumentException("url null");
        }

        int colon = name.indexOf(':');
        if (colon < 1)
        {
            throw new IllegalArgumentException("invalid url");
        }

        try
        {
            String protocol;
            protocol = name.substring(0, colon).toLowerCase();
            name = name.substring(colon+1);

            String root = "com.nokia.mj.impl.gcf.protocol";

            // System.out.println("open: " + root + "." + protocol + ".Protocol");
            Class clazz = Class.forName(root+"."+protocol+".Protocol");

            ProtocolBase cb = (ProtocolBase)clazz.newInstance();
            return cb.openConn(name, mode, timeouts);
        }
        catch (Exception e)
        {
            throw new IOException(e.toString());
        }
    }

    public static DataInputStream openDataInputStream(String name) throws IOException
    {
        Connection con = Connector.open(name, Connector.READ);
        DataInputStream is = null;

        try
        {
            is = ((InputConnection)con).openDataInputStream();
        }
        catch (ClassCastException e)
        {
            throw new IllegalArgumentException("invalid class");
        }
        finally
        {
            con.close();
        }

        return is;
    }

    public static DataOutputStream openDataOutputStream(String name) throws IOException
    {
        Connection con = Connector.open(name, Connector.WRITE);
        DataOutputStream os = null;

        try
        {
            os = ((OutputConnection)con).openDataOutputStream();
        }
        catch (ClassCastException e)
        {
            throw new IllegalArgumentException("invalid class");
        }
        finally
        {
            con.close();
        }

        return os;
    }

    public static InputStream openInputStream(String name) throws IOException
    {
        return openDataInputStream(name);
    }

    public static OutputStream openOutputStream(String name) throws IOException
    {
        return openDataOutputStream(name);
    }

}
