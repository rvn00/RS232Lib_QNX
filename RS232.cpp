#include "RS232.h"

void RS232::shFmtSystem(const char *fmt, ...) {
	char text[256];
	va_list ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	if (DEBUG)
		cout << "Executable command: " << text << endl;
	system(text);
}
/*
 * Create new class object
 * debug - write debug message flag
 * 	true - on
 *  false -off
 */
RS232::RS232(bool debug):DEBUG(debug) {
	handle = 0;
	WSYSLOG::open(0, 0, WSYSLOG::APP_KPOVS_DRV_BURAN);
}

/*
 * Create new class object, open port by port path,
 * initialization port by default parameters:
 * - baud rate = 19200
 * - parity = "even"
 * - data bits = 8
 * - stop bits = 1
 * + string path - port path name
 * 		example: "/dev/ser1"
 * + bool debug - write debug message flag
 * 		true - on
 *		false -off
 */
RS232::RS232(string path, bool debug):port_path(path), DEBUG(debug) {
	handle = 0;
	if (open(path)) init();
}

int RS232::open(string path) {
	port_path = path;
	handle = ::open(port_path.c_str(), O_RDWR);
	if (handle == -1) {
		if (DEBUG)
			cerr << "Cannot open serial port!" << endl;
		return 0;
	}
	tcflush(handle, TCIOFLUSH);
	isOpen = true;
	return 1;
}

void RS232::close() {
	::close(handle);
}
/*
 * Setting port parameters
 * - baud rate
 * - parity
 * - data bits
 * - stop bits
 */
void RS232::init(const int baud, const string par, const short bits, const short stopb) {
	shFmtSystem("stty baud=%u par=%s bits=%u stopb=%u < %s", baud, par.c_str(), bits, stopb, port_path.c_str());
}

/*
 * Control RTS
 * true = on
 * false = off
 */
int RS232::setRTS(bool on_off) {
	int data = 0, error = 0;
	if (on_off)
		data = _CTL_RTS_CHG | _CTL_RTS;
	else
		data = _CTL_RTS_CHG | 0;
	if ((error = devctl(handle, DCMD_CHR_SERCTL, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error setting RTS: " << strerror(error) << endl;
		return 1;
	}
	return 0;
}
/*
 * Control DCD
 * true = on
 * false = off
 */
int RS232::setDCD(bool on_off) {
	int data = 0, error = 0;
	if (on_off)
		data = _CTL_DCD_CHG | _CTL_DCD;
	else
		data = _CTL_DCD_CHG | 0;
	if ((error = devctl(handle, DCMD_CHR_SERCTL, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error setting DCD: " << strerror(error) << endl;
		return 1;
	}
	return 0;
}
/*
 * Control DTR
 * true = on
 * false = off
 */
int RS232::setDTR(bool on_off) {
	int data = 0, error = 0;
	if (on_off)
		data = _CTL_DTR_CHG | _CTL_DTR;
	else
		data = _CTL_DTR_CHG | 0;
	if ((error = devctl(handle, DCMD_CHR_SERCTL, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error setting DTR: " << strerror(error) << endl;
		return 1;
	}
	return 0;
}
/*
 * Get DTR status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getDTR(){
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_DTR) {
		if (DEBUG)
			//cout << " +DTR ";
			lineStatus += " +DTR ";
			//cout << "DTR is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -DTR ";
			//cout << " -DTR ";
			//cout << "DTR is NOT set!" << endl;
		return 0;
	}
}
/*
 * Get CTS status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getCTS(){
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_CTS) {
		if (DEBUG)
			lineStatus += " +CTS ";
			//cout << " +CTS ";
			//cout << "CTS is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -CTS ";
			//cout << " -CTS ";
			//cout << "CTS is NOT set!" << endl;
		return 0;
	}
}
/*
 * Get DSR status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getDSR(){
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_DSR) {
		if (DEBUG)
			lineStatus += " +DSR ";
			//cout << " +DSR ";
			//cout << "DSR is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -DSR ";
			//cout << "DSR is NOT set!" << endl;
		return 0;
	}
}
/*
 * Get RI status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getRI(){
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_RI) {
		if (DEBUG)
			lineStatus += " +RI ";
			//cout << " +RI ";
			//cout << "RI is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -RI ";
			//cout << " -RI ";
			//cout << "RI is NOT set!" << endl;
		return 0;
	}
}

/*
 * Get RTS status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getRTS() {
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_RTS) {
		if (DEBUG)
			lineStatus += " +RTS ";
			//cout << " +RTS ";
			//cout << "RTS is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -RTS ";
			//cout << " -RTS ";
			//cout << "RTS is NOT set!" << endl;
		return 0;
	}
}
/*
 * Get DCD status
 * 1 = on
 * 0 = off
 * -1 = get status error
 */
int RS232::getDCD() {
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_LINESTATUS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (data & _LINESTATUS_SER_CD) {
		if (DEBUG)
			lineStatus += " +DCD ";
			//cout << " +DCD ";
			//cout << "DCD is SET!" << endl;
		return 1;
	} else {
		if (DEBUG)
			lineStatus += " -DCD ";
			//cout << " -DCD ";
			//cout << "DCD is NOT set!" << endl;
		return 0;
	}
}

/*
 * Print all line status signals
 *
 */
void RS232::getLineStatus(){
	getDTR();
	getRTS();
	getCTS();
	getDSR();
	getRI();
	getDCD();
	if (DEBUG){
		cout << "LINESTATUS: " << lineStatus << endl;
		lineStatus = "";
	}
}

/*
 * This call is made to determine the number of characters waiting to be sent.
 *
 */
int RS232::getBTW() {
	int data = 0, error = 0;
	if ((error = devctl(handle, DCMD_CHR_OSCHARS, &data, sizeof(data), NULL))!= 0) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(error) << endl;
		return -1;
	}
	if (DEBUG)
		cout << "The number of characters waiting to be sent: " << data << endl;
	return data;
}

/*
 * Determine the number of characters waiting to be read
 *
 */
int RS232::getBTR() {
	int data = 0;
	if ((data = tcischars(handle))== -1) {
		if (DEBUG)
			cerr << "Error requested line status information: " << strerror(data) << endl;
		return -1;
	}
	if (DEBUG && data > 0)
		cout << dec <<"The number of characters waiting to be read: " << data << endl;
	return data;
}


int RS232::write(u_char *data, ssize_t size) {
	ssize_t size_written = ::write(handle, data, size);
	if (size_written == -1) {
		if (DEBUG)
			cerr << "Error write data!" << endl;
		tcflush(handle, TCOFLUSH);
		/*close();
		open(port_path);
		init();*/
		return -1;
	}
	if (size_written != size) {
		if (DEBUG)
			cerr << "Error write data! Should write " << size << " bytes, but write " << size_written << "bytes" << endl;
		return -1;
	}
	if (DEBUG)
		WSYSLOG::OUT_DEBUG("%s(%d) %s: Write %d bytes!", __FILE__,	__LINE__, __func__, size_written);
		//cout << "Write " << size_written << " bytes!" << endl;
	return size_written;
}

/*
 * Write data to a port with enabled RTS line.
 * After writing data, RTS line turns off.
 */
int RS232::write_using_RTS(u_char *data, ssize_t size) {
	setRTS(true);
	delay(5);
	//while(!getRTS()){} // Waiting for the RTS to be set
	ssize_t size_written = ::write(handle, data, size);
	int a = 0;
	while(getBTW()){
		if (++a == 100){
			//flush();
			break;
		}
		delay(1);
	} // Waiting for the data to be sent
	setRTS(false);
	//while(getRTS()){} // Waiting for the RTS to be switched off
	if (size_written == -1) {
		if (DEBUG)
			cerr << "Error write data!" << endl;
		tcflush(handle, TCOFLUSH);
		close();
		open(port_path);
		init();
		return -1;
	}
	if (size_written != size) {
		if (DEBUG)
			cerr << "Error write data! Should write " << size << " bytes, but write " << size_written << "bytes" << endl;
		return -1;
	}
	if (DEBUG)
		WSYSLOG::OUT_DEBUG("%s(%d) %s: Write %d bytes!", __FILE__,	__LINE__, __func__, size_written);
		//cout << "Write " << size_written << " bytes!" << endl;
	return size_written;
}

int RS232::read(u_char *data, size_t size, size_t timeout) {

	timeval tv;

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	//while(getBTR() <= 0) {}//delay(1);
	int count_rcv_bytes = readcond(handle, data, size, size, 0, timeout);
	if (count_rcv_bytes < 0) {
		if (DEBUG)
			cerr << "Error read data!";
		close();
		open(port_path);
		init();
		//tcflush(handle, TCIFLUSH);
		return -1;
	}
	//tcflush(handle, TCIFLUSH);
	return count_rcv_bytes;
}

void RS232::flush(){
	tcflush(handle, TCIOFLUSH);
}

RS232::~RS232() {
	tcflush(handle, TCIOFLUSH);
	close();
	handle = 0;
}
