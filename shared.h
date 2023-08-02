#ifndef SHARED
#define SHARED

#define SIZE 100
#define SELECTION_SIZE 100
#define BODY_SIZE 20000
#define SUBJECT_SIZE 200
#define FULL_PAYLOAD_SIZE 900000
#define ATTACHMENT_SIZE 500000

struct Email_Sub {
	char Temp[SIZE];
	char FirstName[SIZE];
	char LastName[SIZE];
	char AdditionalText[SIZE];
	char Email[SIZE];
};



struct Email {
	char To_addr[SIZE];
	char To_name[SIZE];
	char Cc_addr[SIZE];
	char Subject[SUBJECT_SIZE];
	char Body[BODY_SIZE];
	char Attachment_path[SIZE];
	char Attachment_name[SIZE];
};

#endif

