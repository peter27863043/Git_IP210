/******************************************************************************
*
*   Name:           uip_other.c
*
*   Description:    This file is not directerly related to uip, just for compatible issue.
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
int PKT_FLAGS;

/*bit eth_xmt(unsigned char* buf);//JC_MODIFY
void jc_eth_send()
{
	u8_t xdata* pTx=uip_buf_real;
	u8_t PktTypy;
	int old_len=uip_len;
	
	//padding
//	for(i=uip_len;i<64;i++)
//	{
//		pTx[i]=0;
//	}
	uip_len+=4;//JC_STOP??????:Is the EtherNet-CRC?
	if(uip_len<64)uip_len=64;
	
		
	if(pTx[2+12]==0x08)
	{
		if(pTx[2+13]==0)
		{//IP
			switch(pTx[2+23])
			{
			case 1: PktTypy=0x18; break; //ICMP
			case 6: PktTypy=0x08; break; //TCP
			case 17: PktTypy=0x10; break; //IP
			default: PktTypy=0; break;
			}
		}
		else if(pTx[2+13]==0x06)
		{//ARP			
			PktTypy=0x20;
		}
		else
		{
			PktTypy=0;
		}
	}
	else
	{
		PktTypy=0;
	}
	 
	 if(PktTypy==0x08 || PktTypy==0x10)PktTypy=0;
	 
    pTx[0]=0x80|PktTypy|((uip_len>>8)&0x07);
	 pTx[1]=uip_len&0xff;

    Tf320RegWrite(TF320_TXD0_LO,(int)uip_buf_real&0xff);
    Tf320RegWrite(TF320_TXD0_HI,((int)uip_buf_real>>8)&0xff);
	
	 eth_xmt(&uip_buf_real);
	 uip_len=0;
	 uip_slen=0;
}*/