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
	int size_of_array = 10;
	struct Email_Sub* email_sub_array = malloc(size_of_array * sizeof(struct Email_Sub));
	get_email_subs(email_sub_array,subs,size_of_array);
	printf("Sending to %d subscribers\n",amount_of_subs);

	for (int i=0; i<amount_of_subs;i++) {
		struct Email_Sub email_sub = email_sub_array[i];

		/* send email */
		send_email(email_sub);
	}

	if (email_sub_array) {
		for (int i=0; i<amount_of_subs;i++) {
		struct Email_Sub email_sub = email_sub_array[i];
		if (email_sub.Email) {
			free(email_sub.Email);
			free(email_sub.Temp);
			free(email_sub.FirstName);
			free(email_sub.LastName);
			free(email_sub.AdditionalText);
		}
		}
		free(email_sub_array);
	}

	return 0;

}

