#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <curl/curl.h>
#include "emailer.h"
#include "shared.h"




int main() {

	int amount_of_subs=0;
	int* subs = &amount_of_subs;
	int size_of_array = 30;
	struct Email_Sub* email_sub_array = malloc(size_of_array * sizeof(struct Email_Sub));
	get_email_subs(email_sub_array,subs,size_of_array);
	printf("Sending to %d subscribers\n",amount_of_subs);

	/* Array is freed in here */
	for (int i=0;i<amount_of_subs;i++){
		struct Email_Sub es = email_sub_array[i];
 		printf("\n%d\n",i);
	}
	send_email_to_subs(email_sub_array,amount_of_subs);

	return 0;

}

