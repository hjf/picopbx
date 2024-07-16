#ifndef DIALTONE_H
#define DIALTONE_H

class Dialtone
{
public:
    Dialtone();
    ~Dialtone();
    void start();
    void start(uint16_t duration);
    void stop();

private:
    bool started = false;
};

#endif