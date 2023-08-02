#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include "shared.h"
#include <stdbool.h>
#include "config.h"


#define fact_file "facts_store.txt"
#define comps_file "compliments_store.txt"


char* read_attachment(char* file_path) {
	char * buffer = 0;
	long length;
	FILE * f = fopen(file_path, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length*2);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}

	if (buffer)
	{
		buffer[length] = '\0';
		return buffer;
	}
	return NULL;


}

int count_lines_of_file(char* file_path) {
	FILE * fp;
	int lines = 0;
	char ch;
	if ((fp = fopen(file_path, "r")) == NULL) {
		fprintf(stderr,"Error! opening file");
		exit(0);
	    }

	while(!feof(fp))
	{
	  ch = fgetc(fp);
	  if(ch == '\n')
	  {
	    lines++;
	  }
	}

	return lines;

}

void get_random_word(char* file,char* buffer) {
    FILE *words;
    size_t lineno = 0;
    size_t selectlen;
    char selected[SELECTION_SIZE];
    char current[SELECTION_SIZE];
    int lines = count_lines_of_file(file);
    int chosen_line = rand() % lines;

    if ((words = fopen(file, "r")) == NULL) {
        fprintf(stderr,"Error! opening file");
        exit(0);
    }
    while (fgets(current, sizeof(current), words)) {
	strcpy(selected, current);
	if (chosen_line == lineno) {
		break;
	}
	++lineno;

	}
    fclose(words);
    selectlen = strlen(selected);
    if (selectlen > 0 && selected[selectlen-1] == '\n') {
        selected[selectlen-1] = '\0';
    }
    strncpy(buffer,selected,SELECTION_SIZE);

}

struct upload_status {
  size_t bytes_read;
};

char payload_text[FULL_PAYLOAD_SIZE];


static size_t payload_source(char *ptr,size_t size,size_t nmemb,void *userp) {
struct upload_status *upload_ctx = (struct upload_status *)userp;
const char *data;
  size_t room = size * nmemb;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  data = &payload_text[upload_ctx->bytes_read];

  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;

    return len;
  }

  return 0;

}



void send_email(struct Email email) {

	char to_email[SIZE];
	char to[SIZE];
	char subject[SUBJECT_SIZE];
	char body[BODY_SIZE];
	bool has_attachment = false;
	int attachment_size_limit = 24000;
	char attachment_content[attachment_size_limit];
	char* attachment_file;
	char* attachment_file_name;

	sprintf(to_email,"<%s>",email.To_addr);
	sprintf(to,"%s <%s>",email.To_name,email.To_addr);

	if (email.Attachment_name && email.Attachment_path) {

	char* attachment_buffer = read_attachment(email.Attachment_path);
	strncpy(attachment_content,attachment_buffer,attachment_size_limit);

	sprintf(payload_text,"To: %s \r\nFrom:%s \r\nCc: %s \r\nMIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n\tboundary=\"XXXXboundary text\"\r\nSubject: %s \r\n\r\n--XXXXboundary text\r\nContent-Type: text/plain\r\n\r\n %s \r\n\r\n--XXXXboundary text\r\nContent-Type: text/plain;\r\nContent-Disposition: attachment;\r\n\tfilename=\"%s\"\r\n\r\n %s \r\n\r\n--XXXXboundary text--\r\n",to,from,email.Cc_addr,email.Subject,email.Body,email.Attachment_name,attachment_content);
	}
	else {
	sprintf(payload_text,"To: %s \r\nFrom: %s \r\nCc: %s \r\nSubject: %s \r\n\r\n %s \r\n\r\n \r\n ",to,from,email.Cc_addr,email.Subject,email.Body);
	}

	CURL *curl;
	CURLcode res_ = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx = { 0 };

	curl = curl_easy_init();
	  if(curl) {
	    char* EMAIL_USER = getenv("EMAIL_USER");
	    char* EMAIL_PASS = getenv("EMAIL_PASS");
	    char* EMAIL_SMTP = getenv("EMAIL_SMTP");
	    if (EMAIL_USER == NULL || EMAIL_PASS == NULL || EMAIL_SMTP == NULL) {
		    fprintf(stderr,"Could not find env variables\n");
		    exit(0);
	    }
	    curl_easy_setopt(curl, CURLOPT_USERNAME, EMAIL_USER);
	    curl_easy_setopt(curl, CURLOPT_PASSWORD, EMAIL_PASS);

	    curl_easy_setopt(curl, CURLOPT_URL, EMAIL_SMTP);

	    curl_easy_setopt(curl,CURLOPT_USE_SSL,(long)CURLUSESSL_ALL);

	    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_mail);

	    recipients = curl_slist_append(recipients, to_email);
	    recipients = curl_slist_append(recipients, Cc_addr);
	    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
	    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
	    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	     /* Send the message */
	    res_ = curl_easy_perform(curl);

	    /* Check for errors */
	    if(res_ != CURLE_OK) {
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",
		      curl_easy_strerror(res_));
	    }
	    else {
	    	printf("\nsent email - email to -->  %s with email: %s\n",email.To_name,email.To_addr);
	    }

	    /* Free the list of recipients */
	    curl_slist_free_all(recipients);

	    curl_easy_cleanup(curl);
	  }
}









void send_email_to_subs(struct Email_Sub* email_sub_array, int amount_of_subs) {
	srand(time(NULL));

	/* choose fact */
	char fact[SELECTION_SIZE];
	get_random_word(fact_file,fact);

	/* choose compliment */
	char comp[SELECTION_SIZE];
	get_random_word(comps_file,comp);

	for (int i=0; i<amount_of_subs;i++) {
		/* init vars*/
		char subject[SUBJECT_SIZE];
		char body[BODY_SIZE];
		char* attachment_name = NULL;
		char* attachment_path = NULL;
		struct Email_Sub email_sub = email_sub_array[i];
		struct Email email_template;
		printf("\n%s %s %s %s %s\n",email_sub.FirstName,email_sub.LastName,email_sub.Email,email_sub.AdditionalText,email_sub.Temp);



		/* Changed based on template */
		if (strncmp("fact_app",email_sub.Temp,9) == 0) {
			/* fact Template */

			sprintf(subject,"Sherlly's Fact app for %s\n",email_sub.FirstName);
			sprintf(body,"Hey! hope you enjoy todays fact:\n %s \n %s",fact,email_sub.AdditionalText);
		}
		else if (strncmp("comp_app",email_sub.Temp,9) == 0) {

			/* compliment Template */
			sprintf(subject,"Sherlly's Compliment app for %s\n",email_sub.FirstName);
			sprintf(body,"\nHey! hope you have a good day queen:\n %s \n %s",comp,email_sub.AdditionalText);
		}
		else if (strncmp("mem_app",email_sub.Temp,9) == 0) {
			/* Memory Template */
			attachment_path = "attachments/memory.csv";
			attachment_name = "memory_system.csv";
			sprintf(subject,"Sherlly's Memory app for %s\n",email_sub.FirstName);
			int rand_int = rand() % 10;

			sprintf(body,"\nHey! Today you have to study:\n %d00s \n %s",rand_int,email_sub.AdditionalText);


		}
		else {
		/* Default */
			sprintf(subject,"Error finding email sub type for %s",email_sub.FirstName);
			sprintf(body,"Please contact jacob@sherllymail for help");

		}

		char tmp_to_name[SIZE];
		sprintf(tmp_to_name,"%s %s", email_sub.FirstName,email_sub.LastName);
		strncpy(email_template.To_addr,email_sub.Email,SIZE);
		strncpy(email_template.To_name,tmp_to_name,SIZE);
		strncpy(email_template.Cc_addr,Cc_addr,SIZE);
		strncpy(email_template.Subject,subject,SUBJECT_SIZE);
		strncpy(email_template.Body,body,BODY_SIZE);
		if (attachment_path && attachment_name) {
			strncpy(email_template.Attachment_path,attachment_path,SIZE);
			strncpy(email_template.Attachment_name,attachment_name,SIZE);
		}

		send_email(email_template);

	}



	/* free the boi */
	if (email_sub_array) {
		free(email_sub_array);
	}

}

