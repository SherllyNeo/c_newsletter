CC=gcc
cflags=-L/usr/lib/ -lmariadb -I/usr/include/mysql -I/usr/include/mysql/mysql -lcurl

cmail: cmail.c emailer.h sql_connector.c mail.c config.h shared.h
	$(CC) -o mailer cmail.c emailer.h config.h shared.h $(cflags)
