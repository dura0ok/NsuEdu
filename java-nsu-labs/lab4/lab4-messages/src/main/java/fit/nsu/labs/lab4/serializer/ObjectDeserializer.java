package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.exception.DeserializeException;

import java.io.InputStream;
import java.io.ObjectInputStream;

public class ObjectDeserializer implements Deserializer {

    @Override
    public Object deserialize(InputStream inputStream) throws DeserializeException {
        try {
            var ois = new ObjectInputStream(inputStream);
            return ois.readObject();
        } catch (Exception e) {
            throw new DeserializeException("Error deserialize object", e);
        }
    }
}
