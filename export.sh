/usr/bin/em++ -O3 -Wno- -std=c++17 -I . -I $HOME/boost_1_84_0/  ./*.cpp -o ai.js -s WASM=0 -s ENVIRONMENT=web,worker -sSINGLE_FILE=1 -s NO_EXIT_RUNTIME=1 -s EXPORT_NAME="ai" -s "EXPORTED_RUNTIME_METHODS=['ccall']" -sMODULARIZE