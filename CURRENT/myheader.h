#ifndef MYHEADER_H // if not defined
#define MYHEADER_H // define it

int namecheck(int argc, char* argv[]);

int FindPID();

void write_int(int *p, int value);

int int_pow(int a, int b);

int calculate_size(int min, int sec);

int Measurement(int **p_values);

void SendViaFile(int *Values, int NumValues);

int checkdup(int argc, char *argv[]);

void BMPcreator(int *Values, int NumValues);

void execute_commands(int modes[]);

void signal_handeler(int sig);

#endif
