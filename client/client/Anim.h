#ifndef ANIM_H
#define ANIM_H

#include <vector>

#include "Frame.h"

// La classe animation n'est qu'un 'vector' de Frame
class Anim
{
    public:

    // par défaut
    Anim();

    // destructeur
    virtual ~Anim();

    // Par copie
    Anim(const Anim& Cpy);

    // Ajouter une Frame
    void PushFrame(const Frame& NewFrame);

    // Nombre de Frame(s)
    size_t Size() const;

    // Accès a la frame numéro N
    Frame& operator [] (size_t N);

    // Plus tard, nous pourrons ajouter différentes fonctions liées au fait qu'une 'Anim' est une ressource
    // Par exemple : LoadFromFile, SaveToFile etc...

    private:
    std::vector< Frame > myFrame;

};

#endif // ANIM_H
