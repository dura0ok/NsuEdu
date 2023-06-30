package fit.nsu.labs.lab4.client.model;

import fit.nsu.labs.lab4.client.OnEvent;

interface Observable {
    void registerObserver(OnEvent o);
}
