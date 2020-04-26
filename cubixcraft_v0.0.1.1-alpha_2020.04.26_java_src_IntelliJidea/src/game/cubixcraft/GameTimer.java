package game.cubixcraft;

public class GameTimer{

    private static final long NS_PER_SECOND       = 1000000000L;
    private static final long MAX_NS_PER_UPDATE   = 1000000000L;
    private static final int MAX_TICKS_PER_UPDATE = 100;
    private long lastDelta;
    public double deltaTime;
    private float ticksPerSecond;
    private long lastTime;
    public int ticks;
    public float timeScale;
    public double fps;
    public double passedTime;

    public GameTimer(final float ticksPerSecond) {
        timeScale = 1.0f;
        fps = 0.0;
        passedTime = 0.0;
        deltaTime = 0.0;
        this.ticksPerSecond = ticksPerSecond;
        lastTime = System.nanoTime();
        lastDelta = System.nanoTime();
    }

    public void advanceTime() {
        final long now = System.nanoTime();
        long passedNs = now - lastTime;
        lastTime = now;
        if (passedNs < 0L) {
            passedNs = 0L;
        }
        if (passedNs > MAX_NS_PER_UPDATE) {
            passedNs = MAX_NS_PER_UPDATE;
        }
        fps = NS_PER_SECOND / (double)passedNs;
        passedTime += passedNs * timeScale * ticksPerSecond / (double)NS_PER_SECOND;
        ticks = (int)passedTime;
        if (ticks > MAX_TICKS_PER_UPDATE) {
            ticks = MAX_TICKS_PER_UPDATE;
        }
        passedTime -= ticks;

        long currentTime = System.nanoTime();
        deltaTime = (currentTime - lastDelta) / (double)NS_PER_SECOND;
        lastDelta = currentTime;
    }
}
