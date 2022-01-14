#include <cstdlib>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <string>
#include <termios.h>
#include <stdarg.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

class RS232 {
public:
	RS232(bool debug = true);
	RS232(string path, bool debug = true);
	~RS232();
	int open(string path);
	void init(const int baud = 19200, const string par = "even", const short bits = 8, const short stopb = 1);
	void close();
	void flush();
	int setRTS(bool on_off);
	int setDCD(bool on_off);
	int setDTR(bool on_off);
	int getBTW();
	int getBTR();

	int getDTR();
	int getRTS();
	int getCTS();
	int getDSR();
	int getRI();
	int getDCD();

	void getLineStatus();

	int write(u_char *data, ssize_t size);
	int write_using_RTS(u_char *data, ssize_t size);
	int read(u_char *data, size_t size = 113, size_t timeout = 2);
private:
	int handle;
	bool isOpen; //port opening status
	string port_path;
	string lineStatus;
	bool DEBUG; //write debug message flag
	void shFmtSystem(const char *fmt, ...); //format system command call for initialization port
};

#endif /* RS232_H_ */
