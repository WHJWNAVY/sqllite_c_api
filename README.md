# sqllite_c_api

## sql_count

```c
/*****************************************************************************
 Name        : sql_count
 Description : Get the number of entries in the specified table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
 Output      : NULL
 Return      : The number of entries in the specified table.
*****************************************************************************/
uint32_t sql_count(sqlite3 *db, const char *tbl);
```

## sql_test

```c
/*****************************************************************************
 Name        : sql_test
 Description : Determine if the specified entry exists in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Non-zero for Existence / Zero for Nonexistence
*****************************************************************************/
uint32_t sql_test(sqlite3 *db, const char *tbl, sql_keyval_t *pkey);
```

## sql_delete

```c
/*****************************************************************************
 Name        : sql_delete
 Description : Delete the specified entry in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               sql_keyval_t *pkey     : Primary key and value of this table.
 Output      : NULL
 Return      : Error code.
*****************************************************************************/
error_t sql_delete(sqlite3 *db, const char *tbl, sql_keyval_t *pkey);
```

## sql_read

```c
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
                 const char *etykey, char *outs);
```

## sql_readi

```c
/*****************************************************************************
 Name        : sql_readi
 Description : Read the specified entry line in the table.
 Input       : sqlite3 *db            : sqlite context.
               const char *tbl        : table name.
               homedb_keyval_t *pkey  : Primary key and value of this table.
               const char *etykey     : Specified field of table entry.
 Output      : char *outs             : Read output string
 Return      : Error code.
*****************************************************************************/
error_t sql_readi(sqlite3 *db, const char *tbl, uint32_t line,
                  const char *etykey, char *outs);
```

## sql_update

```c
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
                   sql_keyval_t *etytbl, uint32_t etylen);
```

## sql_insert

```c
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
                   uint32_t etylen);
```

## sql_insert

```c
/*****************************************************************************
 Name        : sql_splout
 Description : Split the output of the SQL statement.
 Input       : char *sql_out          : Sql output.
 Output      : char out_tbl[tbl_hlen][tbl_vlen];
 Return      : Error code.
*****************************************************************************/
uint32_t sql_splout(void *out_tbl, uint32_t tbl_hlen, uint32_t tbl_vlen,
                    char *sql_out);
```

