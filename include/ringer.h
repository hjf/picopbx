#ifndef RINGER_H
#define RINGER_H

class Ringer
{
private:
    int _a1, _a2, _en, _relay;
    unsigned int last_change = 0;
    unsigned int stop_at = 0;
    int period = 20;
    bool running = false;
    bool forward = true;
    int ring_counter = 0;
    void (*timeout_callback)(void) = NULL;
    void set_forward();
    void set_reverse();
    void set_coasting();

public:
    Ringer(int a1, int a2, int en, int relay, int freq);
    void start();
    void start(int duration, void (*timeout_callback)(void));
    void stop();
    void handle();
};

#endif // RINGER_H