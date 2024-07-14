#ifndef RINGER_H
#define RINGER_H

class Ringer
{
private:
    int _q1, _q2, _q3, _q4, _relay;
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
    Ringer(int q1, int q2, int q3, int q4, int ringer_relay, int freq);
    void start();
    void start(int duration, void (*timeout_callback)(void));
    void stop();
    void handle();
};

#endif // RINGER_H