
/*
 * This file was generated by mib2c and is intended for use as a mib module
 * for the ucd-snmp snmpd agent.
 */

#ifndef _MIBGROUP_DLMOD_MIB_H
#define _MIBGROUP_DLMOD_MIB_H

/* we use header_generic and checkmib from the util_funcs module */


/* Magic number definitions: */

#define   DLMODNEXTINDEX        1
#define   DLMODINDEX            2
#define   DLMODNAME             3
#define   DLMODPATH             4
#define   DLMODERROR            5
#define   DLMODSTATUS           6

/* function definitions */

extern void init_dlmod (void);
extern void deinit_dlmod (void);
extern int dynamic_init_dlmod (void);
extern int dynamic_deinit_dlmod (void);

static unsigned char  *var_dlmod (struct variable *, oid *, int *, int, int *, int (**write) (int, unsigned char *, unsigned char, int, unsigned char *, oid *, int));
static unsigned char  *var_dlmodEntry (struct variable *, oid *, int *, int, int *, int (**write) (int, unsigned char *, unsigned char, int, unsigned char *, oid *, int));
static int write_dlmodName (int, u_char *, u_char, int, u_char *, oid *, int);
static int write_dlmodPath (int, u_char *, u_char, int, u_char *, oid *, int);
static int write_dlmodStatus (int, u_char *, u_char, int, u_char *, oid *, int);


/*
 * this variable defines function callbacks and type return information for
 * the dlmod mib
 */

struct variable4 dlmod_variables[] = {
	{DLMODNEXTINDEX, ASN_INTEGER, RONLY, var_dlmod, 1, {1}},
	{DLMODINDEX, ASN_INTEGER, RONLY, var_dlmodEntry, 3, {2, 1, 1}},
	{DLMODNAME, ASN_OCTET_STR, RWRITE, var_dlmodEntry, 3, {2, 1, 2}},
	{DLMODPATH, ASN_OCTET_STR, RWRITE, var_dlmodEntry, 3, {2, 1, 3}},
	{DLMODERROR, ASN_OCTET_STR, RONLY, var_dlmodEntry, 3, {2, 1, 4}},
	{DLMODSTATUS, ASN_INTEGER, RWRITE, var_dlmodEntry, 3, {2, 1, 5}},

};
#endif				/* _MIBGROUP_DLMOD_MIB_H */
