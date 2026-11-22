#include "soundeffects.h"
#include <QSettings>
#include <QApplication>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

void SoundEffects::tone(unsigned int frequency, unsigned int milliseconds)
{
    QSettings settings("ChessPBL", "ChessArena");
    if (!settings.value("settings/sound", true).toBool())
        return;
#ifdef Q_OS_WIN
    ::Beep(frequency, milliseconds);
#else
    QApplication::beep();
    Q_UNUSED(frequency);
    Q_UNUSED(milliseconds);
#endif
}

void SoundEffects::button()      { tone(780, 35); }
void SoundEffects::move()        { tone(560, 45); }
void SoundEffects::capture()     { tone(360, 95); }
void SoundEffects::notification(){ tone(920, 90); }
