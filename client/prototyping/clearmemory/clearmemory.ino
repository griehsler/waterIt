#include <Storage.h>

Storage _storage;

void setup()
{
    Serial.begin(115200);

    Serial.print("clearing internal storage ... ");
    _storage.deleteAllFiles();
    Serial.println("done");
}

void loop()
{}