#include "Animated.h"

// Par Copie
Animated::Animated(const Animated& Cpy) :
sf::Sprite(Cpy)
{
    myCurrentFrame = Cpy.myCurrentFrame;
    myTime = Cpy.myTime;
    myElapsedTime = Cpy.myElapsedTime;
    Paused = Cpy.Paused;
    myAnim = Cpy.myAnim;
    myLoop = Cpy.myLoop;
}

// Par défault
Animated::Animated(bool Play, bool Loop, float Time)
{
    myAnim = NULL;
    myCurrentFrame = 0;
    myTime = Time;
    myElapsedTime = Time;
    Paused = !Play;
    myLoop = Loop;
}

// Directement avec une Anim
Animated::Animated(Anim* NewAnim, bool Play, bool Loop, float Time)
{
    myTime = Time;
    myElapsedTime = Time;
    Paused = !Play;
    myLoop = Loop;
    myAnim = NewAnim;

    SetFrame(0);
}

// Destructeur
Animated::~Animated()
{

}

// Comme 'SetImage', sauf qu'on lui fournit l'Anim
void Animated::SetAnim(Anim* NewAnim)
{
    myAnim = NewAnim;

    SetFrame(0);
}

// Retourne un pointeur sur l'anim
Anim* Animated::GetAnim()
{
    return myAnim;
}

// Passer à l'image numéro X
void Animated::SetFrame(int Frame)
{
    if ( myAnim != NULL)
    {
        if (myAnim->Size() > 0)
        {
            if ((*myAnim)[Frame].Image != NULL)
                SetImage(*((*myAnim)[Frame].Image));

            SetSubRect((*myAnim)[Frame].Rect);

            SetColor((*myAnim)[Frame].Color);

            myCurrentFrame = Frame;
        }
    }
}

//Retourne La frame courante
int Animated::GetCurrentFrame()
{
    return myCurrentFrame;
}

// Fixer le temps entre chaques Frame
void Animated::SetFrameTime(float Time)
{
    myTime = Time;
}

// retourne le temps entre chaques Frame
float Animated::GetFrameTime()
{
    return myTime;
}

// Jouer en boucle ?
void Animated::SetLoop(bool Loop)
{
    myLoop = Loop;
}

// Jouer en boucle ?
bool Animated::IsLoop()
{
    return myLoop;
}

// Met en pause la lecture
void Animated::Pause()
{
    Paused = true;
}

// Relance la lecture
void Animated::Play()
{
    Paused = false;
}

// Met en pause la lecture, et 'rembobine'
void Animated::Stop()
{
    SetFrame(0);
    myElapsedTime = myTime;
    Paused = true;
}

// Est En pause ?
bool Animated::IsPaused()
{
    return Paused;
}

// Est Stoppé ?
bool Animated::IsStoped()
{
    return (Paused && (myCurrentFrame == 0) && (myElapsedTime == myTime));
}

// Fonction à appeler à chaque tours de boucle, prend le temps
// écoulé depuis le dernier appel à la fonction en paramètre
void Animated::anim(float ElapsedTime)
{
    // Si il n'est pas en pause et que l'animation est valide
    if (!Paused && myAnim != NULL)
    {
        // on retranche le temps écoulé a notre compteur
        myElapsedTime -= ElapsedTime;

        // Si Le temps entre chaque frame est atteint
        if (myElapsedTime <= 0.f)
        {
            // On réinitialise notre compteur
            myElapsedTime = myTime;

            // On passe a la frame suivante
            if (myCurrentFrame + 1 < myAnim->Size())
                myCurrentFrame++;
            else
            {
                // Ou on a la premiere
                if (myLoop)
                    myCurrentFrame = 0;
                else
                {
                    // Si le mode Loop est désactivé, on stop l'animation
                    Stop();
                }
            }

            // On change la frame
            SetFrame(myCurrentFrame);
        }
    }
}
