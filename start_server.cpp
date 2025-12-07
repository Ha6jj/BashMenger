#include "./src/Server.hpp"

int main()
{
    uint16_t port = 54001;
    io::io_context io_context;
    RoomManager room_manager;
    Server serv(io_context, port, room_manager);
    serv.async_accept();
    io_context.run();
    return 0;
}