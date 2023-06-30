rm dot.*
mpicxx main.cpp -o dot1
mpicxx main2.cpp -o dot2
mpicxx main3.cpp -o dot3
time mpirun -n 2 ./dot1
time mpirun -n 2 ./dot2
time mpirun -n 2 ./dot3