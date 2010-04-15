#include "Anim.h"

// Par d�faut
Anim::Anim()
{

}

// d�structeur
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

// Acc�s a la frame num�ro N
Frame& Anim::operator [] (size_t N)
{
    return myFrame[N];
}



// Plus tard, nous pourrons ajouter diff�rentes fonctions li�es au fait qu'une 'Anim' est une ressource
// Par exemple : LoadFromFile, SaveToFile etc...
