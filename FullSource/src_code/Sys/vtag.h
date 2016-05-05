void Check_Vlan_Tag();
void re_init();
u8_t *vlan_tag_pkt(u8_t *input_buf, u16_t *len_ptr);
u8_t EnVlanCheck();


extern u8_t eth_vlan;
extern u8_t vlan_value[4];

