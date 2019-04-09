\ENCODING=ISO-8859-1
\Problem name: IloCplex

Maximize
 obj: Y(0) + Y(1) + Y(2) + Y(3) + z(0) + z(1) + z(2) + z(3)
Subject To
 c1: Y(0) + Y(1) + Y(2) + Y(3)  = 1
Bounds
 0 <= Y(0) <= 2
 0 <= Y(1) <= 2
 0 <= Y(2) <= 2
 0 <= Y(3) <= 2
 0 <= z(0) <= 3
 0 <= z(1) <= 3
 0 <= z(2) <= 3
 0 <= z(3) <= 3
Generals
 Y(0)  Y(1)  Y(2)  Y(3)  z(0)  z(1)  z(2)  z(3) 
End
