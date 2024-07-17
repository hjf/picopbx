#ifndef DIALTONE_H
#define DIALTONE_H

class Dialtone
{
public:
    Dialtone(){};
    void start();
    void start(uint16_t duration);
    void stop();

private:
    static void dialtone_cb();
    bool started = false;
    static volatile int stop_at;
    static volatile int count;
};

#endif