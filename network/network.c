#include "network.h"
#include <stdio.h>
#include <ifaddrs.h>

//#include <sys/socket.h>
//#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


int get_local_ips()
{
  struct ifaddrs *addrs = NULL;
  if (0 != getifaddrs(&addrs)) {
      printf("Failed to get local ips\n");
      return -1;
  }

  for (; addrs != NULL; addrs = addrs->ifa_next)
  {
    //check it is IP4
    //is a valid IP4 Address
    //if (ifAddrStruct->ifa_addr->sa_family==AF_INET6)
    if (addrs->ifa_addr == NULL || addrs->ifa_addr->sa_family != AF_INET)
      continue;

    if ((addrs->ifa_flags & IFF_LOOPBACK) || !(addrs->ifa_flags & IFF_UP))
      continue;

    void *sin_addr = &((struct sockaddr_in*)addrs->ifa_addr)->sin_addr;
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(addrs->ifa_addr->sa_family, sin_addr, buffer, INET_ADDRSTRLEN);
    //inet_ntop(addrs->ifa_addr->sa_family, sin_addr, addressBuffer, INET6_ADDRSTRLEN);
    printf("%s IPV4 Address %s\n", addrs->ifa_name, buffer);
  }

  freeifaddrs(addrs);
  return 0;
}
