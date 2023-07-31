#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include "shared.h"


#define fact_file "facts_store.txt"
#define comps_file "compliments_store.txt"


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
    char selected[800];
    char current[800];
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
    strncpy(buffer,selected,800);

}

struct upload_status {
  size_t bytes_read;
};

char payload_text[1000];


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

void send_email(struct Email_Sub email_sub) {
	/* constant values */
	char* from_email = "<app@sherllymail.com>";
	char* from = "Sherlly's App <App@sherllymail.com>";
	char* cc_addr = "Jacob <jacob@sherllymail.com>";

	char to_email[100];
	char to[100];
	char subject[200];
	char body[1000];
	sprintf(to_email,"<%s>",email_sub.Email);
	sprintf(to,"%s %s <%s>",email_sub.FirstName,email_sub.LastName,email_sub.Email);
	/*A lot of email templates require random selections*/
	srand(time(NULL));
	if (strncmp("fact_app",email_sub.Temp,9) == 0) {

		sprintf(subject,"Sherlly's Fact app for %s\n",email_sub.FirstName);
		char fact[800];
		get_random_word(fact_file,fact);
		sprintf(body,"Hey! hope you enjoy todays fact:\n %s \n %s",fact,email_sub.AdditionalText);
	}
	else if (strncmp("comp_app",email_sub.Temp,9) == 0) {

		sprintf(subject,"Sherlly's Compliment app for %s\n",email_sub.FirstName);
		char comp[800];
		get_random_word(comps_file,comp);
		sprintf(body,"\nHey! hope you have a good day queen:\n %s \n %s",comp,email_sub.AdditionalText);
	}
	else if (strncmp("mem_app",email_sub.Temp,9) == 0) {

		sprintf(subject,"Sherlly's Memory app for %s\n",email_sub.FirstName);
		int rand_int = rand() % 10;
		sprintf(body,"\nHey! Today you have to study:\n %d00s \n %s",rand_int,email_sub.AdditionalText);
	}
	else {
	/* Default */
		sprintf(subject,"Error finding email sub type for %s",email_sub.FirstName);
		sprintf(body,"Please contact jacob@sherllymail for help");

	}

	sprintf(payload_text,"To: %s \nFrom: %s \nCc: %s \nSubject: %s \n\n This is a beta version of my new app. Please tell me if there are any errors\n %s \n\n \n ",to,from,cc_addr,subject,body);



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

	    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_email);

	    recipients = curl_slist_append(recipients, to_email);
	    recipients = curl_slist_append(recipients, to_email);
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

	    /* Free the list of recipients */
	    curl_slist_free_all(recipients);

	    curl_easy_cleanup(curl);
	    printf("\nsent email - email to -->  %s %s with email: %s and additonal text:  %s using template: %s\n",
			email_sub.FirstName,
			email_sub.LastName,
			email_sub.Email,
			email_sub.AdditionalText,
			email_sub.Temp);
	  }
}

