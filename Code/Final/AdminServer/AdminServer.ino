#include <WiFi.h>

#define SSID "Admin PIT"
#define PORT 1337

bool bAdminLogging = false;
bool bAdminActive = false;

/**
 * @brief Fonction qui affiche le logo du projet
 */
void printBanner() {
  // "TraceurDessinateur" en art ASCII
  Serial.println(" ______                                    ");
  Serial.println("(_) |                                      ");
  Serial.println("    | ,_    __,   __   _          ,_       ");
  Serial.println("  _ |/  |  /  |  /    |/  |   |  /  |      ");
  Serial.println(" (_/    |_/\\_/|_/\\___/|__/ \\_/|_/   |_/    ");
  Serial.println();
  Serial.println("  ____                                                   ");
  Serial.println(" (|   \\             o                                    ");
  Serial.println("  |    | _   ,   ,      _  _    __, _|_  _          ,_   ");
  Serial.println(" _|    ||/  / \\_/ \\_|  / |/ |  /  |  |  |/  |   |  /  |  ");
  Serial.println("(/\\___/ |__/ \\/  \\/ |_/  |  |_/\\_/|_/|_/|__/ \\_/|_/   |_/");
  Serial.println();
}


void setup()
{
	
}

void loop()
{
	
}
