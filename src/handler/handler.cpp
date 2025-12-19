#include <handler/handler.h>
#include <handler/handler_room.h>
#include <handler/handler_setting.h>
#include <server/server_manager.h>


void initHandlers() {
  handlerRooms();
  handlerSettings();
}
