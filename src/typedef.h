#ifndef __TYPEDEF__
struct ilm_struct
{
    unsigned short src_mod_id;
    unsigned short dest_mod_id;
    unsigned short sap_id;
    unsigned short msg_id;
    unsigned int local_para_ptr;
    unsigned int peer_buff_ptr;
};
#endif