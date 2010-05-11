package javax.microedition.amms;

/**
 * Please refer JSR 234 for more details.
 */
public interface MediaProcessorListener
{
    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSOR_REALIZED = "processRealized";

    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSING_STARTED = "processingStarted";

    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSING_STOPPED = "processingStopped";

    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSING_ABORTED = "processingAborted";

    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSING_COMPLETED = "processingCompleted";

    /**
     * Please refer JSR 234 for more details.
     */
    public static final String PROCESSING_ERROR = "processingError";


    /**
     * Please refer JSR 234 for more details.
     */
    public void mediaProcessorUpdate(MediaProcessor processor, String event, Object eventData);
}
