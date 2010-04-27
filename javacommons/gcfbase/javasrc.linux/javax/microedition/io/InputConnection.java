package javax.microedition.io;

import java.io.*;

public interface InputConnection extends Connection
{
    public InputStream openInputStream() throws IOException;
    public DataInputStream openDataInputStream() throws IOException;
}

