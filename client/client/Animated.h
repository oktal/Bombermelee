#ifndef ANIMATED_H
#define ANIMATED_H

#include "Anim.h"

// Un sprite animé

// Il est composé de :

// Le temps écoulé entre chaque frame
// Un pointeur sur l'animation qu'il doit lire

// Des fonctions de lecture :
// Play, Pause, Stop, Loop
class Animated : public sf::Sprite
{
    public:

    // Par Copie
    Animated(const Animated& Cpy);

    // Par défault
    Animated(bool Play = false, bool Loop = true, float Time = 0.f);

    // Directement avec une Anim
    Animated(Anim* NewAnim, bool Play = false, bool Loop = true, float Time = 0.f);

    // Destructeur
    virtual ~Animated();

    // Comme 'SetImage', sauf qu'on lui fournit l'Anim
    void SetAnim(Anim* NewAnim);

    // Retourne un pointeur sur l'anim
    Anim* GetAnim();

    // Passer à l'image numéro X
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

    // Est stoppé ?
    bool IsStoped();

    // Fonction à appeler à chaque tour de boucle, prend le temps
    // écoulé depuis le dernier appel à la fonction en paramètre
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
