package javax.microedition.io;

import java.io.*;

public interface StreamConnectionNotifier extends Connection
{
    public StreamConnection acceptAndOpen() throws IOException;
}


