#include <handler/handler_room.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "spiff_manager.h"
#include <controllers/relay_control.h>
#include "helper.h"
#include <server/web_sockets.h>
#include <server/server_manager.h>
#include "debug.h"

#include "pzem_config.h"


void handlerRooms(){
  getRooms();
  hanlderSetChannels();
}

void getRooms(){
  wsEvents.on("get:pzem-data", [](JsonVariant data, AsyncWebSocketClient *client) {
      broadcastPzemData();
    });

  wsEvents.on("get:rooms-data", [](JsonVariant data, AsyncWebSocketClient *client) {
    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docDatabase;

    if (!loadJsonFromFile("/database.json", docDatabase)) {
      LOG_ERROR("Gagal membaca database.json");
      client->text(makeJsonMessageWS("error", "Gagal membaca database"));
      return;
    }

    JsonArray channels = docDatabase["channels"].as<JsonArray>();
    JsonArray rooms = docDatabase["rooms"].as<JsonArray>();

    for (JsonObject room : rooms) {
      String roomId = room["id"];
      int channelCount = 0;
      for(JsonObject channel : channels) {
        if(channel["roomId"] == roomId){
          channelCount++;
        }
      }
      room["channelCount"] = channelCount;
    }

      LOG_INFO("Sending all rooms data to client %u\n", client->id());
      client->text( makeJsonDataWS("rooms:data", rooms) );
  });

  wsEvents.on("get:channels-data", [](JsonVariant data, AsyncWebSocketClient *client) {
    String reqRoomId = data["roomId"];
    LOG_INFO("Client %u requested room %s\n", client->id(), reqRoomId.c_str());

    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
      client->text(makeJsonMessageWS("error", "database tidak ditemukan"));
      return;
    }

    JsonDocument docDatabase;

    if (!loadJsonFromFile("/database.json", docDatabase)) {
      LOG_ERROR("Gagal membaca database.json");
      client->text(makeJsonMessageWS("error", "Gagal membaca database"));
      return;
    }

    JsonArray channels = docDatabase["channels"].as<JsonArray>();

    if(reqRoomId == "undefined" || reqRoomId == "null") {
      LOG_INFO("Sending all channels data to client %u\n", client->id());
      client->text( makeJsonDataWS("channels:data", channels) );
      return;
    }

    JsonDocument docOutput;
    JsonArray docChannels = docDatabase["channels"].as<JsonArray>();
    JsonArray filteredChannels = docOutput.to<JsonArray>();
    for (JsonObject channel : channels) {
      if (channel["roomId"] == reqRoomId) {
        filteredChannels.add(channel);
      }
    }

    LOG_INFO("Sending filtered channels data to client %u\n", client->id());
    client->text( makeJsonDataWS("channels:data", filteredChannels) );
  });
}

void hanlderSetChannels(){
  AsyncCallbackJsonWebHandler *handlerSetChannel = new AsyncCallbackJsonWebHandler("/api/channels/toggle", [](AsyncWebServerRequest *request, JsonVariant &json){
    LOG_INFO("Received /api/channel request");

    if( !json.is<JsonObject>()) {
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    bool state = reqObj["state"];
    JsonArray channelIds = reqObj["channelIds"].as<JsonArray>();
    String roomId = reqObj["roomId"];


    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
      request->send(500, "application/json", makeJsonMessage("database tidak ditemukan"));
      return;
    }

    JsonDocument docDatabase;

    if (!loadJsonFromFile("/database.json", docDatabase)) {
      LOG_ERROR("Gagal membaca database.json");
      request->send(500, "application/json", makeJsonMessage("Gagal membaca database"));
      return;
    }

    JsonArray channels = docDatabase["channels"].as<JsonArray>();


    if(channels.size() == 0) {
      LOG_ERROR("Channels data not found in database.json");
      request->send(500, "application/json", makeJsonMessage("Channels data not found in database.json"));
      return;
    }

    for(JsonVariant idVar : channelIds) {
      for (JsonObject channel : channels) {
        String id = idVar.as<String>();
        String channelId = channel["id"];
        if (channelId == id) {
          LOG_INFO("Toggling channel %s\n", id.c_str());
          channel["isOn"] = state;
          int pin = channel["pin"];
          relaySwitch(pin, state);
          break;
        }
      }
    }

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
      return;
    }
    serializeJson(docDatabase, file);
    file.close();


    JsonDocument docOutput;
    JsonArray docChannels = docDatabase["channels"].as<JsonArray>();
    JsonArray filteredChannels = docOutput.to<JsonArray>();

    for (JsonObject channel : docChannels) {
      if (channel["roomId"] == roomId) {
        filteredChannels.add(channel);
      }
    }

    broadcast("channels:data", filteredChannels);;

    request->send(200, "application/json", makeJsonMessage("Data berhasil disimpan")); 
  });
  server.addHandler(handlerSetChannel);


  AsyncCallbackJsonWebHandler *handlerUpdateChannel = new AsyncCallbackJsonWebHandler("/api/channels/update", [](AsyncWebServerRequest *request, JsonVariant &json){
    LOG_INFO("Received /api/channel request");

    if( !json.is<JsonObject>()) {
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }

    JsonObject reqObj = json.as<JsonObject>();
    String reqId = reqObj["id"];
    String reqName = reqObj["name"];
    String reqIcon = reqObj["icon"];
    String reqRoomId = reqObj["roomId"];


    if (!LittleFS.exists("/database.json")) {
      LOG_ERROR("File database.json tidak ditemukan");
      request->send(500, "application/json", makeJsonMessage("database tidak ditemukan"));
      return;
    }

    JsonDocument docDatabase;

    if (!loadJsonFromFile("/database.json", docDatabase)) {
      LOG_ERROR("Gagal membaca database.json");
      request->send(500, "application/json", makeJsonMessage("Gagal membaca database"));
      return;
    }

    JsonArray channels = docDatabase["channels"].as<JsonArray>();

    if(channels.size() == 0) {
      LOG_ERROR("Channels data not found in database.json");
      request->send(500, "application/json", makeJsonMessage("Channels data not found in database.json"));
      return;
    }

    if(reqId.isEmpty() || reqName.isEmpty() || reqIcon.isEmpty() || reqRoomId.isEmpty()) {
      LOG_ERROR("Data tidak valid");
      request->send(400, "application/json", makeJsonMessage("Data tidak valid"));
      return;
    }


    for (JsonObject channel : channels) {
      String id = channel["id"];
      if (id == reqId) {
        LOG_INFO("Updating channel %s\n", id.c_str());
        channel["name"] =  reqName ;
        channel["icon"] = reqIcon;
        channel["roomId"] = reqRoomId;
        break;
      }
    }

    File file = LittleFS.open("/database.json", "w");
    if (!file) {
      request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
      return;
    }
    serializeJson(docDatabase, file);
    file.close();

    String Sout;
    serializeJsonPretty(docDatabase, Sout);
    LOG_INFO("Updated database.json:\n%s\n", Sout.c_str());

    JsonDocument docOutput;
    JsonArray docChannels = docDatabase["channels"].as<JsonArray>();
    JsonArray filteredChannels = docOutput.to<JsonArray>();

    for (JsonObject channel : docChannels) {
      if (channel["roomId"] == reqRoomId) {
        filteredChannels.add(channel);
      }
    }

    broadcast("channels:data", filteredChannels);;

    request->send(200, "application/json", makeJsonMessage("Data berhasil disimpan")); 
  });
  server.addHandler(handlerUpdateChannel);


  server.addHandler(
    new AsyncCallbackJsonWebHandler("/api/rooms/create",
      [](AsyncWebServerRequest *request, JsonVariant &json) {
        if (!json.is<JsonObject>()) {
          request->send(400, "application/json", makeJsonMessage("Invalid JSON"));
          return;
        }
        JsonObject reqObj = json.as<JsonObject>();
        String reqName = reqObj["name"];
        String reqIcon = reqObj["icon"];
        JsonArray reqChannels = reqObj["channels"].as<JsonArray>();

        if (!LittleFS.exists("/database.json")) {
          request->send(500, "application/json", makeJsonMessage("Database not found"));
          return;
        }

        JsonDocument docDatabase;
        if (!loadJsonFromFile("/database.json", docDatabase)) {
          request->send(500, "application/json", makeJsonMessage("Failed to read database"));
          return;
        }

        JsonArray rooms = docDatabase["rooms"].as<JsonArray>();
        JsonArray channels = docDatabase["channels"].as<JsonArray>();

        if (rooms.size() == 0) {
          request->send(500, "application/json", makeJsonMessage("Rooms data not found in database.json"));
          return;
        }
        
        String newRoomId = generateId();
        newRoomId .reserve(16);

        for (JsonObject room : rooms) {
          String name = room["name"];
          if (name == reqName) {
            request->send(400, "application/json", makeJsonMessage("Room name already exists"));
            return;
          }

          String roomId = room["id"];
          if (roomId == newRoomId) {
            newRoomId = generateId();
            return;
          }
        }

        JsonObject newRoom = rooms.add<JsonObject>();
        newRoom["id"] = newRoomId;
        newRoom["name"] = reqName;
        newRoom["icon"] = reqIcon;

      if(!reqChannels.isNull()) {       
        LOG_INFO("Assigning channels to new room\n");  
          for (JsonVariant channelIdVar : reqChannels) {
            String channelId = channelIdVar["id"].as<String>();
            if (channelId.isEmpty()) continue;

            for (JsonObject channel : channels) {
              String id = channel["id"];
              if (id == channelId) {
                channel["roomId"] = String(newRoomId);
                break;
              }
            }
          }
        }

        File file = LittleFS.open("/database.json", "w");
        if (!file) {
          request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
          return;
        }
        serializeJson(docDatabase, file);
        file.close();

        for (JsonObject room : rooms) {
          String roomId = room["id"];
          int channelCount = 0;
          for(JsonObject channel : channels) {
            if(channel["roomId"] == roomId){
              channelCount++;
            }
          }
          room["channelCount"] = channelCount;
        }

        broadcast("rooms:data", rooms);

        request->send(200, "application/json", makeJsonMessage("Room created successfully"));
      }
    )
  );

  server.addHandler(
    new AsyncCallbackJsonWebHandler("/api/rooms/delete",
      [](AsyncWebServerRequest *request, JsonVariant &json) {
        if (!json.is<JsonObject>()) {
          request->send(400, "application/json", makeJsonMessage("Invalid JSON"));
          return;
        }
        JsonObject reqObj = json.as<JsonObject>();

        String reqId = reqObj["id"];

        if (!LittleFS.exists("/database.json")) {
          request->send(500, "application/json", makeJsonMessage("Database not found"));
          return;
        }

        JsonDocument docDatabase;
        if (!loadJsonFromFile("/database.json", docDatabase)) {
          request->send(500, "application/json", makeJsonMessage("Failed to read database"));
          return;
        }

        JsonArray rooms = docDatabase["rooms"].as<JsonArray>();
        JsonArray channels = docDatabase["channels"].as<JsonArray>();

        if (rooms.size() == 0) {
          request->send(500, "application/json", makeJsonMessage("Rooms data not found in database.json"));
          return;
        }


        for(JsonObject room : rooms) {
          String roomId = room["id"];
          if (roomId == reqId) {
            rooms.remove(room);
            break;
          }
        }

        for (JsonObject channel : channels) {
          String channelRoomId = channel["roomId"];
          if (channelRoomId == reqId) {
            channel["roomId"] = "";
          }
        }

        File file = LittleFS.open("/database.json", "w");
        if (!file) {
          request->send(500, "application/json", makeJsonMessage("Gagal menulis file"));
          return;
        }
        serializeJson(docDatabase, file);
        file.close();


        for (JsonObject room : rooms) {
          String roomId = room["id"];
          int channelCount = 0;
          for(JsonObject channel : channels) {
            if(channel["roomId"] == roomId){
              channelCount++;
            }
          }
          room["channelCount"] = channelCount;
        }

        broadcast("rooms:data", rooms);

        request->send(200, "application/json", makeJsonMessage("Room deleted successfully"));
      }
    )
  );


  server.addHandler(
    new AsyncCallbackJsonWebHandler("/api/rooms/update",[](AsyncWebServerRequest *request, JsonVariant &json) {
      if (!json.is<JsonObject>()) {
        request->send(400, "application/json",
                      makeJsonMessage("Invalid JSON"));
        return;
      }

      JsonObject req = json.as<JsonObject>();

      const char* reqId   = req["id"]   | "";
      const char* reqName = req["name"] | "";
      const char* reqIcon = req["icon"] | "";

      JsonArray reqChannels = req["channels"].as<JsonArray>();

      if (!LittleFS.exists("/database.json")) {
        request->send(500, "application/json",makeJsonMessage("Database not found"));
        return;
      }

      // 🔹 Pakai StaticJsonDocument (sesuaikan ukuran!)
      JsonDocument docDatabase;

      if (!loadJsonFromFile("/database.json", docDatabase)) {
        request->send(500, "application/json",makeJsonMessage("Failed to read database"));
        return;
      }

      JsonArray rooms    = docDatabase["rooms"].as<JsonArray>();
      JsonArray channels = docDatabase["channels"].as<JsonArray>();

      if (rooms.isNull()) {
        request->send(500, "application/json",makeJsonMessage("Rooms data not found"));
        return;
      }

      for (JsonObject room : rooms) {
        const char* id = room["id"];
        if (id && strcmp(id, reqId) == 0) {
          LOG_INFO("Updating room %s", id);
          room["name"] = reqName;
          room["icon"] = reqIcon;
          break;
        }
      }


      for (JsonVariant reqChanel : reqChannels) {
        const char* reqChannelId = reqChanel["id"].as<const char*>();
        if (!reqChannelId) continue;

        for (JsonObject channel : channels) {
          const char* id = channel["id"];
          if (id && strcmp(id, reqChannelId) == 0) {
            channel["roomId"] = reqId;
            break;
          }
        }
      }

      File file = LittleFS.open("/database.json", "w");
      if (!file) {
        request->send(500, "application/json",makeJsonMessage("Write failed"));
        return;
      }

      serializeJson(docDatabase, file);
      file.close();

      for (JsonObject room : rooms) {
        String roomId = room["id"];
        int channelCount = 0;
        for(JsonObject channel : channels) {
          if(channel["roomId"] == roomId){
            channelCount++;
          }
        }
        room["channelCount"] = channelCount;
      }

      broadcast("rooms:data", rooms);

      request->send(200, "application/json",makeJsonMessage("Room updated successfully"));
    })
  );
}



