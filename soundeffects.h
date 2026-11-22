#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

class SoundEffects
{
public:
    static void button();
    static void move();
    static void capture();
    static void notification();

private:
    static void tone(unsigned int frequency, unsigned int milliseconds);
};

#endif
