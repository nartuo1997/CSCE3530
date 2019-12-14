Khang Tran

To compile:
gcc -o route lab5.c

Run:
./route

Make sure the router.txt is in the same directory

The program would read in the router.txt file and transform the information into 2d matrix. Then it will use dijkstra's algorithm
to computer the shortest path between the nodes. The sample output can be seen as below

Ajacency Matrix built from text file:
u v w x y z
0 2 5 1 0 0
2 0 3 2 0 0
5 3 0 3 1 5
1 2 3 0 1 0
0 0 1 1 0 2
0 0 5 0 2 0



Example Output:
v (u, v)
w (u, x)
x (u, x)
y (u, x)
z (u, x)
--------
u (v, u)
w (v, w)
x (v, x)
y (v, x)
z (v, x)
--------
u (w, y)
v (w, v)
x (w, y)
y (w, y)
z (w, y)
--------
u (x, u)
v (x, v)
w (x, y)
y (x, y)
z (x, y)
--------
u (y, x)
v (y, x)
w (y, w)
x (y, x)
z (y, z)
--------
u (z, y)
v (z, y)
w (z, y)
x (z, y)
y (z, y)
--------