#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SUCCESS 0
#define FAILURE -1
#define BUFFER_MAX 5
#define DIRECTION_MAX 35
#define VALUE_MAX 35
#define HIGH 1
#define LOW 0
#define IN 0
#define OUT 1
#define LEN 150

static int GPIOExport(int pin)
{
	int Status = SUCCESS;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd == FAILURE ) {
		Status = FAILURE;
		printf("Failed to open export for writing!\n");
		goto END;
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	
END:
	close(fd);
	return Status;
}

static int GPIOUnexport(int pin)
{
	int Status = SUCCESS;
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd == FAILURE) {
		Status = FAILURE;
		printf("Failed to open unexport for writing!\n");
		goto END;
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	
END:
	close(fd);
	return Status;
}

static int GPIODirection(int pin, int dir)
{
	int Status = SUCCESS;
	static const char s_directions_str[]  = "in\0out";
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (fd == FAILURE) {
		Status = FAILURE;
		printf("Failed to open gpio direction for writing!\n");
		goto END;
	}

	if (FAILURE == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		Status = FAILURE;
		printf("Failed to set direction!\n");
		goto END;
	}
	
END:
	close(fd);
	return Status;
}

static int GPIORead(int pin)
{
	int Status = SUCCESS;
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (fd == FAILURE) {
		Status = FAILURE;
		printf("Failed to open gpio value for reading!\n");
		goto END;
	}

	if (read(fd, value_str, 3) == FAILURE) {
		Status = FAILURE;
		printf("Failed to read value!\n");
		goto END;
	}
	Status = atoi(value_str);
	
END:
	close(fd);
	return Status;
}

static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";
	int Status = SUCCESS;
	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (fd == FAILURE) {
		Status = FAILURE;
		printf("Failed to open gpio value for writing!\n");
		goto END;
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		Status = FAILURE;
		printf("Failed to write value!\n");
		goto END;
	}

END:
	close(fd);
	return Status;
}


int main(int argc, char** argv)
{
	int Status = FAILURE;
	int GpioVal = 0;
	int fd;
	char buf[LEN];
    	time_t curtime;
    	struct tm *loc_time;
    	int size = 0;
	int counter = 1;
	
	if(argc < 5) {
		printf("Please enter the GPIO Number as below..\n");
		printf("Usage: ./gpio -i <input gpio number> -o <output gpio number> --log\n"); 
		goto END;
	}
	
	if (strcmp(argv[1],"-i") == SUCCESS) {
		Status = GPIOExport(atoi(argv[2]));
		if(Status != SUCCESS) {
			Status = FAILURE;
			printf("Input GPIO Export Failed..\n");
			goto END;
		}
		Status = GPIODirection(atoi(argv[2]), IN);
		if(Status != SUCCESS) {
			Status = FAILURE;
			printf("Input GPIO Set Direction Failed..\n");
			goto END;
		}
	}
	
	if(strcmp(argv[3],"-o") == SUCCESS) {
		Status = GPIOExport(atoi(argv[4]));
		if(Status != SUCCESS) {
			Status = FAILURE;
			printf("Output GPIO Export Failed..\n");
			goto END;
		}
		Status = GPIODirection(atoi(argv[4]), OUT);
		if(Status != SUCCESS) {
			Status = FAILURE;
			printf("Output GPIO Set Direction Failed..\n");
			goto END;
		}
	}
	while(1) {
		GpioVal = GPIORead(atoi(argv[2]));
		if (strcmp(argv[5],"--log") == SUCCESS) {
			fd = open("gpiostatus.txt",O_RDWR | O_CREAT | O_APPEND );
			if(fd == FAILURE) {
				Status = FAILURE;
				printf("Status Log file creation Failed..\n");
				goto END;
			}
			curtime = time (NULL);
			loc_time = localtime (&curtime);
			sprintf(buf,"%d",counter++);
			size = strlen(buf);
			sprintf(&buf[size],"%s", asctime (loc_time));
			size = strlen(buf);
			sprintf(&buf[size],"%s", "gpio <Y> ");
			size = strlen(buf);
			if(GpioVal == HIGH) {
				sprintf(&buf[size],"%s","HIGH");
			}
			else {
				sprintf(&buf[size],"%s","LOW");
			}
			write(fd,buf,strlen(buf)+1);
		}
		if(GpioVal == HIGH) {
			GPIOWrite(atoi(argv[4]), LOW);
			printf("\n NS:APP: READ1=%d",GPIORead(atoi(argv[4])));
			sleep(1);
			GPIOWrite(atoi(argv[4]), HIGH);
			printf("\n NS:APP: READ2=%d",GPIORead(atoi(argv[4])));
			sleep(1);
		}
		else {
			GPIOWrite(atoi(argv[4]), LOW);
		}
	}
	
END:
    return Status;
}
