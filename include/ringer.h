#ifndef RINGER_H
#define RINGER_H

class Ringer
{
private:
    int _q1, _q2, _q3, _q4;

public:
    Ringer(int q1, int q2, int q3, int q4);
    void start();
    void stop();
};

#endif // RINGER_H