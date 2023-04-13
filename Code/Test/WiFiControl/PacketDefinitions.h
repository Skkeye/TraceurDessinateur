/**
 * @file
 * @brief Definitions des index et du format des packets
 */

#include <MsgPacketizer.h>

// Index MOSI: Controlleur -> Manette
// Index MISO: Manette -> Controlleur
#define INDEX_MOSI 0x77
#define INDEX_MISO 0x42

/**
 * @class DataPacket
 * @brief Packet pour transmettre les donnees de la Manette au Controlleur
 *
 * sX: IMU X angle
 * sY: IMU Y angle
 * sZ: Bouton
 */
struct DataPacket {
  short sX; // 0x00-0x7f: negative movement; 0x80-0xff: positive movement
  short sY; // 0x00-0x7f: negative movement; 0x80-0xff: positive movement
  short sZ; // 0x00-0x7f: negative movement; 0x80-0xff: positive movement
  MSGPACK_DEFINE(sX, sY, sZ);
};

/**
 * @class CommandPacket
 * @brief Packet pour transmettre une commande du Controlleur a la Manette
 *
 * bRecalibrate: true -> Recalibrer le IMU
 */
struct CommandPacket {
  short bRecalibrate; // 0x01: true; 0x00: false;
  MSGPACK_DEFINE(bRecalibrate);
};

