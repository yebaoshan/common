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

ssize_t sread(int fd, void *buf, size_t count)
{
    char *ptr;
    size_t nleft;
    ssize_t status;

    ptr = (char *)buf;
    nleft = count;

    while (nleft > 0) {
        status = read(fd, (void *)ptr, nleft);

        if ((status < 0) && ((errno == EAGAIN) || (errno == EINTR)))
            continue;

        if (status < 0)
            return (status);

        if (status == 0) {
            DEBUG("Received EOF from fd %i. "
                        "Closing fd and returning error.",
                        fd);
            close(fd);
            return (-1);
        }

        assert((0 > status) || (nleft >= (size_t)status));

        nleft = nleft - ((size_t)status);
        ptr = ptr + ((size_t)status);
    }

    return (0);
}

ssize_t swrite(int fd, const void *buf, size_t count)
{
    const char *ptr;
    size_t nleft;
    ssize_t status;
    struct pollfd pfd;

    ptr = (const char *)buf;
    nleft = count;

    if (fd < 0) {
        errno = EINVAL;
        return errno;
    }

    /* checking for closed peer connection */
    pfd.fd = fd;
    pfd.events = POLLIN | POLLHUP;
    pfd.revents = 0;
    if (poll(&pfd, 1, 0) > 0) {
        char buffer[32];
        if (recv(fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
            /* if recv returns zero (even though poll() said there is data to be
             * read), that means the connection has been closed */
            return errno ? errno : -1;
        }
    }

    while (nleft > 0) {
        status = write(fd, (const void *)ptr, nleft);

        if ((status < 0) && ((errno == EAGAIN) || (errno == EINTR)))
            continue;

        if (status < 0)
            return errno ? errno : status;

        nleft = nleft - ((size_t)status);
        ptr = ptr + ((size_t)status);
    }

    return (0);
}

int service_name_to_port_number(const char *service_name) {
    struct addrinfo *ai_list;
    int status;
    int service_number;

    if (service_name == NULL)
        return (-1);

    struct addrinfo ai_hints = {.ai_family = AF_UNSPEC};

    status = getaddrinfo(/* node = */ NULL, service_name, &ai_hints, &ai_list);
    if (status != 0) {
        ERROR("service_name_to_port_number: getaddrinfo failed: %s",
                    gai_strerror(status));
        return (-1);
    }

    service_number = -1;
    for (struct addrinfo *ai_ptr = ai_list; ai_ptr != NULL;
             ai_ptr = ai_ptr->ai_next) {
        if (ai_ptr->ai_family == AF_INET) {
            struct sockaddr_in *sa;

            sa = (void *)ai_ptr->ai_addr;
            service_number = (int)ntohs(sa->sin_port);
        } else if (ai_ptr->ai_family == AF_INET6) {
            struct sockaddr_in6 *sa;

            sa = (void *)ai_ptr->ai_addr;
            service_number = (int)ntohs(sa->sin6_port);
        }

        if ((service_number > 0) && (service_number <= 65535))
            break;
    }

    freeaddrinfo(ai_list);

    if ((service_number > 0) && (service_number <= 65535))
        return (service_number);
    return (-1);
}

void set_sock_opts(int sockfd)
{
    int status;
    int socktype;

    status = getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &socktype,
                                            &(socklen_t){sizeof(socktype)});
    if (status != 0) {
        WARNING("set_sock_opts: failed to determine socket type");
        return;
    }

    if (socktype == SOCK_STREAM) {
        status =
                setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &(int){1}, sizeof(int));
        if (status != 0)
            WARNING("set_sock_opts: failed to set socket keepalive flag");

#ifdef TCP_KEEPIDLE
        int tcp_keepidle = ((CDTIME_T_TO_MS(plugin_get_interval()) - 1) / 100 + 1);
        status = setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepidle,
                                                sizeof(tcp_keepidle));
        if (status != 0)
            WARNING("set_sock_opts: failed to set socket tcp keepalive time");
#endif

#ifdef TCP_KEEPINTVL
        int tcp_keepintvl =
                ((CDTIME_T_TO_MS(plugin_get_interval()) - 1) / 1000 + 1);
        status = setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepintvl,
                                                sizeof(tcp_keepintvl));
        if (status != 0)
            WARNING("set_sock_opts: failed to set socket tcp keepalive interval");
#endif
    }
}
