#include "mail.c"
#include "sql_connector.c"
#include "shared.h"


struct Email_Sub* get_email_subs(int* amount_of_subscribers);
void send_email_to_subs(struct Email_Sub* email_sub_array, int amount_of_subs);

