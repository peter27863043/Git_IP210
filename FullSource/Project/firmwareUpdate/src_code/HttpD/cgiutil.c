/******************************************************************************
*
*   Name:           cgiutil.c
*
*   Description:    Handle cgi.c function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//*******************Include files area**************************
#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "uip.h"
#include "cgi.h"
#include "httpd.h"
//*******************Extern variable*****************************
#include "fs.h"
//********************************************
/******************************************************************************
*
*  Function:    itostr
*
*  Description: Transfer integer to string
*               
*  Parameters:  integer value
*               
*  Returns:     Address of the return string
*               
*******************************************************************************/
/*char *itostr(u8_t int_value)
{
	int i=0,num;
    static char ch[4];
	num=int_value;
	if((num/100)>0)
	{
		//100 
		ch[i]='0' + (num/100);
		num-=(num/100)*100;
		i++;
		//10
		ch[i]='0' + (num/10);
		num-=(num/10)*10;
		i++;
		//1
		ch[i]='0' + num;
		i++;
	}
	else
	{
		if((num/10)>0)
		{
			ch[i]='0' + (num/10);
			num-=(num/10)*10;
			i++;
			//1
			ch[i]='0' + num;
			i++;
		}
		else
		{
			ch[i]='0' + num;
			i++;
		}
	}
	ch[i]=0;
    return ch;
}*/
char *uitos(u16_t int_value)
{
	int i=0;
	static char str[6];
    int base=10000,last_div=0,div=0;
	while(base>0)
	{
		if((int_value/base)!=0)break;
		base/=10;
	}
	for(i=0;base>0;i++)
	{
		div=int_value/base;
		str[i]='0'+(div-last_div*10);
		last_div=div;
		base/=10;
	}
	if(i!=0)str[i]='\0';
	else strcpy(str,"0");
	return str;
}
u16_t stoui(char* str_value)
{
	u16_t value=0;
	int i,len;
	len=strlen(str_value);
	for(i=0;i<len;i++)
	{
		value=value*10+(str_value[i]-'0');
	}
	return value;
}
/******************************************************************************
*
*  Function:    no_tag_page
*
*  Description: Handle html files without any tag information. ex: logo.jpg
*               
*  Parameters:  None
*               
*  Returns:     If finish, return true.
*               
*******************************************************************************/
/*u8_t no_tag_page()
{

	if((hs->count+uip_len)>uip_conn->mss)
	{
		indata.len=(uip_conn->mss-uip_len);		
	    uip_len +=(uip_conn->mss-uip_len);	

	}
	else
	{
		indata.len=hs->count;			
	    uip_len +=hs->count;
	}		
	fsop[hs->op_index].Mem_copy(&indata);
	hs->html_current_pos+=indata.len;// add chance 20050810 PM 0612

	return 1;
}*/
/******************************************************************************
*
*  Function:    last_tag_page
*
*  Description: Handle html files which after the last tag information and the 
*
*               size of this part is smaller than MSS.
*               
*  Parameters:  None
*               
*  Returns:     If finish, return true.
*               
*******************************************************************************/
u8_t last_tag_page()
{
	if(((hs->hsfile->file_len-hs->html_current_pos)+uip_len)>uip_conn->mss)
	{
		indata.len=(uip_conn->mss-uip_len);		
	    uip_len +=(uip_conn->mss-uip_len);	
		hs->count_unack=uip_len;
	}
	else
	{
		indata.len=(hs->hsfile->file_len-hs->html_current_pos);		
	    uip_len +=(hs->hsfile->file_len-hs->html_current_pos);
		hs->count_unack=0;

	}		
	fsop[hs->op_index].Mem_copy(&indata);
	hs->html_current_pos+=indata.len;
	hs->dataptr+=indata.len;
	return 1;
}
/******************************************************************************
*
*  Function:    tag_address_bigger_mss
*
*  Description: Handle html file which the address of first tag is bigger 
*
*               than MSS.
*               
*  Parameters:  None
*               
*  Returns:     If finish, return true.
*               
*******************************************************************************/
u8_t tag_address_bigger_mss()
{				
	indata.len=(uip_conn->mss-uip_len);		
    uip_len +=(uip_conn->mss-uip_len);	
	fsop[hs->op_index].Mem_copy(&indata);
	hs->html_current_pos+=indata.len;
	hs->dataptr+=indata.len;
	hs->count_unack=uip_len;
	return 1;
}
/******************************************************************************
*
*  Function:    with_tag_page
*
*  Description: Handle html files which have tag informaiton
*               
*  Parameters:  None
*               
*  Returns:     If finish, return true.
*               
*******************************************************************************/
u8_t with_tag_page()
{
	data_value tag_value;
	u8_t tag_mss_flag=0,switch_varid;
	u16_t tag_value_len;
	char *tag_asc_value;

	do 
	{	
		if((fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos==0)&&(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).varid==0))
		{
			tag_mss_flag=0;
		}
		else
		{
			if((fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos+uip_len-hs->html_current_pos)<uip_conn->mss)
			{
				indata.len=(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos-hs->html_current_pos);		
			    uip_len +=(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos-hs->html_current_pos);	
				fsop[hs->op_index].Mem_copy(&indata);
				hs->html_current_pos+=indata.len;
				hs->dataptr+=indata.len;
				tag_value_len=0;
				switch_varid=fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).varid;
				switch(switch_varid)
				{
					case 0xfe:					
						hs->tag_flag++;
						if(hs->tag_flag>2)
						{
							hs->tag_flag=1;
						}
						break;
					case 0xf1:
						hs->tag_flag=1;
						hs->row_num=1;
						hs->table_flag=1;
					 	hs->tag_index_start=hs->tag_index;
						break;
					case 0xf2:
						hs->tag_flag=2;
						if(hs->table_flag)
						{							
							hs->col_num=0;
							hs->row_num++;
							if(HS->cgi_func.func(0xf2,&tag_value)==ERROR)
							{
								hs->table_flag=0;//remove the TABLE1
								hs->row_num--;
							}
							else
							{
								hs->tag_index=hs->tag_index_start;
								hs->tag_flag=1;
							}
						}
						break;
					case 0xf3:
						if(hs->table_flag)
						{
							hs->col_num=1;
							hs->table2_flag=1;
						 	hs->tag2_index_start=hs->tag_index;
						}
						break;
					case 0xf4:
							if(hs->table2_flag)
						{
							hs->col_num++;
							if(HS->cgi_func.func(0xf4,&tag_value)==ERROR)
							{
								hs->table2_flag=0;//remove the TABLE2
								hs->col_num--;	
							}
							else
							{
								hs->tag_index=hs->tag2_index_start;
							}
						}
						break;
					default:
						hs->tag_flag=1;
						if(hs->tag_flag==1)
						{				
							if(HS->cgi_func.func(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).varid,&tag_value)==OK)
							{										
								switch(tag_value.type)
								{
									case digits_3_int://1
										tag_asc_value=uitos(tag_value.value.digits_3_int);
										tag_value_len=strlen(tag_asc_value);
										break;
									case string://2
										tag_asc_value=tag_value.value.string;				
										tag_value_len=strlen(tag_asc_value);
										break;
								}
							}
						}																				
						break;
				}		
				if(((uip_len+tag_value_len)<uip_conn->mss))//||(hs->html_current_pos!=hs->fsfile->datalen))
				{
					if(tag_value_len!=0)
					{
						memcpy(&uip_appdata[uip_len],tag_asc_value,tag_value_len);
						uip_len+=tag_value_len;

					}
					if(hs->table_flag)
					{
						hs->html_current_pos=fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos;
						hs->dataptr=hs->hsfile->file_ptr;
						hs->dataptr+=fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos;
						
					}
					else
					{
					}
					tag_mss_flag=1;									
					indata.source_ptr=hs->dataptr;
					indata.dest_ptr=&uip_appdata[uip_len];
					indata.len=0;
					hs->tag_index++;
				}
				else
				{
					tag_mss_flag=0;
				}
			}
			else
			{
				tag_mss_flag=0;
			}							    					
		}		
	}while(tag_mss_flag);
	hs->count_unack=uip_len;
    last_tag_page();//sorbica071101
	return 1;
}
