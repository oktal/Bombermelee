#ifndef ANIMATED_H
#define ANIMATED_H

#include "Anim.h"

// Un sprite anim�

// Il est compos� de :

// Le temps �coul� entre chaque frame
// Un pointeur sur l'animation qu'il doit lire

// Des fonctions de lecture :
// Play, Pause, Stop, Loop
class Animated : public sf::Sprite
{
    public:

    // Par Copie
    Animated(const Animated& Cpy);

    // Par d�fault
    Animated(bool Play = false, bool Loop = true, float Time = 0.f);

    // Directement avec une Anim
    Animated(Anim* NewAnim, bool Play = false, bool Loop = true, float Time = 0.f);

    // Destructeur
    virtual ~Animated();

    // Comme 'SetImage', sauf qu'on lui fournit l'Anim
    void SetAnim(Anim* NewAnim);

    // Retourne un pointeur sur l'anim
    Anim* GetAnim();

    // Passer � l'image num�ro X
    void SetFrame(int Frame);

    // Retourne la Frame courante
    int GetCurrentFrame();

    // Fixer le temps entre chaques Frame
    void SetFrameTime(float Time);

    // Retourne le temps entre chaques Frame
    float GetFrameTime();

    // Jouer en boucle ?
    void SetLoop(bool Loop);

    // Jouer en boucle ?
    bool IsLoop();

    // Met en pause la lecture
    void Pause();

    // Relance la lecture
    void Play();

    // Met en pause la lecture, et 'rembobine'
    void Stop();

    // Est en pause ?
    bool IsPaused();

    // Est stopp� ?
    bool IsStoped();

    // Fonction � appeler � chaque tour de boucle, prend le temps
    // �coul� depuis le dernier appel � la fonction en param�tre
    void anim(float ElapsedTime);

    private:
    float myTime;
    float myElapsedTime;
    bool Paused;
    bool myLoop;
    Anim* myAnim;
    int myCurrentFrame;
};


#endif // ANIMATED_H
