#ifndef DMSPACETYPE_H
#define DMSPACETYPE_H

#ifndef NULL
#define NULL 0
#endif /* NULL */

/*
 *
 * type summarize:
 * 0xf0 string
 *
 * 0xd0 number
 *
 *
 * 0xe1 list
 * 0xe2 hash table
 * 0xe3 key_value
 * 0xe4 json
 *
 * 0xe5 RootNode
 *
 */


static char TYPE_STRING = 0xf0;

static char TYPE_NUMBER = 0xd0;

static char TYPE_LIST = 0xe1;
static char TYPE_HASH_TABLE = 0xe2;
static char TYPE_KEY_VALUE = 0xe3;
static char TYPE_JSON = 0xe4;

static char TYPE_ROOT_NODE = 0xe5;
static char TYPE_JSONBASE_CONNECTION_POOL = 0xe6;


#endif /* DMSPACETYPE_H */

