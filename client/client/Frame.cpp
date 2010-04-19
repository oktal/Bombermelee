#include "Frame.h"

// Par défaut
Frame::Frame(const sf::Color& NewColor)
{
    Image = NULL;
    Color = NewColor;
}

// Par copie
Frame::Frame(const Frame& Cpy)
{
    Image = Cpy.Image;
    Rect = Cpy.Rect;
    Color = Cpy.Color;
}

// Image et Rect
Frame::Frame(sf::Image* NewImage, const sf::Rect<int>& NewRect, const sf::Color& NewColor)
{
    Image = NewImage;
    Rect = NewRect;
    Color = NewColor;
}

// Image (Le Rect est au dimension de l'image)
Frame::Frame(sf::Image* NewImage, const sf::Color& NewColor)
{
    Image = NewImage;
    if (Image != NULL)
        Rect = sf::Rect<int>(0, 0, Image->GetWidth(), Image->GetHeight());

    Color = NewColor;
}

// destructeur
Frame::~Frame()
{

}
