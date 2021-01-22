#include "header/epoll_interface.h"

void epoll_init(int* epoll_fd)
{
    *epoll_fd = epoll_create1(0);
    if (*epoll_fd == -1) {
        perror("Couldn't create epoll FD");
        exit(1);
    }
}


void epoll_add_handler(int epoll_fd, struct epoll_event_handler* handler, uint32_t event_mask)
{
    struct epoll_event event;

    memset(&event, 0, sizeof(struct epoll_event));
    event.data.ptr = handler;
    event.events = event_mask;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd, &event) == -1) {
        perror("Couldn't register server socket with epoll");
        exit(-1);
    }
}


void epoll_remove_handler(int epoll_fd, struct epoll_event_handler* handler)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, handler->fd, NULL);
}


struct free_list_entry {
    void* block;
    struct free_list_entry* next;
};

struct free_list_entry* free_list = NULL;


void epoll_add_to_free_list(void* block) 
{
    struct free_list_entry* entry = (free_list_entry*)malloc(sizeof(struct free_list_entry));
    entry->block = block;
    entry->next = free_list;
    free_list = entry;
}


void epoll_do_reactor_loop(int epoll_fd)
{
    struct epoll_event current_epoll_event;

    while (1) {
        struct epoll_event_handler* handler;

        epoll_wait(epoll_fd, &current_epoll_event, 1, -1);
        handler = (struct epoll_event_handler*) current_epoll_event.data.ptr;
        handler->handle(handler, current_epoll_event.events);

        struct free_list_entry* temp;
        while (free_list != NULL) {
            free(free_list->block);
            temp = free_list->next;
            free(free_list);
            free_list = temp;
        }
    }
}

// ======================================================== EPOLL C++ ================================================


    auto epoll_proxy::create_and_bind(string const& port)
    {
        struct addrinfo hints;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
        hints.ai_socktype = SOCK_STREAM; /* TCP */
        hints.ai_flags = AI_PASSIVE; /* All interfaces */

        struct addrinfo* result;
        int sockt = getaddrinfo(nullptr, port.c_str(), &hints, &result);
        if (sockt != 0)
        {
            cerr << "[E] getaddrinfo failed\n";
            return -1;
        }

        struct addrinfo* rp;
        int socketfd;
        for (rp = result; rp != nullptr; rp = rp->ai_next)
        {
            socketfd = Socket();
            if (socketfd == -1)
                continue;
            
            Setsockopt(socketfd);
            if (Bind(socketfd, stoi(port)) == 0)
                break;

            close(socketfd);
        }

        if (rp == nullptr)
        {
            cerr << "[E] bind failed\n";
            return -1;
        }

        freeaddrinfo(result);

        return socketfd;
    }

    auto epoll_proxy::sock_set_non_blocking(int socketfd)
    {
        int flags = fcntl(socketfd, F_GETFL, 0);
        if (flags == -1)
        {
            cerr << "[E] fcntl failed (F_GETFL)\n";
            return false;
        }

        flags |= O_NONBLOCK;
        int s = fcntl(socketfd, F_SETFL, flags);
        if (s == -1)
        {
            cerr << "[E] fcntl failed (F_SETFL)\n";
            return false;
        }

        return true;
    }

    auto epoll_proxy::accept_connection(int socketfd, struct epoll_event& event, int epollfd)
    {
        struct sockaddr in_addr;
        socklen_t in_len = sizeof(in_addr);

        int infd = Accept(socketfd);
        if (infd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // Done processing incoming connections
            {
                return false;
            }
            else
            {
                cerr << "[E] accept failed\n";
                return false;
            }
        }

        std::string hbuf(NI_MAXHOST, '\0');
        std::string sbuf(NI_MAXSERV, '\0');
        if (getnameinfo(&in_addr, in_len,
                        const_cast<char*>(hbuf.data()), hbuf.size(),
                        const_cast<char*>(sbuf.data()), sbuf.size(),
                        NI_NUMERICHOST | NI_NUMERICSERV) == 0)
        {
            std::cout << "[I] Accepted connection on descriptor " << infd << "(host=" << hbuf << ", port=" << sbuf << ")" << "\n";
        }

        if (!sock_set_non_blocking(infd))
        {
            cerr << "[E] make_socket_nonblocking failed\n";
            return false;
        }

        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, &event) == -1)
        {
            cerr << "[E] epoll_ctl failed\n";
            return false;
        }

        return true;
    }

    auto epoll_proxy::read_data(int fd)
    {
        char buf[512];
        auto count = read(fd, buf, 512);
        if (count == -1)
        {
            if (errno == EAGAIN) // read all data
            {
                return false;
            }
        }
        else if (count == 0) // EOF - remote closed connection
        {
            std::cout << "[I] Close " << fd << "\n";
            close(fd);
            return false;
        }

        std::cout << fd << " says: " <<  buf;

        return true;
    }