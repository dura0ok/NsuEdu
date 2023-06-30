package fit.nsu.labs.model;

public interface Observable {
    void registerObserver(OnEvent o);
}