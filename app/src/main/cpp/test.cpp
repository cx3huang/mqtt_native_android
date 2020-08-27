#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <memory.h>
#include <iostream>
#include <android/log.h>
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"

#define ADDRESS "tcp://192.168.1.74:1883"
#define CLIENTID "TestClient1"
#define TOPIC "hello"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L
#define KEEPALIVE 10
#define MAXDATALEN 100

volatile int toStop = 0;
MQTTClient client;
int rc = 0;

extern "C" JNIEXPORT jint JNICALL Java_com_example_a1_MainActivity_connect(
        JNIEnv* env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_INFO, "Connect", "Entered connect()");

    //Initializes connection options for MQTT Client.
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    // char url[100] = ADDRESS;
    // Creates the MQTT Client.
    rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE,
            NULL);
    __android_log_print(ANDROID_LOG_INFO, "Connect", "MQTT client created");

    // Configures the options for connection.
    conn_opts.keepAliveInterval = KEEPALIVE;
    conn_opts.reliable = 0;
    conn_opts.cleansession = 1;
    conn_opts.username = NULL;
    conn_opts.password = NULL;
    // Connects the MQTT Client
    rc = 0;
    if((rc = MQTTClient_connect(client, &conn_opts)) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Connect", "Connect failed");
        return rc;
    }
    __android_log_print(ANDROID_LOG_INFO, "Connect", "Exiting connect()");
    return rc;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_example_a1_MainActivity_subscribe(
        JNIEnv* env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_INFO, "Subscribe", "Entered subscribe()");
    char toReturn[100];
    // Subscribes to the topic provided.
    if((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Subscribe", "Subscribe failed");
        exit(EXIT_FAILURE);
    }
    int cont = 0;
    while(!toStop && cont < 10) {
        char* topicName = NULL;
        int topicLength;
        MQTTClient_message* message = NULL;

        if((rc = MQTTClient_receive(client, &topicName, &topicLength, &message,
                1000)) != 0) {
            __android_log_print(ANDROID_LOG_ERROR, "Subscribe",
                    "Receive message failed");
            exit(EXIT_FAILURE);
        }

        if(message) {
            fflush(stdout);
            strcpy(toReturn, reinterpret_cast<const char *>(message));
            MQTTClient_freeMessage(&message);
            MQTTClient_free(topicName);
        }
        ++cont;
    }
    __android_log_print(ANDROID_LOG_INFO, "Subscribe", "Exiting subscribe()");
    return reinterpret_cast<jstring>(toReturn);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_example_a1_MainActivity_publish(
        JNIEnv* env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_INFO, "Publish", "Entered publish()");

    // Setting callbacks for MQTT Client
    char* buffer = NULL;
    if((rc = MQTTClient_setCallbacks(client, NULL, NULL,
                                     reinterpret_cast<int (*)(void *, char *, int,
                                             MQTTClient_message *)>(1), NULL)) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Publish", "Set callbacks failed");
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, "Hello from publish()");

    int cont = 0;
    while(!toStop && cont < 10) {
        int dataLen = 0;
        int delimLen = 0;

        delimLen = (int)strlen(0);
        do {
            dataLen++;
            if(dataLen > delimLen) {
                if(strncmp(0, &buffer[dataLen-0], 0) == 0) {
                    break;
                }
            }
        }
        while (dataLen < MAXDATALEN);

        // Publish using MQTT Client
        if((rc = MQTTClient_publish(client, TOPIC, dataLen, buffer, QOS, 0,
                NULL) != 0)) {
            __android_log_print(ANDROID_LOG_ERROR, "Publish", "Publish failed");
            exit(EXIT_FAILURE);
        }
        ++cont;
    }
    __android_log_print(ANDROID_LOG_INFO, "Publish", "Exiting publish()");

    // Free the message buffer
    free(buffer);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_example_a1_MainActivity_disconnect(
        JNIEnv* env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_INFO, "Disconnect", "Entered disconnect()");
    // Disconnect and destroy the created client
    MQTTClient_disconnect(client, 0);
    MQTTClient_destroy(&client);
    __android_log_print(ANDROID_LOG_INFO, "Disconnect", "Exiting disconnect()");
}

