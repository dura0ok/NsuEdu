package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.exception.DeserializeException;

import java.io.InputStream;

public interface Deserializer {
    Object deserialize(InputStream inputStream) throws DeserializeException;
}
