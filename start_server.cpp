#include "./src/Server.hpp"

int main()
{
    uint16_t port = 54001;
    io::io_context io_context;
    Server serv(io_context, port);
    serv.async_accept();
    io_context.run();
    return 0;
}