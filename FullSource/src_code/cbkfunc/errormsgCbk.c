#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "type_def.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "eeprom.h"
#include "eepdef.h"
//include html files
#include "errormsg_htm.h"
#include "..\net_uart\dhcpc.h"
unsigned char errmsgflag = ERRORMSG_LOGIN ;
#ifdef MODULE_CLI_CUSTOMIAZE
#else
/******************************************************************************
*
*  Function:    ssi_errormsg
*
*  Description: Httpd will send error message to client, while error occurs.
*
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t ssi_errormsg(u8_t varid, data_value *vp)
{
    switch (varid)
    {
        case CGI_TITLE :
			vp->value.string="Error";
            if( errmsgflag == ERRORMSG_SETOK )
                vp->value.string = "Setting Saved";
            if( errmsgflag == ERRORMSG_SETNO )
                vp->value.string = "Setting Fail";
            if( errmsgflag == ERRORMSG_RESET || errmsgflag == ERRORMSG_NOIP)
            	vp->value.string = "";		            		
            break ;
        case CGI_ERROR_MESSAGE :
            vp->value.string = hs->errormsg;
            vp->type = string;
            break;
            
        case CGI_BNAME:
        	vp->value.string=" ok ";
        	if((errmsgflag == ERRORMSG_RESET)||(errmsgflag == ERRORMSG_SETOK))
        	{	vp->value.string="Reset";  	}
			vp->type = string;
        	break;
		case CGI_COLOR:
			vp->value.string ="00FF33";
			if( errmsgflag == ERRORMSG_SETNO)vp->value.string ="FF0033";
            vp->type = string;
			break;

        case CGI_PATH :
            if((errmsgflag == ERRORMSG_RESET)||(errmsgflag == ERRORMSG_SETOK))
			{
				vp->value.string = "/cgi/reset.cgi";
			}
            else if( (errmsgflag == ERRORMSG_CHALLENGE)||(errmsgflag == ERRORMSG_LOGIN)||(errmsgflag == ERRORMSG_NOIP) )
			{
				vp->value.string = "javascript:top.location='/login.htm'";
			}
			else
			{	vp->value.string="javascript:history.back()";	}	
            vp->type = string;
            break ;
		case CGI_LL:
			if(errmsgflag == ERRORMSG_NOIP)
			{	vp->value.string="<!--";	}
			else
			{	vp->value.string="";}
			vp->type=string;
			break;
		case CGI_RR:
			if(errmsgflag == ERRORMSG_NOIP)
			{	vp->value.string="-->";	}
			else
			{	vp->value.string="";}
			vp->type=string;
			break;
		case CGI_NAME:
            if((errmsgflag == ERRORMSG_RESET)||(errmsgflag == ERRORMSG_SETOK))
			{
				vp->value.string="reset";
			}
			else
			{	vp->value.string="";	}
			vp->type=string;
			break;
		case CGI_VALUE:
            if((errmsgflag == ERRORMSG_RESET)||(errmsgflag == ERRORMSG_SETOK)||(errmsgflag == ERRORMSG_NOIP))
			{	vp->value.string="ture";	}
			else
			{	vp->value.string="";	}
			vp->type=string;
			break;
    }
    return OK;
}
/******************************************************************************
*
*  Function:    eepromerr
*
*  Description: While read/write EEPROM fail, system will show error message
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void eepromerr()
{
	hs->errormsg = "Check EEPROM!";
	errmsgflag = ERRORMSG_EEP ;
	httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
}

/******************************************************************************
*
*  Function:    cgi_reset
*
*  Description: Handle Password request from client
*
*  Parameters:  cnt: number of the cgi data,
*				vp:  CGI name and value.
*					 vp[0] - username
*					 vp[1] - password one
*					 vp[2] - password two
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
extern u8_t force_reset_countdown;
extern u8_t ip_chged;
u8_t cgi_reset(u8_t cnt, req_data *vp)
{
	u8_t i;
	req_data* vp2=vp;
	char* filename="errormsg.htm";
	hs->errormsg = "Reset Device";
	errmsgflag = ERRORMSG_RESET;	
	for (i=0; i < cnt; i++, vp2++)
 	{
 		if(!strcmp(vp2->item, "reset"))
 		{ 
 			if((EEConfigInfo.DHCPEnable==sDHCP_DISABLE)&&(ip_chged==0))
 			{
 				force_reset_countdown=10;
				hs->errormsg = "Back to login";
				errmsgflag = ERRORMSG_NOIP;
  	 	 	}
  	 	 	else
  	 	 	{
  	 	 		force_reset_countdown=10;
				hs->errormsg = "Type IP in URL";
				errmsgflag = ERRORMSG_NOIP;
  	 	 	}
		}
  	}	
  	httpd_init_file(fs_open(filename,&hs->op_index));
	return OK;
}
#endif
