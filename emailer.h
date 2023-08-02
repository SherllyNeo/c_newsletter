#include "mail.c"
#include "sql_connector.c"
#include "shared.h"


void get_email_subs(struct Email_Sub* email_sub_array,int* amount_of_subscribers,int size_of_array);
void send_email_to_subs(struct Email_Sub* email_sub_array, int amount_of_subs);

