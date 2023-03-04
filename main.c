/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating RIOT's MQTT-SN library
 *              emCute
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "msg.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"
#include "pms5003.h"

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("gertrud")
#endif
#define EMCUTE_PORT         (1883U)
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#define PUB_PRIO            (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

#define INTERVAL (10U * US_PER_SEC)

static char stack[THREAD_STACKSIZE_DEFAULT];
static char pubstack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];

char msg[5] = "pm1:";

static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

// static void msgformat (void)
// {
//     char pm1str[1];
//     strcat(msg, itoa(pms5003_pm1(), pm1str, 10));
// }

static int pub (char *topic, char *msg)
{
    puts("Publishing a topic");
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    /* step 1: get topic id */
    t.name = topic;
    if (emcute_reg(&t) != EMCUTE_OK) {
        //puts("error: unable to obtain topic ID");
        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, msg, strlen(msg), flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }
    return 0;
}

static int con (void)
{
    sock_udp_ep_t gw = {
        .family = AF_INET6,
        .port = 10000
    };

    /* Parse address */
     if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, "2001:6b0:32:13::236") == NULL) {
        puts("error parsing IPv6 address");
        return 1;
    }

    /* Connect to broker */
   puts("Connecting");
    if (emcute_con(&gw, true, NULL, NULL, 0, 0) != EMCUTE_OK) {
        puts("error: unable to connect to broker");
        return 1;
    }

    puts("Successfully connected to broker");
  /* Connect to broker */
  /*  if (connect_to_broker() != 0) {
        return 1;
    }
    */
       
    /* should be never reached */
    return 0;
}


static int discon(void)
{
    puts("Attempting to disconnect");
    int res = emcute_discon();
    if (res == EMCUTE_NOGW) {
        puts("error: not connected to any broker");
        return 1;
    }
    else if (res != EMCUTE_OK) {
        puts("error: unable to disconnect");
        return 1;
    }
    puts("Disconnect successful");
    return 0;
}

static void *pub_thread(void *arg)
{
    (void)arg;
    int pubdelay = 0;
    xtimer_init();
    xtimer_ticks32_t last_wakeup = xtimer_now();
    puts("Pub thread activated");

    while(1)
    {
        con();
        // if(PM1alarm)
        // {
        //     // publish immediately
        // }

        // /*else*/if(pubdelay > 0) // if pubdelay+1 minutes has passed
        // {
        //     // publish an entry
        //     int i = 0;
        //     int error = 1;
        //     for(i=0; i<3; i++) // check if you are able to ping the gateway 3 times
        //     {
        //         if(con())
        //         {
        //             error = 0; // indicate no error
        //             break; // leave the loop
         uint32_t pm1 = pms5003_pm1();
        uint32_t pm2_5 = pms5003_pm2_5();
        uint32_t pm10 = pms5003_pm10();
        uint32_t db0_3 = pms5003_db0_3();
        uint32_t db0_5 = pms5003_db0_5();
        uint32_t db1 = pms5003_db1();
        uint32_t db2_5 = pms5003_db2_5();
        uint32_t db5 = pms5003_db5();
        uint32_t db10 = pms5003_db10();

        /* Format data as JSON */
        char json[256];
        snprintf(json, sizeof(json),
                 
                 "pm1:%lu,"
                 "pm2d5:%lu,"
                 "pm10:%lu,"
                 "db0d3:%lu,"
                 "db0d5:%lu,"
                 "db1:%lu,"
                 "db2d5:%lu,"
                 "db5:%lu,"
                 "db10:%lu"
                 ,
                 pm1, pm2_5, pm10, db0_3, db0_5, db1, db2_5, db5, db10);
        pub("group3", json);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        discon();
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);   
             
        puts("pub_thread loop");
        pubdelay += 1; // add 1 to the counter
    }
    return NULL;
}

static void on_pub(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");
}

static unsigned get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}

static int cmd_con(int argc, char **argv)
{
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 2) {
        printf("usage: %s <ipv6 addr> [port] [<will topic> <will message>]\n",
                argv[0]);
        return 1;
    }

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, argv[1]) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (argc >= 3) {
        gw.port = atoi(argv[2]);
    }
    if (argc >= 5) {
        topic = argv[3];
        message = argv[4];
        len = strlen(message);
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", argv[1], (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n",
           argv[1], (int)gw.port);

    return 0;
}

static int cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = emcute_discon();
    if (res == EMCUTE_NOGW) {
        puts("error: not connected to any broker");
        return 1;
    }
    else if (res != EMCUTE_OK) {
        puts("error: unable to disconnect");
        return 1;
    }
    puts("Disconnect successful");
    return 0;
}

static int cmd_pub(int argc, char **argv)
{
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    if (argc < 3) {
        printf("usage: %s <topic name> <data> [QoS level]\n", argv[0]);
        return 1;
    }

    /* parse QoS level */
    if (argc >= 4) {
        flags |= get_qos(argv[3]);
    }

    printf("pub with topic: %s and name %s and flags 0x%02x\n", argv[1], argv[2], (int)flags);

    /* step 1: get topic id */
    t.name = argv[1];
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, argv[2], strlen(argv[2]), flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%i]'\n",
                t.name, (int)t.id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [%i]'\n",
            (int)strlen(argv[2]), t.name, t.id);

    return 0;
}

static int cmd_sub(int argc, char **argv)
{
    unsigned flags = EMCUTE_QOS_0;

    if (argc < 2) {
        printf("usage: %s <topic name> [QoS level]\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > TOPIC_MAXLEN) {
        puts("error: topic name exceeds maximum possible size");
        return 1;
    }
    if (argc >= 3) {
        flags |= get_qos(argv[2]);
    }

    /* find empty subscription slot */
    unsigned i = 0;
    for (; (i < NUMOFSUBS) && (subscriptions[i].topic.id != 0); i++) {}
    if (i == NUMOFSUBS) {
        puts("error: no memory to store new subscriptions");
        return 1;
    }

    subscriptions[i].cb = on_pub;
    strcpy(topics[i], argv[1]);
    subscriptions[i].topic.name = topics[i];
    if (emcute_sub(&subscriptions[i], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", argv[1]);
        return 1;
    }

    printf("Now subscribed to %s\n", argv[1]);
    return 0;
}

static int cmd_unsub(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage %s <topic name>\n", argv[0]);
        return 1;
    }

    /* find subscriptions entry */
    for (unsigned i = 0; i < NUMOFSUBS; i++) {
        if (subscriptions[i].topic.name &&
            (strcmp(subscriptions[i].topic.name, argv[1]) == 0)) {
            if (emcute_unsub(&subscriptions[i]) == EMCUTE_OK) {
                memset(&subscriptions[i], 0, sizeof(emcute_sub_t));
                printf("Unsubscribed from '%s'\n", argv[1]);
            }
            else {
                printf("Unsubscription form '%s' failed\n", argv[1]);
            }
            return 0;
        }
    }

    printf("error: no subscription for topic '%s' found\n", argv[1]);
    return 1;
}

static int cmd_will(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage %s <will topic name> <will message content>\n", argv[0]);
        return 1;
    }

    if (emcute_willupd_topic(argv[1], 0) != EMCUTE_OK) {
        puts("error: unable to update the last will topic");
        return 1;
    }
    if (emcute_willupd_msg(argv[2], strlen(argv[2])) != EMCUTE_OK) {
        puts("error: unable to update the last will message");
        return 1;
    }

    puts("Successfully updated last will topic and message");
    return 0;
}

static int cmd_read(__attribute__((unused)) int ac, __attribute__((unused)) char **av)
  {
    printf("PM[1 2.5 10]: %-u %-u %-u\n", pms5003_pm1(), pms5003_pm2_5(), pms5003_pm10());
    printf("DB[0.3 0.5 1.0 2.5 5 10]: %-u %-u %-u %-u %-u %-u\n", pms5003_db0_3(), pms5003_db0_5(),
    pms5003_db1(),  pms5003_db2_5(), pms5003_db5(), pms5003_db10());
    return 0;
  }

static const shell_command_t shell_commands[] = {
    { "con", "connect to MQTT broker", cmd_con },
    { "discon", "disconnect from the current broker", cmd_discon },
    { "pub", "publish something", cmd_pub },
    { "sub", "subscribe topic", cmd_sub },
    { "unsub", "unsubscribe from topic", cmd_unsub },
    { "will", "register a last will", cmd_will },
    { "read", "read PM sensor data", cmd_read},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MQTT-SN example application\n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");

    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, ARRAY_SIZE(queue));

    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    /* start pms5003 thread */
    pms5003_init();

    thread_create(pubstack, sizeof(pubstack), PUB_PRIO, 0,
                  pub_thread, NULL, "pub");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
