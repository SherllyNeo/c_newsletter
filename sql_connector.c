#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include "shared.h"

#define SIZE 100

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

	/* make email sub */
	struct Email_Sub email_sub;
	strncpy(email_sub.Email,row[0],SIZE);
	strncpy(email_sub.FirstName,row[1],SIZE);
	strncpy(email_sub.LastName,row[2],SIZE);
	strncpy(email_sub.AdditionalText,row[3],SIZE);
	strncpy(email_sub.Temp,row[4],SIZE);
	email_sub_array[email_sub_count] = email_sub;
	email_sub_count++;

}


/* close connection */
mysql_free_result(res);
mysql_close(conn);

*amount_of_subscribers=email_sub_count;

}

