#ifndef __SQL_C_H__
#define __SQL_C_H__

#include <sqlite3.h>

#include "sql_error.h"

#define SQL_CMD_BUFF_LEN 500
#define SQL_BUFF_USE_MALLOC 0

typedef enum {
    SQL_KEYVAL_TYPE_INT = 0,
    SQL_KEYVAL_TYPE_TEXT,
    SQL_KEYVAL_TYPE_MAX,
} sql_valtyp_e;

typedef struct {
    char *keystr;
    char *valstr;
    sql_valtyp_e valtyp;
} sql_keyval_t;

typedef sqlite3 *sqlctx_t;

void sql_free(sqlctx_t db);
sqlctx_t sql_ctx(const char *dbmane);
error_t sql_get(sqlctx_t db, const char *sql, char *outs);
error_t sql_set(sqlctx_t db, const char *sql);

/*****************************************************************************
 Name        : sql_count
 Description : Get the number of entries in the specified table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
 Output      : NULL
 Return      : The number of entries in the specified table.
*****************************************************************************/
uint32_t sql_count(sqlctx_t db, const char *tbl);

/*****************************************************************************
 Name        : sql_test
 Description : Determine if the specified entry exists in the table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Non-zero for Existence / Zero for Nonexistence
*****************************************************************************/
uint32_t sql_test(sqlctx_t db, const char *tbl, sql_keyval_t *pkey);

/*****************************************************************************
 Name        : sql_delete
 Description : Delete the specified entry in the table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Error code.
*****************************************************************************/
error_t sql_delete(sqlctx_t db, const char *tbl, sql_keyval_t *pkey);

/*****************************************************************************
 Name        : sql_read
 Description : Read the specified entry in the table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
               const char *etykey     : Specified field of table entry.
 Output      : char *outs             : Read output string
 Return      : Error code.
*****************************************************************************/
error_t sql_read(sqlctx_t db, const char *tbl, sql_keyval_t *pkey,
                 const char *etykey, char *outs);

/*****************************************************************************
 Name        : sql_readi
 Description : Read the specified entry line in the table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               homedb_keyval_t *pkey  : Primary key and value of this table.
               const char *etykey     : Specified field of table entry.
 Output      : char *outs             : Read output string
 Return      : Error code.
*****************************************************************************/
error_t sql_readi(sqlctx_t db, const char *tbl, uint32_t line,
                  const char *etykey, char *outs);

/*****************************************************************************
 Name        : sql_update
 Description : Update the specified entry in the table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
               sql_keyval_t *etytbl   : Field table that needs to be updated.
               uint32_t etylen        : Field table length that needs to be
updated. Output      : NULL. Return      : Error code.
*****************************************************************************/
error_t sql_update(sqlctx_t db, const char *tbl, sql_keyval_t *pkey,
                   sql_keyval_t *etytbl, uint32_t etylen);

/*****************************************************************************
 Name        : sql_insert
 Description : Insert entry to the specified table.
 Input       : sqlctx_t db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *etytbl   : Field table that needs to be insert.
               uint32_t etylen        : Field table length that needs to be
insert. Output      : NULL. Return      : Error code.
*****************************************************************************/
error_t sql_insert(sqlctx_t db, const char *tbl, sql_keyval_t *etytbl,
                   uint32_t etylen);

/*****************************************************************************
 Name        : sql_splout
 Description : Split the output of the SQL statement.
 Input       : char *sql_out          : Sql output.
 Output      : char out_tbl[tbl_hlen][tbl_vlen];
 Return      : Error code.
*****************************************************************************/
uint32_t sql_splout(void *out_tbl, uint32_t tbl_hlen, uint32_t tbl_vlen,
                    char *sql_out);
#endif