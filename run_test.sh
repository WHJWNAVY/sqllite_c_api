#!/bin/sh
SQL_C_TEST_PROGRAM=./bin/sqlc_test
CUSTOMER_HW_SERIAL_NUM="018621945292157"
MERCKU_HOME_DB_PATH="home.db"
MERCKU_HOME_UUID=$(cat /proc/sys/kernel/random/uuid | sed 's/-//g')

echo "home uuid = $MERCKU_HOME_UUID"
echo "home sn = $CUSTOMER_HW_SERIAL_NUM"
MERCKU_HOME_NAME="HOME-${CUSTOMER_HW_SERIAL_NUM:10}"
echo "home name = $MERCKU_HOME_NAME"
mercku_homedb_create() {
    if [[ -e $MERCKU_HOME_DB_PATH ]]; then
        echo "Database file <$MERCKU_HOME_DB_PATH> already exists!"
    else
        echo "The database file <$MERCKU_HOME_DB_PATH> does not exist, Create It!"
        sqlite3 $MERCKU_HOME_DB_PATH "CREATE TABLE HOME(ID TEXT PRIMARY KEY NOT NULL,NAME TEXT,ICON TEXT,CENTRAL_SN TEXT,EXTEND TEXT);"
        sqlite3 $MERCKU_HOME_DB_PATH "CREATE TABLE DEVICES(ID INTEGER PRIMARY KEY AUTOINCREMENT,SN TEXT NOT NULL,NAME TEXT NOT NULL,MAC_2G TEXT NOT NULL,MAC_5G TEXT,IP TEXT,VERSION TEXT,ICON TEXT,EKEY TEXT,RPC_PORT TEXT,MODEL TEXT,STATUS TEXT,UPTIME TEXT,EXTEND TEXT);"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO HOME (ID,NAME,CENTRAL_SN) VALUES ('$MERCKU_HOME_UUID', '$MERCKU_HOME_NAME', '$CUSTOMER_HW_SERIAL_NUM');"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO 'DEVICES' VALUES(NULL,'018621945200138','LED0','001122334455',NULL,'192.168.127.101','1.0.0',NULL,NULL,2000,1,1,160,NULL	 );"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO 'DEVICES' VALUES(NULL,'018621945200139','LED1','001122335566',NULL,'192.168.127.102','1.0.2',NULL,NULL,2000,1,0,260,NULL    );"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO 'DEVICES' VALUES(NULL,'018621945200140','LED2','001122336677',NULL,'192.168.127.103','1.0.4',NULL,NULL,2000,1,1,360,NULL    );"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO 'DEVICES' VALUES(NULL,'018621945200141','LED3','001122337788',NULL,'192.168.127.104','1.0.6',NULL,NULL,2000,1,0,460,NULL    );"
        sqlite3 $MERCKU_HOME_DB_PATH "INSERT INTO 'DEVICES' VALUES(NULL,'018621945200142','LED4','001122338899',NULL,'192.168.127.105','1.0.8',NULL,NULL,2000,1,1,560,NULL    );"
        echo "==========$MERCKU_HOME_DB_PATH==========" >> preinit.log
        sqlite3 $MERCKU_HOME_DB_PATH .dump >> preinit.log
        echo "==========$MERCKU_HOME_DB_PATH==========" >> preinit.log
    fi
}

run_test() {
    rm $MERCKU_HOME_DB_PATH
	mercku_homedb_create
    $SQL_C_TEST_PROGRAM
}

run_test