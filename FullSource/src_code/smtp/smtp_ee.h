//#ifdef MODULE_SMTP //---MODULE_SMTP.Begin---
#define MAX_MAIL_TO_ADDR_LEN 201
#define MAX_MAIL_ADDR_LEN     61
#define MAX_MAIL_SUBJ_LEN     61
#define MAX_MAIL_BODY_LEN     101
#define MAX_MAIL_SUBJ 3

typedef struct _SSMTPInfo
{
u8_t  SMTPEnable;                                        
u8_t  AuthEnable;                                        
u8_t  AuthID[31];                                        
u8_t  AuthPW[31];                                        
u16_t SMTPPort;                                          
u8_t  SMTPServer[MAX_MAIL_ADDR_LEN];                     
u8_t  MAIL_TO[MAX_MAIL_TO_ADDR_LEN];                     
u8_t  MAIL_FROM[MAX_MAIL_ADDR_LEN];                       
u8_t  MAIL_SUBJ[MAX_MAIL_SUBJ][MAX_MAIL_SUBJ_LEN]; 
u8_t  MAIL_MSG[MAX_MAIL_SUBJ][MAX_MAIL_BODY_LEN];  
}SSMTPInfo;                                        
//#else   //---MODULE_SMTP.Else---
//#endif  //---MODULE_SMTP.End---