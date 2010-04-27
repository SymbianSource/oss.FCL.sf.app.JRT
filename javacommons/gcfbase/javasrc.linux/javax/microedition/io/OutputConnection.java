package javax.microedition.io;

import java.io.*;

public interface OutputConnection extends Connection
{
    public OutputStream openOutputStream() throws IOException;
    public DataOutputStream openDataOutputStream() throws IOException;
}

