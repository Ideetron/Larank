/*
 * Written by Ruud Vlaming, Beta Research BV
 * http://www.betaresearch.nl/
 *
 * This program pulses the resetline of the IMST iC880A Concentrator
 * as it is connected to GPIO on the Lorank 8 gateway. This may not
 * work, or even bring damage to other pieces of hardware. Use with 
 * care. Compile with:
 *
 * g++ -O2 -Wall ResetIC880A.cpp -o ResetIC880A
 * 
 * Based on:
 *
 * Work by Derek Molloy, School of Electronic Engineering, DCU
 * www.derekmolloy.ie
 *
 * Almost entirely based on Software by RidgeRun:
 *
 * Copyright (c) 2011, RidgeRun
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL I
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

using namespace std;

/* Definition of constants. */

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

enum PIN_DIRECTION
{	INPUT_PIN=0,
	OUTPUT_PIN=1 };

enum PIN_VALUE
{	LOW=0,
	HIGH=1 };

unsigned int RESET = 14;


/* Definitions of GPIO manipulation functions. */

int gpio_export(unsigned int gpio)
{	int fd, len;
	char buf[MAX_BUF];
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) 
	{	perror("gpio/export");
		return fd; }
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0; }

int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag)
{	int fd;
	char buf[MAX_BUF];
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) 
	{	perror("gpio/direction");
		return fd; }
	if (out_flag == OUTPUT_PIN) write(fd, "out", 4); else write(fd, "in", 3);
	close(fd);
	return 0; }

int gpio_set_value(unsigned int gpio, PIN_VALUE value)
{	int fd;
	char buf[MAX_BUF];
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) 
	{	perror("gpio/set-value");
		return fd; }
	if (value==LOW) write(fd, "0", 2); else write(fd, "1", 2);
	close(fd);
	return 0; }


/* Pulse the concentrator reset line once. */

int main(int argc, char *argv[])
{ gpio_export(RESET);
  gpio_set_dir(RESET, OUTPUT_PIN);
  gpio_set_value(RESET, LOW);
  usleep(10000);
  gpio_set_value(RESET, HIGH);
  usleep(10000);
  gpio_set_value(RESET, LOW);
  return 0; }
