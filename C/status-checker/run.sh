rm build/status-checker
gcc src/*.c -o build/status-checker -I/usr/local/include -L/usr/local/lib -ldiscord -lcurl -lpthread -lcrypto
build/status-checker
