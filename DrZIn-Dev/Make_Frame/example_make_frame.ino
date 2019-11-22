uint8_t data[] = {'1', '2', '3', '4'};
uint8_t buffer[sizeof(data) + 2];
void setup()
{
    Serial.begin(9600);
    make_Frame(buffer, data, sizeof(data));
    for (int i = 0; i < sizeof(buffer); i++)
    {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
}

void loop()
{
}

void make_Frame(uint8_t *buffer, uint8_t *data, uint8_t size)
{
    uint8_t idx = 0;
    buffer[idx] = 21;
    for (idx = 1; idx <= size; idx++)
    {
        buffer[idx] = data[idx - 1];
    }
    buffer[idx] = 21;
}
