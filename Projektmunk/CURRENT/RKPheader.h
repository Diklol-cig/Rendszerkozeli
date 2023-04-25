#ifndef MYHEADER_H // if not defined
#define MYHEADER_H // define it

int namecheck(int argc, char* argv[]);

void write_help();

int FindPID();

char* concat(const char *s1,const char *s2);

void write_int(char *p, int value);

int int_pow(int a, int b);

int calculate_size(int min, int sec);

int Measurement(int **p_values);

void SendViaFile(int *Values, int NumValues);

int checkdup(int argc, char *argv[]);

void BMPcreator(int *Values, int NumValues);

void execute_commands(int modes[]);

void signal_handeler(int sig);

void modecheck(int * modes, int argc, char* argv[]);

void ReceiveViaFile(int sig);

void SendViaFile(int *Values, int NumValues);

void SendViaSocket(int *Values, int NumValues);

void ReceiveViaSocket();

#endif
