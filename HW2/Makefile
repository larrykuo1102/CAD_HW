run :
	g++ -std=c++11 ./main.cpp -o test
	./test -h aoi_sample02.blif 2 1 1

install :
	sudo apt install libboost-all-dev

env :
	export GUROBI_HOME="/opt/gurobi1003/linux64"
	export PATH="${PATH}:${GUROBI_HOME}/bin"
	export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"