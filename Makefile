CC      = gcc
CFLAGS  = -Wall -Wextra -g
SIG     = signals
PIP     = pipes

SIG_BIN = $(SIG)/father $(SIG)/sig_father $(SIG)/sig_son $(SIG)/sigact $(SIG)/sigact_int \
          $(SIG)/threads_sig1 $(SIG)/threads_sig2 $(SIG)/threads_sig3
PIP_BIN = $(PIP)/pipe_basic $(PIP)/pipe_clientserver $(PIP)/fifo_hello_server $(PIP)/fifo_hello_client \
          $(PIP)/fifo_server $(PIP)/fifo_client $(PIP)/fifomsg_server $(PIP)/fifomsg_client

all: $(SIG_BIN) $(PIP_BIN)

$(SIG)/sigact_int: $(SIG)/sigact.c
	$(CC) $(CFLAGS) -DSEND_SIGINT -o $@ $<

$(SIG)/threads_sig1: $(SIG)/threads_sig.c
	$(CC) $(CFLAGS) -pthread -DVARIANT=1 -o $@ $<
$(SIG)/threads_sig2: $(SIG)/threads_sig.c
	$(CC) $(CFLAGS) -pthread -DVARIANT=2 -o $@ $<
$(SIG)/threads_sig3: $(SIG)/threads_sig.c
	$(CC) $(CFLAGS) -pthread -DVARIANT=3 -o $@ $<

$(PIP)/fifomsg_server: $(PIP)/fifomsg_server.c $(PIP)/mesg.c $(PIP)/mesg.h
	$(CC) $(CFLAGS) -o $@ $(PIP)/fifomsg_server.c $(PIP)/mesg.c
$(PIP)/fifomsg_client: $(PIP)/fifomsg_client.c $(PIP)/mesg.c $(PIP)/mesg.h
	$(CC) $(CFLAGS) -o $@ $(PIP)/fifomsg_client.c $(PIP)/mesg.c

$(SIG)/%: $(SIG)/%.c
	$(CC) $(CFLAGS) -o $@ $<
$(PIP)/%: $(PIP)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SIG_BIN) $(PIP_BIN) $(SIG)/ps_log.txt $(PIP)/output.txt

.PHONY: all clean
