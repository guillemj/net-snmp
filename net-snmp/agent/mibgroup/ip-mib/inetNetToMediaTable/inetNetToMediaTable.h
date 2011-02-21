/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 1.48 $ of : mfd-top.m2c,v $
 *
 * $Id$
 */
#ifndef INETNETTOMEDIATABLE_H
#define INETNETTOMEDIATABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


/** @addtogroup misc misc: Miscellaneous routines
 *
 * @{
 */
#include <net-snmp/library/asn1.h>
#include <net-snmp/data_access/arp.h>

    /*
     * other required module components 
     */
    /* *INDENT-OFF*  */
config_require(ip-mib/data_access/arp)
config_require(ip-mib/inetNetToMediaTable/inetNetToMediaTable_interface)
config_require(ip-mib/inetNetToMediaTable/inetNetToMediaTable_data_access)
    /* *INDENT-ON*  */

    /*
     * OID, column number and enum definions for inetNetToMediaTable 
     */
#include "inetNetToMediaTable_constants.h"

    /*
     *********************************************************************
     * function declarations
     */
    void            init_inetNetToMediaTable(void);
    void            shutdown_inetNetToMediaTable(void);

    /*
     *********************************************************************
     * Table declarations
     */
/**********************************************************************
 **********************************************************************
 ***
 *** Table inetNetToMediaTable
 ***
 **********************************************************************
 **********************************************************************/
    /*
     * IP-MIB::inetNetToMediaTable is subid 35 of ip.
     * Its status is Current.
     * OID: .1.3.6.1.2.1.4.35, length: 8
     */
    /*
     *********************************************************************
     * When you register your mib, you get to provide a generic
     * pointer that will be passed back to you for most of the
     * functions calls.
     *
     * TODO:100:r: Review all context structures
     */
    /*
     * TODO:101:o: |-> Review inetNetToMediaTable registration context.
     */
    typedef netsnmp_data_list inetNetToMediaTable_registration;

/**********************************************************************/
    /*
     * TODO:110:r: |-> Review inetNetToMediaTable data context structure.
     * This structure is used to represent the data for inetNetToMediaTable.
     */
    typedef netsnmp_arp_entry inetNetToMediaTable_data;


    /*
     *********************************************************************
     * TODO:115:o: |-> Review inetNetToMediaTable undo context.
     * We're just going to use the same data structure for our
     * undo_context. If you want to do something more efficent,
     * define your typedef here.
     */
    typedef inetNetToMediaTable_data inetNetToMediaTable_undo_data;

    /*
     * TODO:120:r: |-> Review inetNetToMediaTable mib index.
     * This structure is used to represent the index for inetNetToMediaTable.
     */
    typedef struct inetNetToMediaTable_mib_index_s {

        /*
         * inetNetToMediaIfIndex(1)/InterfaceIndex/ASN_INTEGER/long(long)//l/a/w/e/R/d/H
         */
        long            inetNetToMediaIfIndex;

        /*
         * inetNetToMediaNetAddressType(2)/InetAddressType/ASN_INTEGER/long(u_long)//l/a/w/E/r/d/h
         */
        u_long          inetNetToMediaNetAddressType;

        /*
         * inetNetToMediaNetAddress(3)/InetAddress/ASN_OCTET_STR/char(char)//L/a/w/e/R/d/h
         */
        /** 128 - 2(other indexes) - oid length(10) = 115 */
        char            inetNetToMediaNetAddress[115];
        size_t          inetNetToMediaNetAddress_len;


    } inetNetToMediaTable_mib_index;

    /*
     * TODO:121:r: |   |-> Review inetNetToMediaTable max index length.
     * If you KNOW that your indexes will never exceed a certain
     * length, update this macro to that length.
     *
     * BE VERY CAREFUL TO TAKE INTO ACCOUNT THE MAXIMUM
     * POSSIBLE LENGHT FOR EVERY VARIABLE LENGTH INDEX!
     * Guessing 128 - col/entry(2)  - oid len(8)
     */
#define MAX_inetNetToMediaTable_IDX_LEN     118


    /*
     *********************************************************************
     * TODO:130:o: |-> Review inetNetToMediaTable Row request (rowreq) context.
     * When your functions are called, you will be passed a
     * inetNetToMediaTable_rowreq_ctx pointer.
     */
    typedef struct inetNetToMediaTable_rowreq_ctx_s {

    /** this must be first for container compare to work */
        netsnmp_index   oid_idx;
        oid             oid_tmp[MAX_inetNetToMediaTable_IDX_LEN];

        inetNetToMediaTable_mib_index tbl_idx;

        inetNetToMediaTable_data *data;
        inetNetToMediaTable_undo_data *undo;
        unsigned int    column_set_flags;       /* flags for set columns */


        /*
         * flags per row. Currently, the first (lower) 8 bits are reserved
         * for the user. See mfd.h for other flags.
         */
        u_int           rowreq_flags;

        /*
         * TODO:131:o: |   |-> Add useful data to inetNetToMediaTable rowreq context.
         */
        int             inetNetToMediaRowStatus,
            inetNetToMediaRowStatus_undo;

        /* flag, if the row was present in last read from operating system */
        int valid;

        /*
         * storage for future expansion
         */
        netsnmp_data_list *inetNetToMediaTable_data_list;

    } inetNetToMediaTable_rowreq_ctx;

    typedef struct inetNetToMediaTable_ref_rowreq_ctx_s {
        inetNetToMediaTable_rowreq_ctx *rowreq_ctx;
    } inetNetToMediaTable_ref_rowreq_ctx;

    /*
     *********************************************************************
     * function prototypes
     */
    int
        inetNetToMediaTable_pre_request(inetNetToMediaTable_registration *
                                        user_context);
    int
        inetNetToMediaTable_post_request(inetNetToMediaTable_registration *
                                         user_context, int rc);

    int
        inetNetToMediaTable_rowreq_ctx_init(inetNetToMediaTable_rowreq_ctx
                                            * rowreq_ctx,
                                            void *user_init_ctx);
    void
        inetNetToMediaTable_rowreq_ctx_cleanup
        (inetNetToMediaTable_rowreq_ctx * rowreq_ctx);

    inetNetToMediaTable_data *inetNetToMediaTable_allocate_data(void);
    void
         inetNetToMediaTable_release_data(inetNetToMediaTable_data * data);

    int
        inetNetToMediaTable_check_dependencies
        (inetNetToMediaTable_rowreq_ctx * rowreq_ctx);
    int
        inetNetToMediaTable_commit(inetNetToMediaTable_rowreq_ctx *
                                   rowreq_ctx);

        inetNetToMediaTable_rowreq_ctx
        * inetNetToMediaTable_row_find_by_mib_index
        (inetNetToMediaTable_mib_index * mib_idx);

    extern const oid      inetNetToMediaTable_oid[];
    extern const int      inetNetToMediaTable_oid_size;


#include "inetNetToMediaTable_interface.h"
#include "inetNetToMediaTable_data_access.h"
    /*
     *********************************************************************
     * GET function declarations
     */

    /*
     *********************************************************************
     * GET Table declarations
     */
/**********************************************************************
 **********************************************************************
 ***
 *** Table inetNetToMediaTable
 ***
 **********************************************************************
 **********************************************************************/
    /*
     * IP-MIB::inetNetToMediaTable is subid 35 of ip.
     * Its status is Current.
     * OID: .1.3.6.1.2.1.4.35, length: 8
     */
    /*
     * indexes
     */

    int
        inetNetToMediaPhysAddress_get(inetNetToMediaTable_rowreq_ctx *
                                      rowreq_ctx, char
                                      **inetNetToMediaPhysAddress_val_ptr_ptr,
                                      size_t *
                                      inetNetToMediaPhysAddress_val_ptr_len_ptr);
    int
        inetNetToMediaLastUpdated_get(inetNetToMediaTable_rowreq_ctx *
                                      rowreq_ctx,
                                      u_long *
                                      inetNetToMediaLastUpdated_val_ptr);
    int             inetNetToMediaType_get(inetNetToMediaTable_rowreq_ctx *
                                           rowreq_ctx,
                                           u_long *
                                           inetNetToMediaType_val_ptr);
    int             inetNetToMediaState_get(inetNetToMediaTable_rowreq_ctx
                                            * rowreq_ctx,
                                            u_long *
                                            inetNetToMediaState_val_ptr);
    int
        inetNetToMediaRowStatus_get(inetNetToMediaTable_rowreq_ctx *
                                    rowreq_ctx,
                                    u_long *
                                    inetNetToMediaRowStatus_val_ptr);


    int
        inetNetToMediaTable_indexes_set_tbl_idx
        (inetNetToMediaTable_mib_index * tbl_idx,
         long inetNetToMediaIfIndex_val,
         u_long inetNetToMediaNetAddressType_val,
         char *inetNetToMediaNetAddress_val_ptr,
         size_t inetNetToMediaNetAddress_val_ptr_len);
    int
        inetNetToMediaTable_indexes_set(inetNetToMediaTable_rowreq_ctx *
                                        rowreq_ctx,
                                        long inetNetToMediaIfIndex_val,
                                        u_long
                                        inetNetToMediaNetAddressType_val,
                                        char
                                        *inetNetToMediaNetAddress_val_ptr,
                                        size_t
                                        inetNetToMediaNetAddress_val_ptr_len);



    /*
     *********************************************************************
     * SET function declarations
     */

    /*
     *********************************************************************
     * SET Table declarations
     */
/**********************************************************************
 **********************************************************************
 ***
 *** Table inetNetToMediaTable
 ***
 **********************************************************************
 **********************************************************************/
    /*
     * IP-MIB::inetNetToMediaTable is subid 35 of ip.
     * Its status is Current.
     * OID: .1.3.6.1.2.1.4.35, length: 8
     */


    int
        inetNetToMediaTable_undo_setup(inetNetToMediaTable_rowreq_ctx *
                                       rowreq_ctx);
    int
        inetNetToMediaTable_undo_cleanup(inetNetToMediaTable_rowreq_ctx *
                                         rowreq_ctx);
    int             inetNetToMediaTable_undo(inetNetToMediaTable_rowreq_ctx
                                             * rowreq_ctx);
    int
        inetNetToMediaTable_commit(inetNetToMediaTable_rowreq_ctx *
                                   rowreq_ctx);
    int
        inetNetToMediaTable_undo_commit(inetNetToMediaTable_rowreq_ctx *
                                        rowreq_ctx);


    int
        inetNetToMediaPhysAddress_check_value
        (inetNetToMediaTable_rowreq_ctx * rowreq_ctx,
         char *inetNetToMediaPhysAddress_val_ptr,
         size_t inetNetToMediaPhysAddress_val_ptr_len);
    int
        inetNetToMediaPhysAddress_undo_setup(inetNetToMediaTable_rowreq_ctx
                                             * rowreq_ctx);
    int
        inetNetToMediaPhysAddress_set(inetNetToMediaTable_rowreq_ctx *
                                      rowreq_ctx, char
                                      *inetNetToMediaPhysAddress_val_ptr,
                                      size_t
                                      inetNetToMediaPhysAddress_val_ptr_len);
    int
        inetNetToMediaPhysAddress_undo(inetNetToMediaTable_rowreq_ctx *
                                       rowreq_ctx);

    int
        inetNetToMediaLastUpdated_check_value
        (inetNetToMediaTable_rowreq_ctx * rowreq_ctx,
         u_long inetNetToMediaLastUpdated_val);
    int
        inetNetToMediaLastUpdated_undo_setup(inetNetToMediaTable_rowreq_ctx
                                             * rowreq_ctx);
    int
        inetNetToMediaLastUpdated_set(inetNetToMediaTable_rowreq_ctx *
                                      rowreq_ctx,
                                      u_long
                                      inetNetToMediaLastUpdated_val);
    int
        inetNetToMediaLastUpdated_undo(inetNetToMediaTable_rowreq_ctx *
                                       rowreq_ctx);

    int
        inetNetToMediaType_check_value(inetNetToMediaTable_rowreq_ctx *
                                       rowreq_ctx,
                                       u_long inetNetToMediaType_val);
    int
        inetNetToMediaType_undo_setup(inetNetToMediaTable_rowreq_ctx *
                                      rowreq_ctx);
    int             inetNetToMediaType_set(inetNetToMediaTable_rowreq_ctx *
                                           rowreq_ctx,
                                           u_long inetNetToMediaType_val);
    int             inetNetToMediaType_undo(inetNetToMediaTable_rowreq_ctx
                                            * rowreq_ctx);

    int
        inetNetToMediaState_check_value(inetNetToMediaTable_rowreq_ctx *
                                        rowreq_ctx,
                                        u_long inetNetToMediaState_val);
    int
        inetNetToMediaState_undo_setup(inetNetToMediaTable_rowreq_ctx *
                                       rowreq_ctx);
    int             inetNetToMediaState_set(inetNetToMediaTable_rowreq_ctx
                                            * rowreq_ctx,
                                            u_long
                                            inetNetToMediaState_val);
    int             inetNetToMediaState_undo(inetNetToMediaTable_rowreq_ctx
                                             * rowreq_ctx);

    int
        inetNetToMediaRowStatus_check_value(inetNetToMediaTable_rowreq_ctx
                                            * rowreq_ctx,
                                            u_long
                                            inetNetToMediaRowStatus_val);
    int
        inetNetToMediaRowStatus_undo_setup(inetNetToMediaTable_rowreq_ctx *
                                           rowreq_ctx);
    int
        inetNetToMediaRowStatus_undo(inetNetToMediaTable_rowreq_ctx *
                                     rowreq_ctx);


    int
        inetNetToMediaTable_check_dependencies
        (inetNetToMediaTable_rowreq_ctx * ctx);


    /*
     * DUMMY markers, ignore
     *
     * TODO:099:x: *************************************************************
     * TODO:199:x: *************************************************************
     * TODO:299:x: *************************************************************
     * TODO:399:x: *************************************************************
     * TODO:499:x: *************************************************************
     */

#ifdef __cplusplus
}
#endif
#endif                          /* INETNETTOMEDIATABLE_H */
/**  @} */

