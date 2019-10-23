clear
g++ `wx-config --cxxflags` -o CheckersApp *.cpp `wx-config --libs`
./CheckersApp