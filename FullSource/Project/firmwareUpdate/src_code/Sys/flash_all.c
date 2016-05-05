#include "timer.h"
#include "ip210reg.h"
/******************************************************************************
*
*  Function:    Flash_Switch_Bank
*
*  Description: to switch the bank number of flash that CPU fetches
*               
*  Parameters:  u8_t index(bank number 0-7)
*               
*  Returns: none
*  
*******************************************************************************/
void Flash_Switch_Bank(u8_t index)
{
    FLASH_ADDR_16 = index & 0x01;
    FLASH_ADDR_17 = (index>>1) &0x01;
    FLASH_ADDR_18 = (index>>2) &0x01;

    //if(Flash_Erase_Bankno>8)//16 bank avoid to change p4.4 if index<8
    {
      if((index>>3) &0x01)//for P4_4
      {
          FLASH_ADDR_19 = 0x1;
      }
      else
      {
          FLASH_ADDR_19 = 0x0;
      }
    }
    //if(Flash_Erase_Bankno>16)//32 bank avoid to change p4.5 if index<16
    {
      if((index>>4) &0x01)//for P4_5
      {
          FLASH_ADDR_20 = 0x1;
      }
      else
      {
          FLASH_ADDR_20 = 0x0;
      }
    }

}
	u8_t MX_toggle()//chance add 20100423 check toggle bit to know process finish
	{
	  u8_t toggle_a, toggle_b;
	  u8_t tog_a_bit, tog_b_bit;
	    //toggling process
	    do{
	        WatchDogTimerReset();
	        toggle_a = XBYTE[0x2222];
	        toggle_b = XBYTE[0x2222];
	        tog_a_bit = toggle_a & 0x40;
	        tog_b_bit = toggle_b & 0x40;
	        if(tog_a_bit==tog_b_bit)
	        {break;}
	        //if(MX_toggle_flag)
	        //{
	          if(toggle_a & 0x20)
	          {
	              toggle_a = XBYTE[0x2222];
	              toggle_b = XBYTE[0x2222];
	              tog_a_bit = toggle_a & 0x40;
	              tog_b_bit = toggle_b & 0x40;
	              if(tog_a_bit==tog_b_bit)
	              { break;}         
	              else
	              {
	                  //Flash_Access_Disable();
	                  return 0;
	              }
	          }        
	        //}
	    }while(1);
	    return 1;
	}
/*void Flash_Write_Active_MX29LV800CB()
{
  XBYTE[0xAAA] = 0xAA;
  XBYTE[0x555] = 0x55;
  XBYTE[0xAAA] = 0xA0;  
}

void Flash_Write_Active_SST39VF1681()
{
  XBYTE[0xAAA] = 0xAA;
  XBYTE[0x555] = 0x55;
  XBYTE[0xAAA] = 0xA5;  
}*/
/******************************************************************************
*
*  Function:    Flash_Write_Byte
*
*  Description: to write a byte of data to flash
*               
*  Parameters:
*    u16_t flash_addr: the address of data in Flash
*    u8_t byte_data: the data to be written
*           
*  Returns: 
*  1: succeeded
*  0: failed 
*  
*******************************************************************************/
u8_t Flash_Write_Byte(u16_t flash_addr, u8_t byte_data)
{
	u8_t read_data;
	u16_t ii;
	u16_t data flash_write_addr;

    flash_write_addr = flash_addr;

    DBYTE[TEMP_0] = (u8_t)(flash_addr&0xff);
    DBYTE[TEMP_1] = (u8_t)((flash_addr>>8)&0xff);    
    DBYTE[TEMP_2] = byte_data;    

     
    //printf("id=%x",(u16_t)Global_flash_ID);	      
  	switch(Global_flash_ID)
	{
	    case 0xbfd7://SST39VLF040         	    
            Flash_Access_Enable();          
            XBYTE[0x5555] = 0xAA;
            XBYTE[0x2AAA] = 0x55;            
            XBYTE[0x5555] = 0xA0; 
            Flash_Access_Disable();	    
	           break;
	    case 0x1f13://AT49BV040B
            Flash_Access_Enable();          
            XBYTE[0x555] = 0xAA;
            XBYTE[0xAAA] = 0x55;            
            XBYTE[0x555] = 0xA0; 	 
            Flash_Access_Disable();   
	           break;
      	case 0xc2b6://MX29LV004CB  
            Flash_Access_Enable();          
            XBYTE[0x555] = 0xAA;
            XBYTE[0x2AA] = 0x55;            
            XBYTE[0x555] = 0xA0;  
            Flash_Access_Disable();     
             break;	           
	    case 0xbf43://SST36VF160XX
	    case 0xbf44:
	    case 0xbf4a:
	    case 0xbf4b:
	           break;
	    case 0x7F5B://EN29LV800C 
		case 0x1CBA://EN29LV400AB
	    case 0x7F49://EN29LV160BB
	    case 0xc25b://MX29LV800CB
		case 0xc249://MX29LV160DB
		case 0xbfc8://SST39VF1681
		case 0xbfc9://SST39VF1682
            Flash_Access_Enable();          
            XBYTE[0xAAA] = 0xAA;
            XBYTE[0x555] = 0x55;
            XBYTE[0xAAA] = 0xA0; 
            Flash_Access_Disable();              
            break;                         			       
 	    default:
          	//printf("id=%x",(u16_t)Global_flash_ID);	 
	        break;
	}
    Flash_Access_Enable();
    XBYTE[flash_write_addr] = DBYTE[TEMP_2];
	//XBYTE[(DBYTE[TEMP_1]<<8)+DBYTE[TEMP_0]] = DBYTE[TEMP_2];
    //if(!MX_toggle()){ Flash_Access_Disable();return 0;}
    Flash_Access_Disable();
    //delay_ms(60);
    for(ii=0;ii<600000;ii++)
    {
        read_data = Flash_Read_Byte(flash_addr);
        if(read_data == byte_data)
        { 	//printf("\r%X",(u16_t)read_data);    
          	break;
		}
    }
    return (ii==600000)?0:1;
    //return 1;
}


/******************************************************************************
*
*  Function:    Flash_Erase_Sector
*
*  Description: to erase 4k flash data sector where index refers to in current bank of flash
*               
*  Parameters:  u8_t index(index 0~15 sector)
*               
*  Returns: 
*  1: succeeded           
*  0: failed
*******************************************************************************/
u8_t Flash_Erase_Sector_AT49BV040B_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
if(!MX_toggle()){Flash_Access_Disable();return 0;}      
    Flash_Access_Disable();

    
return 1;
}
u8_t Flash_Erase_Sector_AT49BV040B_BottomSector_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      /*case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;
	               */
      }
if(!MX_toggle()){Flash_Access_Disable();return 0;}
    Flash_Access_Disable();
    
return 1;

}
u8_t Flash_Erase_Sector_MX29LV004CB_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
      if(!MX_toggle()){Flash_Access_Disable();return 0;}
    Flash_Access_Disable();

	return 1;
}
u8_t Flash_Erase_Sector_MX29LV800CB_while(u8_t index)
{
  //u8_t i;
  //u8_t toggle_a, toggle_b;
  //u8_t tog_a_bit, tog_b_bit;
//  u8_t data byte_data1, byte_data2;

    Flash_Access_Enable();	
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
      XBYTE[0xAAA] = 0x80;
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  

    //toggling process
    /*do{
        toggle_a = XBYTE[0x2222];
        toggle_b = XBYTE[0x2222];
        tog_a_bit = toggle_a & 0x40;
        tog_b_bit = toggle_b & 0x40;
        if(tog_a_bit==tog_b_bit)
            break;

        if(toggle_a & 0x20)
        {
            toggle_a = XBYTE[0x2222];
            toggle_b = XBYTE[0x2222];
            tog_a_bit = toggle_a & 0x40;
            tog_b_bit = toggle_b & 0x40;
            if(tog_a_bit==tog_b_bit)
            { break;}         
            else
            {
                Flash_Access_Disable();
                return 0;
            }
        }        
    }while(1);*/
    if(!MX_toggle()){Flash_Access_Disable();return 0;}
    Flash_Access_Disable();
    return 1;
}
u8_t Flash_Erase_Sector_MX29LV004CB_BottomSector_while(u8_t index)
{
  u8_t i;
  u8_t data byte_data1, byte_data2;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      /*case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;*/
      }
    if(!MX_toggle()){Flash_Access_Disable();return 0;}
    Flash_Access_Disable();
 return 1;

}
u8_t Flash_Erase_Sector_MX29LV800CB_BottomSector_while(u8_t index)
{
  u8_t i;
  u8_t data byte_data1, byte_data2;

   Flash_Access_Enable();	
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
      XBYTE[0xAAA] = 0x80;
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      /*case 0x2://because FW 40k
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;*/
      }
    if(!MX_toggle()){Flash_Access_Disable();return 0;}
    Flash_Access_Disable();
    //delay 2s
	/*for(i=0;i<4;i++)
	{
        delay_ms(500);    
//        Flash_Access_Enable();	
//        byte_data1 = XBYTE[0x555];   
//        byte_data2 = XBYTE[0x555];   
//        Flash_Access_Disable();
//		printf("\n FESW[%02x][%02x] ", (u16_t)byte_data1, (u16_t)byte_data1);
	}*/
   
return 1;

}
u8_t Flash_Erase_Sector_SST39VF1681(u8_t index)
{
  u16_t data flash_addr;
  u16_t ii;
//  u8_t  times;

//    times=SECTOR_NUMBER_OF_4K_SIZE;
//do{
    flash_addr = 0x1000*index;   
//    DBYTE[TEMP_0] = (u8_t)(flash_addr&0xff);
//    DBYTE[TEMP_1] = (u8_t)((flash_addr>>8)&0xff);  

    Flash_Access_Enable();	
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
      XBYTE[0xAAA] = 0x80;
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
	  
      XBYTE[flash_addr] = 0x50;  
    //MX_toggle();    
    Flash_Access_Disable();
    
    //delay 20ms
    delay_ms(60);
    
//    times--;
//}while(times>0);

return 1;
}
u8_t Flash_Erase_bank_SST39VF1681()//u8_t index)
{
  u16_t data flash_addr;
  u16_t ii;
//  u8_t  times;

//    times=SECTOR_NUMBER_OF_4K_SIZE;
//do{
    flash_addr = 0x0;   
//    DBYTE[TEMP_0] = (u8_t)(flash_addr&0xff);
//    DBYTE[TEMP_1] = (u8_t)((flash_addr>>8)&0xff);  

    Flash_Access_Enable();	
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
      XBYTE[0xAAA] = 0x80;
      XBYTE[0xAAA] = 0xAA;
      XBYTE[0x555] = 0x55;
	  
      XBYTE[flash_addr] = 0x30;  
    //MX_toggle();    
    Flash_Access_Disable();
    
    //delay 20ms
    delay_ms(60);
    
//    times--;
//}while(times>0);

return 1;
}
u8_t Flash_Erase_Sector_SST39VLF040(u8_t index)
{
	u16_t data flash_addr;
	u16_t ii;
	u8_t  times;
	
	times=SECTOR_NUMBER_OF_4K_SIZE; //4
	do{
		flash_addr=0x1000*index;//+SECTOR_SIZE*(4-times);

		Flash_Access_Enable();	
		XBYTE[0x5555] = 0xAA;
		XBYTE[0x2AAA] = 0x55;
		XBYTE[0x5555] = 0x80;
		XBYTE[0x5555] = 0xAA;
		XBYTE[0x2AAA] = 0x55;
		
		XBYTE[flash_addr] = 0x30;        
		Flash_Access_Disable();
    
		delay_ms(60);
    
		times--;
	}while(times>0);
	return 1;
}


/******************************************************************************
*
*  Function:    FirmwareUpdateEraseFlash
*
*  Description: to erase all flash except for firmware update program sectors
*               
*  Parameters: none
*            
*  Returns: none
*  
*******************************************************************************/
void FirmwareUpdateEraseFlash_MX29LV004CB_while()
{
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2000);
       Flash_Switch_Bank(next_erase_bank);   	
	   if(next_erase_bank==0)
	   {
           if(next_erase_sector==0)
		   {
               Flash_Erase_Sector_MX29LV004CB_BottomSector_while(0);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=0;
               next_erase_sector=4;
		   }
           else
	       if(next_erase_sector==4)
		   {
               Flash_Erase_Sector_MX29LV004CB_BottomSector_while(1);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }
           /*else
	       if(next_erase_sector==6)
		   {
               Flash_Erase_Sector_MX29LV004CB_BottomSector_while(2);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }*/
           Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;
    Stop_Mirror_Mode();
		   return;
	   }
	   Flash_Erase_Sector_MX29LV004CB_while(next_erase_bank);
	     if(!next_erase_sector)
	     {printf("\n");}
       printf("\r b[%d]", (u16_t)next_erase_bank);		   
       Flash_Switch_Bank(0); 
    Stop_Mirror_Mode();
       next_erase_bank+=1;
       Phase_Flash_Erase=next_erase_bank*16;

}
void FirmwareUpdateEraseFlash_AT49BV040B_while()
{
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
       Flash_Switch_Bank(next_erase_bank);   	
	   if(next_erase_bank==0)
	   {
           if(next_erase_sector==0)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(0);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=0;
               next_erase_sector=4;
		   }
           else
	       if(next_erase_sector==4)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(1);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }
          /* else
	       if(next_erase_sector==6)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(2);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }*/
           Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;
    Stop_Mirror_Mode();
		   return;
	   }
	   Flash_Erase_Sector_AT49BV040B_while(next_erase_bank);
	     if(!next_erase_sector)
	     {printf("\n");}
       printf("\r b[%d]", (u16_t)next_erase_bank);		   
       Flash_Switch_Bank(0); 
    Stop_Mirror_Mode();
       next_erase_bank+=1;
       Phase_Flash_Erase=next_erase_bank*16;
}

void FirmwareUpdateEraseFlash_MX29LV800CB_while()//16,8,8,32,64~~~~~~~
{
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
	//printf("\n 1 b[%d]", (u16_t)next_erase_bank);
	Flash_Switch_Bank(next_erase_bank);   	
	if(next_erase_bank==0)
	{
		if(next_erase_sector==0)
		{
			Flash_Erase_Sector_MX29LV800CB_BottomSector_while(0);//(0)16k, (1)8k, (2)8k, (3)32k
			next_erase_bank=0;
			next_erase_sector=4;
		}
		else if(next_erase_sector==4)
		{
			Flash_Erase_Sector_MX29LV800CB_BottomSector_while(1);//(0)16k, (1)8k, (2)8k, (3)32k
			next_erase_bank=1;
			next_erase_sector=0;
		}
		/*else if(next_erase_sector==6)//because FW 40K
		{
		Flash_Erase_Sector_MX29LV800CB_BottomSector_while(2);//(0)16k, (1)8k, (2)8k, (3)32k
		next_erase_bank=1;
		next_erase_sector=0;
		}*/
		Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;
		Stop_Mirror_Mode();
		return;
	}	
	Flash_Erase_Sector_MX29LV800CB_while(next_erase_bank);
	printf("\nb[%d]", (u16_t)next_erase_bank);
	Flash_Switch_Bank(0); 
	Stop_Mirror_Mode();
	next_erase_bank+=1;
	Phase_Flash_Erase=next_erase_bank*16;
}
void FirmwareUpdateEraseFlash_SST39VLF040_while()
{
	u8_t i=0,j=0;
	if(next_erase_sector==0)
		printf("\n\rb[%d]",(u16_t)next_erase_bank);
	//printf("\nFirmwareUpdateEraseFlash_SST39VLF040(%x)(%x)", (u16_t)next_erase_bank, (u16_t)next_erase_sector);	     
	i=(next_erase_sector+1)/16;	//Bank
	j=(next_erase_sector+1)%16;	//Sector
	if(next_erase_bank==0 && next_erase_sector>5)//don't erase secoter 6-15
	{
		next_erase_bank+=i;
		next_erase_sector=j;
		return;
	}
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
	Flash_Switch_Bank(next_erase_bank);
	switch(Global_flash_ID)
	{   	
		case 0xbfd7:
			Flash_Erase_Sector_SST39VLF040(next_erase_sector);
			break;
		case 0xbfc8:
		case 0xbfc9:   
			if(!next_erase_bank)
			{      
				Flash_Erase_Sector_SST39VF1681(next_erase_sector);
			}
			else
			{	//printf("+bank");
				Flash_Erase_bank_SST39VF1681();
			}   
			break;	       
	}
	//if(!next_erase_sector)
	//{printf("\n");}
	//printf("\r b[%d]", (u16_t)next_erase_bank);	     
	Flash_Switch_Bank(0); 
	Stop_Mirror_Mode();
	if((Global_flash_ID==0xbfd7)||(next_erase_bank==0))
	{
		next_erase_bank+=i;
		next_erase_sector=j;     
	}
	else
	{
		next_erase_bank+=1;
		next_erase_sector=0;      
	}
	Phase_Flash_Erase=next_erase_bank*16;
	Phase_Flash_Erase+=next_erase_sector;
}
void Flash_CHECK_SIZE()
{
	u8_t tmp_Bankno=0;

	Flash_Switch_Bank(1);
	tmp_Bankno=Flash_Read_Byte(0xfffb);//get real file bank number at 0x1FFFB
	if((tmp_Bankno!=0)&&(tmp_Bankno!=0xFF)&&(tmp_Bankno<=Flash_Erase_Bankno)&&((tmp_Bankno%8)==0))
	{
		Flash_Bankno=tmp_Bankno;
	}
	Flash_Switch_Bank(0);
}
void FlashIdentification(void)
{
	u8_t data manufacturer=0x0, chipID=0x0,chipID0=0x0;
	u8_t id_flag=1;

	//check Flash ID phase 1 
	Flash_Access_Enable();
	XBYTE[0x5555] = 0xAA;
	XBYTE[0x2AAA] = 0x55;
	XBYTE[0x5555] = 0x90;
	Delay_10ms(3);
	manufacturer=XBYTE[0x0];
	chipID=XBYTE[0x1];
	XBYTE[0x5555] = 0xAA;
	XBYTE[0x2AAA] = 0x55;
	XBYTE[0x5555] = 0xF0;
	Delay_10ms(3);
	Flash_Access_Disable();
    //printf("\nM=[%02x], cID[%02x]", (unsigned short)manufacturer, (unsigned short)chipID);
    Global_flash_ID=((u16_t)manufacturer)<<8 | (u16_t)chipID;    
	switch(Global_flash_ID)
	{
	    case 0xbfd7://512K phase 1
			Global_erase_flash_wait_time=10;//1500ms
			Flash_Bankno = 8;
			//printf("\nSST39VLF040");
			break;
	    case 0xc2b6://512K phase 1
			Global_erase_flash_wait_time=500;//3000ms
			Flash_Bankno = 8;
			//printf("\nMX29LV004CB");
			break; 			       
	    case 0x1f13://512K phase 1
			Global_erase_flash_wait_time=500;//3000ms
			Flash_Bankno = 8;
			//printf("\nAT49BV040B");
			break;
	    case 0xbf43://2m phase 1
	    case 0xbf44:
	    case 0xbf4a:
	    case 0xbf4b:
			Global_erase_flash_wait_time=200;//2000ms
			Flash_Bankno = 32;
			//printf("\nSST36VF160XX");	    
			break;               			       
		/*
		case 0x4003:
		case 0x4063:
		case 0x40A3:
			Global_erase_flash_wait_time=10;//100ms
			//printf("\nSyncMOS");
			break;
		*/
	    default:
            id_flag=0;
			//printf("\n Unknown FlashID[%04x]!!", (u16_t)Global_flash_ID);
			break;
	}

  	if(!id_flag)
  	{   
		id_flag=1;
      	manufacturer=0x0, chipID=0x0;
      	Flash_Access_Enable();
      	XBYTE[0xAAA] = 0xAA;
      	XBYTE[0x555] = 0x55;
      	XBYTE[0xAAA] = 0x90;
      	Delay_10ms(3);
      	manufacturer=XBYTE[0x0];
      	chipID0=XBYTE[0x1];
      	chipID=XBYTE[0x2];
      	XBYTE[0xAAA] = 0xAA;
      	XBYTE[0x555] = 0x55;
      	XBYTE[0xAAA] = 0xF0;
      	Delay_10ms(3);
      	Flash_Access_Disable();         	    
    	//printf("\nManufacturer=[%02x], chipID[%02x]", (unsigned short)manufacturer, (unsigned short)chipID);
    	Global_flash_ID=((u16_t)manufacturer)<<8 | (u16_t)chipID;
	  	switch(Global_flash_ID)
	  	{
	    	case 0x7F49://2m phase 2
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 32;
				//printf("\nEN29LV160BB");
				break;  
	    	case 0x7F5B://1m phase 2
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 16;
				//printf("\nEN29LV800C");
				break;                           			       
			case 0xc25b://1m phase 2
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 16;
				//printf("\nMX29LV800CB");
				break;
			case 0xc249://2m phase 2
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 32;
				//printf("\nMX29LV160DB");
				break; 			       
			/*
			case 0xbfc8://2m phase 2
			case 0xbfc9:	    
				//Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 32;
				printf("\nSST39VF1681");
				break;
			*/
 	    	default:
 	          	id_flag=0;
		        //printf("\n Unknown FlashID[%04x]!!", (u16_t)Global_flash_ID);
		        break;
    	}
  	}
    if(!id_flag)
    { 
		id_flag=1;
		Global_flash_ID=((u16_t)manufacturer)<<8 | (u16_t)chipID0;  
  	  	switch(Global_flash_ID)
  	  	{ 
  	    	case 0xbfc8://2m phase 2
        	case 0xbfc9:	    
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno = 32;
				//printf("\nSST39VF1681");
				break;
			default:
				id_flag=0;
				//printf("\n Unknown FlashID[%04x]!!", (u16_t)Global_flash_ID);
				break;
  	  	}
	}
	if(!id_flag){
		id_flag=1;

      	manufacturer=0x0, chipID=0x0;
      	Flash_Access_Enable();
      	XBYTE[0xAAA] = 0xAA;
      	XBYTE[0x555] = 0x55;
      	XBYTE[0xAAA] = 0x90;
      	Delay_10ms(3);
      	manufacturer=XBYTE[0x200];
      	chipID0=XBYTE[0x1];
      	chipID=XBYTE[0x2];
      	XBYTE[0xAAA] = 0xAA;
      	XBYTE[0x555] = 0x55;
      	XBYTE[0xAAA] = 0xF0;
      	Delay_10ms(3);
      	Flash_Access_Disable();         	    
    	//printf("\nManufacturer=[%02x], chipID[%02x]", (unsigned short)manufacturer, (unsigned short)chipID);
    	Global_flash_ID=((u16_t)manufacturer)<<8 | (u16_t)chipID;

		switch(Global_flash_ID){
			case 0x1CBA:
				Global_erase_flash_wait_time=200;//2000ms
				Flash_Bankno=8;
				//printf("\n\rEN29LV400AB");
				break;
			default:
				id_flag=0;
				break;
		}
	}
    //chance add for file bank 20100423
    if(id_flag==1)
    {
		//printf("+bankno");
		Flash_Erase_Bankno=Flash_Bankno;
		Flash_CHECK_SIZE();
		/*Flash_Switch_Bank(1);
		tmp_Bankno=Flash_Read_Byte(0xfffb);//get real file bank number at 0x1FFFB
		if((tmp_Bankno!=0)&&(tmp_Bankno<Flash_Bankno)&&((tmp_Bankno%8)==0))
		{
			Flash_Bankno=tmp_Bankno;
		}
		Flash_Switch_Bank(0);*/
		//printf("\nbank=%d",(u16_t)Flash_Bankno);
    }
    //---------------------------  
}


#define FirmwareUpdateEraseFlash_S29C31004T_while FirmwareUpdateEraseFlash_SST39VLF040_while
//void FirmwareUpdateEraseFlash_While(void)
u8_t FirmwareUpdateEraseFlash_While(void)
{
    if(Phase_Flash_Erase==(Flash_Erase_Bankno*16))
		return 0;//chance 20100423

    IP210_Update_Timer_Counter();
    if (Delta_Time(last_timercounter) < Global_erase_flash_wait_time) 
        return;
    last_timercounter = timercounter;

    
    switch(Global_flash_ID)
	{
		case 0xbfd7://SST39VLF040
			FirmwareUpdateEraseFlash_SST39VLF040_while();
			break;
		/*
		case 0x4003:
		case 0x4063:
		case 0x40A3:
			FirmwareUpdateEraseFlash_S29C31004T_while();
			break;
		*/
		case 0xc2b6://MX29LV004CB 
			FirmwareUpdateEraseFlash_MX29LV004CB_while();
			break;
		case 0x1f13://AT49BV040B
			FirmwareUpdateEraseFlash_AT49BV040B_while();
			break;
		case 0xbf43://SST36VF160XX
		case 0xbf44:
		case 0xbf4a:
		case 0xbf4b:
			break;                         
		case 0x7F5B://EN29LV800C
		case 0x7F49://EN29LV160BB
		case 0x1CBA://EN29LV400AB
		case 0xc249://MX29LV160DB
		case 0xc25b://MX29LV800CB
			FirmwareUpdateEraseFlash_MX29LV800CB_while();               
			break;
		case 0xbfc8://SST39VF1681
		case 0xbfc9:
			// MX_toggle_flag=0;      
			FirmwareUpdateEraseFlash_SST39VLF040_while();      
			break;
		default:
			break;
	}
  	return 1;
}


/******************************************************************************
*
*  Function:    FirmwareUpdateWriteData
*
*  Description: to write data into Flash when doing Firmware update
*               
*  Parameters:
*    u8_t *ptr_data: the begining pointer of datas to be written into Flash
*    u16_t length: the length of data to be written
*            
*  Returns: none
*  
*******************************************************************************/
u8_t FirmwareUpdateWriteData(u8_t *ptr_data, u16_t length)
{
  u16_t ii;

	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);

    for(ii=0;ii<length;ii++)
    {
        if( (Global_bank_index==0)&&(Global_flash_addr>=0x6000) )
        {}
        else
        {
            Flash_Switch_Bank(Global_bank_index);
	        if(Flash_Write_Byte(Global_flash_addr, ptr_data[ii])!=1)
	        break;
	     }
        Global_flash_count++;Global_flash_addr++;
        if(Global_flash_addr==0) Global_bank_index++;
    }
    //printf("\n\r Global_flash_addr=%ul",(u32_t)Global_flash_addr);
    Stop_Mirror_Mode();
    return (ii==length)?1:0;

}
u8_t CBK_Flash_Size_Check()
{
  FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
  Flash_CHECK_SIZE();
  Stop_Mirror_Mode();
  return 1;
}