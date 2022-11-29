#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define ADDRESS "tcp://192.168.137.1:1883"
#define CLIENTID "ExampleClientPub"
#define TOPIC "MQTT Examples"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L
void main(int argc, char *argv[])
{
    while (1)
    {

        int file_i2c;
        int length;
        unsigned char buffer[60] = {0};
        char temperature[10];

        char *filename = (char *)"/dev/i2c-1";
        if ((file_i2c = open(filename, O_RDWR)) < 0)
        {
            printf("Failed to open the i2c bus");
            return;
        }

        int addr = 0x48;
        if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
        {
            printf("Failed to acquire bus access and/or talk to slave.\n");
            return;
        }

        buffer[0] = 0x00;
        length = 2;
        if (write(file_i2c, buffer, length) != length)
        {
            printf("Failed to write to the i2c bus.\n");
        }

        length = 1;
        if (read(file_i2c, buffer, length) != length)
        {
            printf("Failed to read from the i2c bus.\n");
        }
        else
        {
            printf("Data read: %d\n", buffer[0]);
            sprintf(temperature, "%d", buffer[0]);
        }

        MQTTClient client;
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        MQTTClient_deliveryToken token;

        int rc;

        MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;

        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
        {
            printf("Failed to connect, return code %d\n", rc);
            exit(-1);
        }

        pubmsg.payload = temperature;
        pubmsg.payloadlen = strlen(temperature);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

        printf("Waiting for up to %d seconds for publication of %s\n"
               "on topic %s for client with ClientID: %s\n",
               (int)(TIMEOUT / 1000), PAYLOAD, TOPIC, CLIENTID);

        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

        printf("Message with delivery token %d delivered\n", token);

        MQTTClient_disconnect(client, 10000);

        MQTTClient_destroy(&client);

        sleep(5);
    }

    return;
}
