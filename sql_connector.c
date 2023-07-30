#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include "shared.h"

void get_email_subs(struct Email_Sub* email_sub_array,int* amount_of_subscribers,int size_of_array) {
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

const char *server = getenv("DB_HOST");
unsigned int port = atoi(getenv("DB_PORT"));
char *user = "sherlly";
const char *password = getenv("DB_PASS");
char *database = "sherlly_db";

if (server == NULL || password == NULL) {
	fprintf(stderr,"Cannot get connection details for DB");
	exit(1);
}

printf("Getting subs from \nServer: %s...\n",server,port,password);

conn = mysql_init(NULL);
/* Connect to database */
if (!mysql_real_connect(conn, server,
 user, password, database, port, NULL, 0)) {
fprintf(stderr, "%s\n", mysql_error(conn));
exit(1);
}

/* send SQL query */
if (mysql_query(conn, "SELECT Email,FirstName,LastName,AdditonalText,Template FROM Mailing_list WHERE Active=1")) {
fprintf(stderr, "%s\n", mysql_error(conn));
exit(1);
}

res = mysql_use_result(conn);

int email_sub_count = 0;

/* Collect data */
while ((row = mysql_fetch_row(res)) != NULL) {
	if (email_sub_count > size_of_array) {
		size_of_array *= 2;
		email_sub_array = malloc(size_of_array * sizeof(struct Email_Sub));
	}

	char *email = (char*)malloc(50 * sizeof(char));
	sprintf(email, "%s", row[0]);

	char *firstName = (char*)malloc(50 * sizeof(char));
	sprintf(firstName, "%s", row[1]);

	char *lastName = (char*)malloc(50 * sizeof(char));
	sprintf(lastName, "%s", row[2]);

	char *template = (char*)malloc(50 * sizeof(char));
	sprintf(template, "%s", row[4]);

	char *additionalText = (char*)malloc(50 * sizeof(char));
	sprintf(additionalText, "%s", row[3]);

	struct Email_Sub email_sub = {template,firstName,lastName,additionalText,email};
	email_sub_array[email_sub_count] = email_sub;
	email_sub_count++;

}


/* close connection */
mysql_free_result(res);
mysql_close(conn);

*amount_of_subscribers=email_sub_count;

}

