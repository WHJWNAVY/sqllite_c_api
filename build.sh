gcc sql_test.c sqlite_c.c -lsqlite3 -Wall -o sql_test
rm home.db
./preinit.sh
./sql_test
