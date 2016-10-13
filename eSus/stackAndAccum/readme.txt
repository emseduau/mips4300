stackSim build commands:
mingw32-g++.exe -Wall -fexceptions -g  -c fileParser.cpp -o fileParser.o
mingw32-g++.exe -Wall -fexceptions -g  -c memorySim.cpp -o memorySim.o
mingw32-g++.exe -Wall -fexceptions -g  -c stackSim.cpp -o stackSim.o
mingw32-g++.exe  -o stackSim.exe fileParser.o memorySim.o stackSim.o   
Output file is bin\Debug\stackSimulator.exe with size 496.72 KB

accumSim build commands:
mingw32-g++.exe -Wall -fexceptions -g  -c accumSim.cpp -o accumSim.o
mingw32-g++.exe -Wall -fexceptions -g  -c fileParser.cpp -o fileParser.o
mingw32-g++.exe -Wall -fexceptions -g  -c memorySim.cpp -o memorySim.o
mingw32-g++.exe  -o accSim.exe accumSim.o fileParser.o memorySim.o   
Output file is bin\Debug\simuProj.exe with size 461.19 KB





