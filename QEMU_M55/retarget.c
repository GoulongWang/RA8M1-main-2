/*
 * main.c
 *
 *  Created on: Jul 23, 2024
 *      Author: User
 */


/*
 * main.c
 *
 *  Created on: Mar 18, 2024
 *      Author: User
 */

#include "Driver_USART.h"
#include <stdio.h>

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>STDOUT USART Interface

//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           0

//   <o>Baudrate
#define USART_BAUDRATE          115200

// </h>


#define _USART_Driver_(n)  Driver_USART##n
#define  USART_Driver_(n) _USART_Driver_(n)

extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);
#define ptrUSART       (&USART_Driver_(USART_DRV_NUM))


/**
  Initialize stdout

  \return          0 on success, or -1 on error.
*/
int stdout_init (void);
int stdout_init (void) {
  int32_t status;

  status = ptrUSART->Initialize(NULL);
  if (status != ARM_DRIVER_OK) return (-1);

  status = ptrUSART->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) return (-1);

  status = ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE,
                             USART_BAUDRATE);
  if (status != ARM_DRIVER_OK) return (-1);

  status = ptrUSART->Control(ARM_USART_CONTROL_TX, 1);
  status = ptrUSART->Control(ARM_USART_CONTROL_RX, 1);
  if (status != ARM_DRIVER_OK) return (-1);

  return (0);
}


// /**
//   Put a character to the stdout

//   \param[in]   ch  Character to output
//   \return          The character written, or -1 on write error.
// */
// int stdout_putchar (int ch);
// int stdout_putchar (int ch) {
//   uint8_t buf[1];

//   buf[0] = (uint8_t)ch;
//   if (ptrUSART->Send(buf, 1) != ARM_DRIVER_OK) {
//     return (-1);
//   }
//   // while (ptrUSART->GetTxCount() != 1);
//   return (ch);
// }

/* Implement some system calls to shut up the linker warnings */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>

#ifndef __clang__
#undef errno
extern int errno;
#endif

int _write(int file, char *ptr, int len) {

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    int i;
    for (i = 0; i < len; i++) {
      int status = ptrUSART->Send(&ptr[i], 1);
      if(status != ARM_DRIVER_OK) {
        return i;
      }
    }
    return i;
  }
  errno = EIO;
  return -1;
}

#if 1
int _open(char* file, int flags, int mode);
int _open(char* file, int flags, int mode) {
  (void)file;
  (void)flags;
  (void)mode;
  errno = ENOSYS;
  return -1;
}

int _close(int fd);
int _close(int fd) {
  errno = ENOSYS;
  (void)fd;
  return -1;
}

int _fstat(int fd, struct stat* buf);
int _fstat(int fd, struct stat* buf) {
  (void)fd;
  (void)buf;
  errno = ENOSYS;
  return -1;
}

int _getpid(void);
int _getpid(void) {
  errno = ENOSYS;
  return -1;
}

int _isatty(int file);
int _isatty(int file) {
  (void)file;
  errno = ENOSYS;
  return 0;
}

int _kill(int pid, int sig);
int _kill(int pid, int sig) {
  (void)pid;
  (void)sig;
  errno = ENOSYS;
  return -1;
}

int _lseek(int fd, int ptr, int dir);
int _lseek(int fd, int ptr, int dir) {
  (void)fd;
  (void)ptr;
  (void)dir;
  errno = ENOSYS;
  return -1;
}

int _read(int fd, char* ptr, int len);
int _read(int fd, char* ptr, int len) {
  (void)fd;
  (void)ptr;
  (void)len;
  errno = ENOSYS;
  return -1;
}

clock_t _times(struct tms* buf);
clock_t _times(struct tms* buf) {
  (void)buf;
  errno = ENOSYS;
  return (clock_t)-1;
}

void _exit(int error) {
  (void)error;
	while(1);
}

#endif

