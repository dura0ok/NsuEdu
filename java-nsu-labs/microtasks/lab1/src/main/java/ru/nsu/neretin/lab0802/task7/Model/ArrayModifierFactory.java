package ru.nsu.neretin.lab0802.task7.Model;

public class ArrayModifierFactory {
    public static ArrayModifier createArrayModifier(){
        return (index, arr) -> {
            if (arr == null || index < 0 || index >= arr.length) {
                return arr;
            }

            int[] anotherArray = new int[arr.length - 1];

            for (int i = 0, k = 0; i < arr.length; i++) {
                if (i == index) {
                    continue;
                }
                anotherArray[k++] = arr[i];
            }

            return anotherArray;
        };
    }


}
