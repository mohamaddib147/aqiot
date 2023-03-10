/*
 * Copyright (c) 2017, Peter Sjodin, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 *
 * Author  : Peter Sjodin, KTH Royal Institute of Technology
 * Created : 2017-04-21
 *
 * Hacked by Robert Olsson 2021-01-19
 */

#ifndef PMS5003_H
#define PMS5003_H

/* Default sample period - how often sensor data is collected (sec) */
#ifdef PMS5003_CONF_SAMPLE_PERIOD
#define PMS_SAMPLE_PERIOD       PMS5003_CONF_SAMPLE_PERIOD
#else
#define PMS_SAMPLE_PERIOD       60
#endif /* PMS5003_CONF_SAMPLE_PERIOD */

/* Default warmup time before sensor data can be read (sec) */
#ifdef PMS5003_CONF_WARMUP_INTERVAL
#define PMS_WARMUP_INTERVAL    PMS5003_CONF_WARMUP_INTERVAL
#else
#define PMS_WARMUP_INTERVAL    30
#endif /* PMS5003_WARMUP_INTERVAL */

#ifdef PMS5003_CONF_I2C_ADDR
#define PMS5003_I2C_ADDR       PMS5003_CONF_I2C_ADDR
#else
#define PMS5003_I2C_ADDR 0x12
#endif /* PMS5003_CONF_I2C_ADDR */

void pms5003_init(void);
void pms5003_off(void);

uint16_t pms5003_pm1(void);
uint16_t pms5003_pm2_5(void);
uint16_t pms5003_pm10(void);
uint16_t pms5003_pm1_atm(void);
uint16_t pms5003_pm2_5_atm(void);
uint16_t pms5003_pm10_atm(void);
uint16_t pms5003_db0_3(void);
uint16_t pms5003_db0_5(void);
uint16_t pms5003_db1(void);
uint16_t pms5003_db2_5(void);
uint16_t pms5003_db5(void);
uint16_t pms5003_db10(void);
uint32_t pms5003_timestamp(void);
void pms5003_set_sample_period(unsigned int);
void pms5003_set_warmup_interval(unsigned int);
unsigned pms5003_get_sample_period(void);
unsigned pms5003_get_warmup_interval(void);

void pms5003_init(void);

#endif /* PMS5003_H */
