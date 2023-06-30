package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.exception.SerializeException;

public interface Serializer {
    byte[] serialize(Object object) throws SerializeException;
}
