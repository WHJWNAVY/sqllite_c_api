#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sqlite3.h>

#include "sql_error.h"
#include "sqlite_c.h"

char *SQL_CMD_BUFF = NULL;
char *SQL_OUT_BUFF = NULL;

static uint32_t buff_get(void) {
#if 0
    if (len == 0) {
        SQL_ERROR("Invalid sql buff len!");
        goto err;
    }
#else
    uint32_t len = SQL_CMD_BUFF_LEN;
#endif
    if (SQL_CMD_BUFF == NULL) {
        SQL_CMD_BUFF = (char *)malloc(len);
        if (SQL_CMD_BUFF == NULL) {
            SQL_ERROR("Error to malloc sql cmd buff!");
            goto err;
        }
    }

    if (SQL_OUT_BUFF == NULL) {
        SQL_OUT_BUFF = (char *)malloc(len);
        if (SQL_OUT_BUFF == NULL) {
            SQL_ERROR("Error to malloc sql cmd buff!");
            goto err;
        }
    }

    return len;
err:
    return 0;
}

static void buff_clr(void) {
    if (SQL_CMD_BUFF != NULL) {
        memset(SQL_CMD_BUFF, 0, SQL_CMD_BUFF_LEN);
    }

    if (SQL_OUT_BUFF != NULL) {
        memset(SQL_OUT_BUFF, 0, SQL_CMD_BUFF_LEN);
    }
}

static void buff_free(void) {
    if (SQL_CMD_BUFF) {
        free(SQL_CMD_BUFF);
        SQL_CMD_BUFF = NULL;
    }
    if (SQL_OUT_BUFF) {
        free(SQL_OUT_BUFF);
        SQL_OUT_BUFF = NULL;
    }
}

void sql_free(sqlite3 *db) {
    buff_free();
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

sqlite3 *sql_ctx(const char *dbmane) {
    int rc = 0;
    sqlite3 *db = NULL;
    if ((dbmane == NULL) || (strlen(dbmane) == 0)) {
        SQL_ERROR("Invalid Parameters");
        goto err;
    }
    /* Open database */
    rc = sqlite3_open(dbmane, &db);
    if (rc) {
        SQL_ERROR("Can't open database <%s>, err<%s>", dbmane,
                  sqlite3_errmsg(db));
        goto err;
    }

    if (buff_get() == 0) {
        goto err;
    }

    return db;
err:
    sql_free(db);
    return NULL;
}

/* If the SELECT statement query contains multiple records,
 * SQLITE will call the <sql_callback> callback function for each record.
 */
static int sql_callback(void *sql_out, int argc, char **argv,
                        char **azColName) {
    uint32_t i = 0;
    char *out = (char *)sql_out;
    for (i = 0; i < argc; i++) {
        SQL_DEBUG("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
        sprintf(out, "%s",
                argv[i] ? argv[i] : " "); // Replace empty data with spaces
        out += strlen(out);
        if ((i + 1) < argc) {
            sprintf(out, "%s", "|"); // Split data with '|'
            out += strlen(out);
        }
    }
    *out = '\0';
    return 0;
}

/* If the SELECT statement query result contains multiple records,
 * only the last record will be saved to outs, and the previous results
 * will be overwritten later.
 */
error_t sql_get(sqlite3 *db, const char *sql, char *outs) {
    int32_t rc = 0;
    char *zErrMsg = NULL;
    error_t ecode = E_OK;

    if ((db == NULL) || (sql == NULL) || (outs == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }
    SQL_DEBUG("SQL CMD: [%s]", sql);
    rc = sqlite3_exec(db, sql, sql_callback, (void *)outs, &zErrMsg);
    if (rc != SQLITE_OK) {
        SQL_ERROR("Error to exec SQL:[%s]", zErrMsg);
        ecode = E_ROUTER_IO;
        goto err;
    }

    if (strlen(outs) == 0) {
        SQL_ERROR("Can't get any data from sqlite");
        ecode = E_ROUTER_IO;
        goto err;
    }

    SQL_DEBUG("SQL Output=[%s]", outs);
    ecode = E_OK;
err:
    if (zErrMsg) {
        sqlite3_free(zErrMsg);
    }
    return ecode;
}

error_t sql_set(sqlite3 *db, const char *sql) {
    int32_t rc = 0;
    char *zErrMsg = NULL;
    error_t ecode = E_OK;

    if ((db == NULL) || (sql == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }
    SQL_DEBUG("SQL CMD: [%s]", sql);
    rc = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        ecode = E_ROUTER_IO;
        SQL_ERROR("Error to exec SQL:[%s]", zErrMsg);
        goto err;
    }

    ecode = E_OK;
err:
    if (zErrMsg) {
        sqlite3_free(zErrMsg);
    }
    return ecode;
}

/*****************************************************************************
 Name        : sql_count
 Description : Get the number of entries in the specified table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
 Output      : NULL
 Return      : The number of entries in the specified table.
*****************************************************************************/
uint32_t sql_count(sqlite3 *db, const char *tbl) {
    error_t ecode = E_OK;
    uint32_t count = 0;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    // char outs[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *outs = SQL_OUT_BUFF;

    buff_clr();

    if ((db == NULL) || (tbl == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    sprintf(sqls, "SELECT COUNT(*) FROM %s;", tbl);

    ecode = sql_get(db, sqls, outs);
    if (ecode != E_OK) {
        SQL_ERROR("Error to count table [%s]", tbl);
        count = 0;
        goto err;
    }

    count = atoi(outs);
err:
    return count;
}

#define HOMDB_VALTYP_FMT_INT "%d"
#define HOMDB_VALTYP_FMT_TEXT "'%s'"

static char *kv2str(sql_keyval_t *keyval, char *outs) {
    if (keyval->valtyp == SQL_KEYVAL_TYPE_INT) {
        sprintf(outs, "%s=" HOMDB_VALTYP_FMT_INT, keyval->keystr,
                *(int32_t *)(keyval->valstr));
    } else {
        sprintf(outs, "%s=" HOMDB_VALTYP_FMT_TEXT, keyval->keystr,
                keyval->valstr);
    }

    return outs;
}

static char *kv2str2(sql_keyval_t *keyval, char *outs) {
    if (keyval->valtyp == SQL_KEYVAL_TYPE_INT) {
        sprintf(outs, HOMDB_VALTYP_FMT_INT, *(int32_t *)(keyval->valstr));
    } else {
        sprintf(outs, HOMDB_VALTYP_FMT_TEXT, keyval->valstr);
    }

    return outs;
}

/*****************************************************************************
 Name        : sql_test
 Description : Determine if the specified entry exists in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Non-zero for Existence / Zero for Nonexistence
*****************************************************************************/
uint32_t sql_test(sqlite3 *db, const char *tbl, sql_keyval_t *pkey) {
    error_t ecode = E_OK;
    uint32_t count = 0;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    // char outs[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *outs = SQL_OUT_BUFF;
    char *psqls = sqls;
    buff_clr();

    if ((db == NULL) || (tbl == NULL) || (pkey == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    sprintf(psqls, "SELECT COUNT(*) FROM %s WHERE ", tbl);
    psqls += strlen(psqls);

    kv2str(pkey, psqls);
    psqls += strlen(psqls);

    sprintf(psqls, "%s", ";");
    psqls += strlen(psqls);

    ecode = sql_get(db, sqls, outs);
    if (ecode != E_OK) {
        SQL_ERROR("Error to test table [%s]", tbl);
        count = 0;
        goto err;
    }

    count = atoi(outs);
err:
    return count;
}

/*****************************************************************************
 Name        : sql_delete
 Description : Delete the specified entry in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Error code.
*****************************************************************************/
error_t sql_delete(sqlite3 *db, const char *tbl, sql_keyval_t *pkey) {
    error_t ecode = E_OK;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *psqls = sqls;

    buff_clr();

    if ((db == NULL) || (tbl == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    sprintf(psqls, "DELETE FROM %s ", tbl);
    psqls += strlen(psqls);

    if (pkey != NULL) {
        sprintf(psqls, "%s", "WHERE ");
        psqls += strlen(psqls);

        kv2str(pkey, psqls);
        psqls += strlen(psqls);
    }

    sprintf(psqls, "%s", ";");
    psqls += strlen(psqls);

    ecode = sql_set(db, sqls);
    if (ecode != E_OK) {
        SQL_ERROR("Error To Delete Table [%s]", tbl);
        goto err;
    }
    ecode = E_OK;
err:
    return ecode;
}

/*****************************************************************************
 Name        : sql_read
 Description : Read the specified entry in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
               const char *etykey     : Specified field of table entry.
 Output      : char *outs             : Read output string
 Return      : Error code.
*****************************************************************************/
error_t sql_read(sqlite3 *db, const char *tbl, sql_keyval_t *pkey,
                 const char *etykey, char *outs) {
    error_t ecode = E_OK;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *psqls = sqls;

    buff_clr();

    if ((db == NULL) || (tbl == NULL) || (outs == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    if (etykey == NULL) {
        sprintf(psqls, "SELECT * FROM %s ", tbl);
    } else {
        sprintf(psqls, "SELECT %s FROM %s ", etykey, tbl);
    }
    psqls += strlen(psqls);

    if (pkey != NULL) {
        sprintf(psqls, "%s", "WHERE ");
        psqls += strlen(psqls);

        kv2str(pkey, psqls);
        psqls += strlen(psqls);
    }

    sprintf(psqls, "%s", ";");
    psqls += strlen(psqls);

    ecode = sql_get(db, sqls, outs);
    if (ecode != E_OK) {
        SQL_ERROR("Error To Read Table [%s]", tbl);
        goto err;
    }
    ecode = E_OK;
err:
    return ecode;
}

/*****************************************************************************
 Name        : mercku_sql_readi
 Description : Read the specified entry line in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               homedb_keyval_t *pkey  : Primary key and value of this table.
               const char *etykey     : Specified field of table entry.
 Output      : char *outs             : Read output string
 Return      : Error code.
*****************************************************************************/
error_t sql_readi(sqlite3 *db, const char *tbl, uint32_t line,
                  const char *etykey, char *outs) {
    error_t ecode = E_OK;
    char *sqls = SQL_CMD_BUFF;
    char *psqls = sqls;

    buff_clr();

    if ((db == NULL) || (tbl == NULL) || (outs == NULL)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    if (etykey == NULL) {
        sprintf(psqls, "SELECT * FROM %s ", tbl);
    } else {
        sprintf(psqls, "SELECT %s FROM %s ", etykey, tbl);
    }
    psqls += strlen(psqls);

    sprintf(psqls, "LIMIT 1 OFFSET %d;", line);
    psqls += strlen(psqls);

    ecode = sql_get(db, sqls, outs);
    if (ecode != E_OK) {
        SQL_ERROR("Error To Read Table [%s] line[%d]", tbl, line);
        goto err;
    }
    ecode = E_OK;
err:
    return ecode;
}

/*****************************************************************************
 Name        : sql_update
 Description : Update the specified entry in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
               sql_keyval_t *etytbl   : Field table that needs to be updated.
               uint32_t etylen        : Field table length that needs to be
updated. Output      : NULL. Return      : Error code.
*****************************************************************************/
error_t sql_update(sqlite3 *db, const char *tbl, sql_keyval_t *pkey,
                   sql_keyval_t *etytbl, uint32_t etylen) {
    uint32_t idx = 0;
    error_t ecode = E_OK;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *psqls = sqls;

    buff_clr();

    if ((db == NULL) || (tbl == NULL) || (etytbl == NULL) || (etylen == 0)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    sprintf(psqls, "UPDATE %s SET ", tbl);
    psqls += strlen(psqls);

    for (idx = 0; idx < etylen; idx++) {
        kv2str(&(etytbl[idx]), psqls);
        psqls += strlen(psqls);
        sprintf(psqls, "%s", ",");
        psqls += strlen(psqls);
    }

    psqls--; // Remove extra ','

    if (pkey != NULL) {
        sprintf(psqls, "%s", " WHERE ");
        psqls += strlen(psqls);

        kv2str(pkey, psqls);
        psqls += strlen(psqls);
    }

    sprintf(psqls, "%s", ";");
    psqls += strlen(psqls);

    ecode = sql_set(db, sqls);
    if (ecode != E_OK) {
        SQL_ERROR("Error To Update Table [%s]", tbl);
        goto err;
    }
    ecode = E_OK;
err:
    return ecode;
}

/*****************************************************************************
 Name        : sql_insert
 Description : Insert entry to the specified table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *etytbl   : Field table that needs to be insert.
               uint32_t etylen        : Field table length that needs to be
insert. Output      : NULL. Return      : Error code.
*****************************************************************************/
error_t sql_insert(sqlite3 *db, const char *tbl, sql_keyval_t *etytbl,
                   uint32_t etylen) {
    uint32_t idx = 0;
    error_t ecode = E_OK;
    // char sqls[SQL_CMD_BUFF_LEN] = {0};
    char *sqls = SQL_CMD_BUFF;
    char *psqls = sqls;

    buff_clr();

    if ((db == NULL) || (tbl == NULL) || (etytbl == NULL) || (etylen == 0)) {
        SQL_ERROR("Invalid Parameters");
        ecode = E_BAD_REQ;
        goto err;
    }

    sprintf(psqls, "INSERT INTO %s (", tbl);
    psqls += strlen(psqls);

    for (idx = 0; idx < etylen; idx++) {
        sprintf(psqls, "%s,", etytbl[idx].keystr);
        psqls += strlen(psqls);
    }
    psqls--; // Remove extra ','

    sprintf(psqls, "%s", ") VALUES (");
    psqls += strlen(psqls);

    for (idx = 0; idx < etylen; idx++) {
        kv2str2(&(etytbl[idx]), psqls);
        psqls += strlen(psqls);
        sprintf(psqls, "%s", ",");
        psqls += strlen(psqls);
    }
    psqls--; // Remove extra ','

    sprintf(psqls, "%s", ");");
    psqls += strlen(psqls);

    ecode = sql_set(db, sqls);
    if (ecode != E_OK) {
        SQL_ERROR("Error To Insert Table [%s]", tbl);
        goto err;
    }
    ecode = E_OK;
err:
    return ecode;
}

/*****************************************************************************
 Name        : sql_splout
 Description : Split the output of the SQL statement.
 Input       : char *sql_out          : Sql output.
 Output      : char out_tbl[tbl_hlen][tbl_vlen];
 Return      : Error code.
*****************************************************************************/
uint32_t sql_splout(void *out_tbl, uint32_t tbl_hlen, uint32_t tbl_vlen,
                    char *sql_out) {
    char *split = "|";
    char *outt = (char *)out_tbl;
    uint32_t wcnt = 0;
    uint32_t clen = /*SQL_VALUE_LEN*/ tbl_vlen;

    if ((out_tbl == NULL) || (sql_out == NULL) || (tbl_vlen == 0)) {
        goto err;
    }

    char *token = strtok(sql_out, split);
    while (token != NULL) {
        if (*token == ' ') { // Space indicates that the data is empty.
            *token = '\0';
        }
        // SQL_DEBUG("key[%d] = [%s]", wcnt, token);
        strcpy((outt + wcnt * clen), token);
        token = strtok(NULL, split);
        wcnt++;
        if (tbl_hlen) {
            if (wcnt >= tbl_hlen) {
                break;
            }
        }
    }

    return wcnt;

err:
    return 0;
}