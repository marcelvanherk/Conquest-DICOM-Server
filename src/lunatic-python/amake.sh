gcc -fpic -c pythoninlua.c luainpython.c -I/usr/local/python3.8/include -I/usr/include/python3.8 -I../dgate/lua_5.1.5
gcc -shared *.o -o python.so -lpython3.8
sudo cp python.so /usr/lib/x86_64-linux-gnu/lua/5.1
rm *.o
