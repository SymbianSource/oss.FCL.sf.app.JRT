package javax.microedition.io;

import java.io.*;

public class ConnectionNotFoundException extends IOException
{
    public ConnectionNotFoundException()
    {
        super();
    }

    public ConnectionNotFoundException(String s)
    {
        super(s);
    }
}

