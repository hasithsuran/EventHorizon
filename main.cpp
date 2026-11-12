// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <net/if.h>
// #include <sys/socket.h>
// #include <sys/ioctl.h>
// #include <linux/can.h>
// #include <linux/can/raw.h>

#include <QApplication>
#include "MainWindow.h"
#include "LedIndicator.h"

int main(int argc, char *argv[]) {
    // int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    // if (sock < 0) { perror("socket"); return 1; }
    // printf("socket opened\n"); fflush(stdout);

    // // Use if_nametoindex instead of ioctl
    // unsigned int ifindex = if_nametoindex("vcan0");
    // if (ifindex == 0) {
    //     perror("if_nametoindex");
    //     close(sock);
    //     return 1;
    // }
    // printf("interface index: %u\n", ifindex); fflush(stdout);

    // struct sockaddr_can addr;
    // memset(&addr, 0, sizeof(addr));
    // addr.can_family  = AF_CAN;
    // addr.can_ifindex = ifindex;

    // if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    //     perror("bind");
    //     close(sock);
    //     return 1;
    // }
    // printf("bound, waiting for frames...\n"); fflush(stdout);

    // struct can_frame frame;
    // while (1) {
    //     int nbytes = read(sock, &frame, sizeof(frame));
    //     if (nbytes < 0) { perror("read"); break; }

    //     printf("ID: 0x%03X  DLC: %d  Data:", frame.can_id & CAN_EFF_MASK, frame.can_dlc);
    //     for (int i = 0; i < frame.can_dlc; i++)
    //         printf(" %02X", frame.data[i]);
    //     printf("\n"); fflush(stdout);
    // }

    // close(sock);
    // return 0;

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    window.led(0)->setColor(LedIndicator::Green);
    window.led(1)->setColor(LedIndicator::Amber);
    window.led(2)->setColor(LedIndicator::Red);
    window.led(3)->setColor(LedIndicator::Off);

    return app.exec();
}
