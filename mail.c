#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "shared.h"

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
	printf("\nsending email for --> Email: %s ,First Name: %s, Last Name: %s, Additonal Text: %s, Template: %s\n",email_sub.Email,email_sub.FirstName,email_sub.LastName,email_sub.AdditionalText,email_sub.Temp);
	/* constant values */
	char* from_email = "<app@sherllymail.com>";
	char* from = "Sherlly's App <App@sherllymail.com>";
	char* cc_addr = "Jacob <jacob@sherllymail.com>";

	char to_email[30];
	char to[30];
	char subject[30];
	char body[50];
	sprintf(to_email,"<%s>",email_sub.Email);
	sprintf(to,"%s %s <%s>",email_sub.FirstName,email_sub.LastName,email_sub.Email);
	if (strncmp("fact_app",email_sub.Temp,9) == 0) {
		sprintf(subject,"Sherlly's Fact app for %s",email_sub.FirstName);
		sprintf(body,"Test fact app message: \n %s",email_sub.AdditionalText);
	}
	else {
	/* Default */
		sprintf(subject,"Error finding email sub type for %s",email_sub.FirstName);
		sprintf(body,"Please contact jacob@sherllymail for help");

	}

	sprintf(payload_text,"To: %s \nFrom: %s \nCc: %s \nSubject: %s \n\n %s \n\n \n ",to,from,cc_addr,subject,body);



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
	  }
}

