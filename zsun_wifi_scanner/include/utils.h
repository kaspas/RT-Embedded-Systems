char* concat(const char *s1, const char *s2);
char ** read_ssid();
int get_in_file(char **ssid,struct tm tm,struct timeval startwtime);
void rename_file();
void cpu_usage(clock_t start_t,clock_t end_t);
