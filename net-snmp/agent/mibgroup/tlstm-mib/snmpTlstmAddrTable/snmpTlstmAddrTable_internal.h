/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.table_data.conf 15999 2007-03-25 22:32:02Z dts12 $
 */
#ifndef TLSTMADDRTABLE_INTERNAL_H
#define TLSTMADDRTABLE_INTERNAL_H

/*
 * index definitions for table tlstmAddrTable 
 */
#define SNMPTARGETADDRNAME_MAX_SIZE  255

/*
 * column number definitions for table tlstmAddrTable 
 */
#define COLUMN_SNMPTLSTMADDRSERVERFINGERPRINT       1
#define COLUMN_SNMPTLSTMADDRSERVERIDENTITY          2
#define COLUMN_SNMPTLSTMADDRSTORAGETYPE             3
#define COLUMN_SNMPTLSTMADDRROWSTATUS               4

#define TLSTMADDRTABLE_MIN_COLUMN         COLUMN_SNMPTLSTMADDRSERVERFINGERPRINT
#define TLSTMADDRTABLE_MAX_COLUMN         COLUMN_SNMPTLSTMADDRROWSTATUS

#define TLSTMADDRSERVERFINGERPRINT_MAX_SIZE      255
#define TLSTMADDRSERVERIDENTITY_MAX_SIZE         255

    /*
     * structure for undo storage and other vars for set processing 
     */
typedef struct tlstmAddrTable_undo_s {
    char            fate;
    char            copied;
    char            is_consistent;
    netsnmp_request_info *req[TLSTMADDRTABLE_MAX_COLUMN + 1];
    /*
     * undo Column space 
     */
    char       tlstmAddrServerFingerprint[TLSTMADDRSERVERFINGERPRINT_MAX_SIZE];
    size_t          tlstmAddrServerFingerprint_len;
    char            tlstmAddrServerIdentity[TLSTMADDRSERVERIDENTITY_MAX_SIZE];
    size_t          tlstmAddrServerIdentity_len;
    char            tlstmAddrStorageType;
    char            tlstmAddrRowStatus;
    char            hashType;
} tlstmAddrTable_undo;

    /*
     * Typical data structure for a row entry 
     */
typedef struct tlstmAddrTable_entry_s {
    /*
     * Index values 
     */
    char            snmpTargetAddrName[SNMPTARGETADDRNAME_MAX_SIZE];
    size_t          snmpTargetAddrName_len;

    /*
     * Column values 
     */
    char        tlstmAddrServerFingerprint[TLSTMADDRSERVERFINGERPRINT_MAX_SIZE];
    size_t          tlstmAddrServerFingerprint_len;
    char            tlstmAddrServerIdentity[TLSTMADDRSERVERIDENTITY_MAX_SIZE];
    size_t          tlstmAddrServerIdentity_len;
    char            tlstmAddrStorageType;
    char            tlstmAddrRowStatus;
    char            hashType;

    /*
     * used during set processing 
     */
    tlstmAddrTable_undo *undo;

    /*
     * user data
     */
    struct netsnmp_cert_s   *cert;

} tlstmAddrTable_entry;

netsnmp_tdata_row *tlstmAddrTable_createEntry(netsnmp_tdata * table_data,
                                              char *snmpTargetAddrName,
                                              size_t snmpTargetAddrName_len);
void tlstmAddrTable_removeEntry(netsnmp_tdata * table_data,
                                netsnmp_tdata_row * row);


/*
 * column flags
 */
#define COLUMN_SNMPTLSTMADDRSERVERFINGERPRINT_FLAG      0x01
#define COLUMN_SNMPTLSTMADDRSERVERIDENTITY_FLAG         0x02
#define COLUMN_SNMPTLSTMADDRSTORAGETYPE_FLAG            0x04
#define COLUMN_SNMPTLSTMADDRROWSTATUS_FLAG              0x08

#define TLSTMADDRTABLE_SETTABLE_COLS (\
        COLUMN_SNMPTLSTMADDRSERVERFINGERPRINT_FLAG |                    \
        COLUMN_SNMPTLSTMADDRSERVERIDENTITY_FLAG |                       \
        COLUMN_SNMPTLSTMADDRSTORAGETYPE_FLAG |                          \
        COLUMN_SNMPTLSTMADDRROWSTATUS_FLAG)

#endif                          /* TLSTMADDRTABLE_INTERNAL_H */
