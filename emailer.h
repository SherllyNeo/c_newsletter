#include "sql_connector.c"
#include "mail.c"

struct Email_Sub;

void get_email_subs(struct Email_Sub* email_sub_array,int* amount_of_subscribers,int size_of_array);


void send_email(struct Email_Sub email_sub);

