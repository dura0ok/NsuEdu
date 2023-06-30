package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.exception.SerializeException;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

public class ObjectSerializer implements Serializer {

    @Override
    public byte[] serialize(Object object) throws SerializeException {
        try {
            var baos = new ByteArrayOutputStream();
            var oos = new ObjectOutputStream(baos);
            oos.writeObject(object);
            oos.flush();

            return baos.toByteArray();
        } catch (IOException e) {
            throw new SerializeException("Error serialize object", e);
        }
    }
}
