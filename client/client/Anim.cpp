#include "Anim.h"

// Par défaut
Anim::Anim()
{

}

// déstructeur
Anim::~Anim()
{

}

// Par copie
Anim::Anim(const Anim& Cpy)
{
    myFrame = Cpy.myFrame;
}

// Ajouter une frame
void Anim::PushFrame(const Frame& NewFrame)
{
    myFrame.push_back(NewFrame);
}

// Nombre de frame(s)
size_t Anim::Size() const
{
    return myFrame.size();
}

// Accès a la frame numéro N
Frame& Anim::operator [] (size_t N)
{
    return myFrame[N];
}



// Plus tard, nous pourrons ajouter différentes fonctions liées au fait qu'une 'Anim' est une ressource
// Par exemple : LoadFromFile, SaveToFile etc...
