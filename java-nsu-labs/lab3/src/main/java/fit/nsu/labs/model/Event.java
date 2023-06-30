package fit.nsu.labs.model;

import fit.nsu.labs.model.component.ComponentInfo;

public record Event(Class<? extends ComponentInfo> type, int totalProduced, int storageSize) {
}
