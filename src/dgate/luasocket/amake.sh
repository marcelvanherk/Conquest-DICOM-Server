gcc -I../lua_5.1.5 -c auxiliar.c buffer.c except.c inet.c io.c luasocket.c buffer.c options.c tcp.c udp.c select.c usocket.c timeout.c
ld -r *.o -o luasocket.a
rm *.o
