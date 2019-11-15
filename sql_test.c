#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sqlite_c.h"

#define HOMEDB_PATH "home.db"
#define HOMEDB_HOME_TABLE "HOME"
#define HOMEDB_DEVICES_TABLE "DEVICES"

#define HOMEDB_HOME_ENTRY_ID "ID"
#define HOMEDB_HOME_ENTRY_NAME "NAME"
#define HOMEDB_HOME_ENTRY_ICON "ICON"
#define HOMEDB_HOME_ENTRY_CSN "CENTRAL_SN"
#define HOMEDB_HOME_ENTRY_EXTEND "EXTEND"

#define HOMEDB_DEVICES_ENTRY_SN "SN"
#define HOMEDB_DEVICES_ENTRY_NAME "NAME"
#define HOMEDB_DEVICES_ENTRY_MAC2G "MAC_2G"
#define HOMEDB_DEVICES_ENTRY_MAC5G "MAC_5G"
#define HOMEDB_DEVICES_ENTRY_IP "IP"
#define HOMEDB_DEVICES_ENTRY_VERSION "VERSION"
#define HOMEDB_DEVICES_ENTRY_ICON "ICON"
#define HOMEDB_DEVICES_ENTRY_EKEY "EKEY"
#define HOMEDB_DEVICES_ENTRY_RPCPORT "RPC_PORT"
#define HOMEDB_DEVICES_ENTRY_MODEL "MODEL"
#define HOMEDB_DEVICES_ENTRY_STATUS "STATUS"
#define HOMEDB_DEVICES_ENTRY_UPTIME "UPTIME"
#define HOMEDB_DEVICES_ENTRY_EXTEND "EXTEND"

#define HOMEDB_VALUE_LEN 500

typedef enum {
    HOME_ENTRY_ID = 0,
    HOME_ENTRY_NAME,
    HOME_ENTRY_ICON,
    HOME_ENTRY_CSN,
    HOME_ENTRY_EXTEND,
    HOME_ENTRY_MAX,
} homedb_home_entry_e;

typedef enum {
    DEVICES_ENTRY_ID = 0,
    DEVICES_ENTRY_SN,
    DEVICES_ENTRY_NAME,
    DEVICES_ENTRY_MAC_2G,
    DEVICES_ENTRY_MAC_5G,
    DEVICES_ENTRY_IP,
    DEVICES_ENTRY_VERSION,
    DEVICES_ENTRY_ICON,
    DEVICES_ENTRY_EKEY,
    DEVICES_ENTRY_RPC_PORT,
    DEVICES_ENTRY_MODEL,
    DEVICES_ENTRY_STATUS,
    DEVICES_ENTRY_UPTIME,
    DEVICES_ENTRY_EXTEND,
    DEVICES_ENTRY_MAX,
} homedb_devices_entry_e;

int main(int argc, char *argv[]) {
    sqlite3 *db;
    int rc = 0;
    uint32_t idx = 0;
    uint32_t num = 0;
    char sql_out[HOMEDB_VALUE_LEN] = {0};
    char out_tbl[DEVICES_ENTRY_MAX][HOMEDB_VALUE_LEN] = {0};

    sql_keyval_t home_devkey = {0};
    sql_keyval_t home_devices[DEVICES_ENTRY_MAX] = {0};

    db = sql_ctx(HOMEDB_PATH);
    if (db == NULL) {
        SQL_ERROR("Error to get sqllite ctx!");
        goto err;
    }

    home_devkey.keystr = HOMEDB_DEVICES_ENTRY_SN;
    home_devkey.valstr = "018621945200138";
    home_devkey.valtyp = SQL_KEYVAL_TYPE_TEXT;
    rc = sql_read(db, HOMEDB_DEVICES_TABLE, &home_devkey,
                  NULL /*HOMEDB_DEVICES_ENTRY_MAC2G*/, sql_out);
    if (rc) {
        SQL_ERROR("Error to read");
        goto err;
    }
    SQL_DEBUG("readout[%s]", sql_out);

    num = sql_splout(out_tbl, DEVICES_ENTRY_MAX, HOMEDB_VALUE_LEN, sql_out);
    SQL_DEBUG("get out count=[%d]", num);

    for (idx = 0; idx < num; idx++) {
        SQL_DEBUG("tbl[%d] = [%s]", idx, out_tbl[idx]);
    }

    idx = 0;
    memset(home_devices, 0, sizeof(home_devices));

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_SN;
    home_devices[idx].valstr = "018621945200143";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_NAME;
    home_devices[idx].valstr = "LED5";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_MAC2G;
    home_devices[idx].valstr = "0011223399aa";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_IP;
    home_devices[idx].valstr = "192.168.127.105";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_VERSION;
    home_devices[idx].valstr = "1.0.8";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_MODEL;
    home_devices[idx].valstr = "1";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_STATUS;
    home_devices[idx].valstr = "0";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_UPTIME;
    home_devices[idx].valstr = "660";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    rc = sql_insert(db, HOMEDB_DEVICES_TABLE, home_devices, idx);
    if (rc) {
        SQL_ERROR("Error to insert");
        goto err;
    }

    home_devkey.keystr = HOMEDB_DEVICES_ENTRY_SN;
    home_devkey.valstr = "018621945200138";
    home_devkey.valtyp = SQL_KEYVAL_TYPE_TEXT;

    idx = 0;
    memset(home_devices, 0, sizeof(home_devices));

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_IP;
    home_devices[idx].valstr = "192.168.127.115";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_VERSION;
    home_devices[idx].valstr = "2.1.8";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_STATUS;
    home_devices[idx].valstr = "1";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;

    home_devices[idx].keystr = HOMEDB_DEVICES_ENTRY_UPTIME;
    home_devices[idx].valstr = "888";
    home_devices[idx].valtyp = SQL_KEYVAL_TYPE_TEXT;
    idx++;
    rc = sql_update(db, HOMEDB_DEVICES_TABLE, &home_devkey, home_devices, idx);
    if (rc) {
        SQL_ERROR("Error to update");
        goto err;
    }

    home_devkey.keystr = HOMEDB_DEVICES_ENTRY_SN;
    home_devkey.valstr = "018621945200139";
    home_devkey.valtyp = SQL_KEYVAL_TYPE_TEXT;
    rc = sql_delete(db, HOMEDB_DEVICES_TABLE, &home_devkey);
    if (rc) {
        SQL_ERROR("Error to delete");
        goto err;
    }

    home_devkey.keystr = HOMEDB_DEVICES_ENTRY_SN;
    home_devkey.valstr = "018621945200138";
    home_devkey.valtyp = SQL_KEYVAL_TYPE_TEXT;
    rc = sql_test(db, HOMEDB_DEVICES_TABLE, &home_devkey);
    SQL_DEBUG("Test Table %s [%d]", HOMEDB_DEVICES_TABLE, rc);

    memset(sql_out, 0, HOMEDB_VALUE_LEN);
    rc = sql_readi(db, HOMEDB_DEVICES_TABLE, 2, NULL, sql_out);
    if (rc) {
        SQL_ERROR("Error to read line %d", 2);
        goto err;
    }

    SQL_DEBUG("Read Line[%d] = [%s]", 2, sql_out);
err:
    sql_free(db);
    return 0;
}