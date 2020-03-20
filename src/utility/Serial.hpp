#include "base/base.hpp"

namespace my {

    class Serial : public std::iostream {
    private:
        struct element {
            int rec_num;
#ifdef USE_LINUX
            int serial_fd;
#endif
            serialbuff serbuf;

            element() : rec_num(1) {}

            ~element() {
                serbuf.sync();
                close(serial_fd);
            }
        };

        element *serial;

    public:
        enum BOUND : unsigned int {
            BOUND9600 = B9600,
            BOUND19200 = B19200,
            BOUND115200 = B115200,
            BOUND460800 = B460800,
            BOUND921600 = B921600,
        };

        Serial(std::string com, BOUND bound) {
            serial = new element();

            serial->serial_fd = ::open(com.data(), O_RDWR | O_NOCTTY);
            if (serial->serial_fd == -1) {
                perror("open serial failed");
                exit(0);
            }
            termios option;
            if (tcgetattr(serial->serial_fd, &option) != 0) {
                perror("SetupSerial 1");
                exit(0);
            }
            cfsetispeed(&option, bound);

            option.c_cflag &= ~CSIZE;
            option.c_cflag &= ~CSTOPB;
            option.c_cflag &= ~PARODD;
            option.c_cflag &= ~PARENB;
            option.c_cflag |= CREAD | CLOCAL;
            option.c_cflag |= CS8;

            option.c_oflag &= ~OPOST;
            option.c_oflag &= ~(ONLCR | OCRNL | ONOCR);


            option.c_iflag &= ~INPCK;
            option.c_iflag &= ~ISTRIP;
            option.c_iflag &= ~(ICRNL | INLCR);
            option.c_iflag &= ~(IXON | IXANY | IXOFF);

            option.c_lflag &= ~(ISIG | ECHO | ECHOE);
            option.c_lflag &= ~ICANON;

            option.c_cc[VTIME] = 0;
            option.c_cc[VMIN] = 1;

            tcflush(serial->serial_fd, TCIOFLUSH);

            if (tcsetattr(serial->serial_fd, TCSANOW, &option) != 0) {
                perror("com ser error");
                exit(0);
            }

            serial->serbuf.init(serial->serial_fd, serial->serial_fd);
            this->init(&serial->serbuf);
        }

        Serial(const Serial &s) = delete;

        Serial(Serial &&s) : std::iostream(std::move(s)) {
            this->serial = s.serial;
            this->serial->rec_num += 1;
        };

        Serial &operator=(const Serial &) = delete;

        Serial &operator=(Serial &&) = delete;

        ~Serial() {
            serial->rec_num -= 1;
            if (serial->rec_num <= 0) {
                delete serial;
            }
        }
    };
}