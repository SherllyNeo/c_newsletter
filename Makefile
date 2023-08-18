CC=gcc
cflags=-L/usr/lib/ -lmariadb -I/usr/include/mysql -I/usr/include/mysql/mysql -lcurl -Wall -Wextra -pedantic

emailing_app: cmail.c emailer.h sql_connector.c mail.c config.h shared.h
	$(CC) -o emailing_app cmail.c emailer.h config.h shared.h $(cflags)
install:
	$(CC) -o emailing_app cmail.c emailer.h config.h shared.h $(cflags) && cp ./emailing_app /usr/local/bin
