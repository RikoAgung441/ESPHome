#include <handler/handler.h>
#include <handler/handler_room.h>
#include <handler/handler_setting.h>
#include "server_manager.h"


void initHandlers() {
  handlerRoom();
  handlerSettings();
}
