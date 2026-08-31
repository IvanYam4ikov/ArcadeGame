// ArcadeTexture.h

// DESCRIPTION
// ArcadeTexture is a utility of the arcade system to help 
// render, create, and handle images and text. 


#ifndef ARCADETEXTURE_H
#define ARCADETEXTURE_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Config.h"
#include <string>

class ArcadeTexture
{
public:
	// CONSTRUCTOR
	ArcadeTexture();

	// DESTRUCTOR
	~ArcadeTexture();
	
	// ACCESSORS
	SDL_Rect getImageDestination() {return imageDestination;}

	TTF_Font* getFontRenderer() {return fontRen;}

	SDL_Texture* getSDLTexture(){return texture;}

	// return position of texture on the screen
	int getX() { return imageDestination.x; }

	int getY() { return imageDestination.y; }

	int getWidth() { return imageDestination.w; }

	int getHeight() { return imageDestination.h; }

	// MUTATORS
	void setRenderer(SDL_Renderer* renderer_in) { texRen = renderer_in; }

	// these are for the source image from the file. Only use
	// these if you want to crop an image out, otherwise the entire
	// source image is used by default
	void setXSrc(int x_in);

	void setYSrc(int y_in);

	void setPositionSrc(int x_in, int y_in);

	void setSizeSrc(int w_in, int h_in);

	// these are for the destination image that renders to the screen
	void setX(int x_in);

	void setY(int y_in);

	void setPosition(int x_in, int y_in);

	void setSize(int w_in, int h_in);

	// this method is used to create image textures that can be rendered
	bool loadFromFile(std::string path);

	// this method is used to create text textures that can be rendered
	bool loadFromRenderedText(std::string filePath, int size, std::string textureText, SDL_Color textColor);
	bool loadFromRenderedTextWrapped(std::string filePath, int size,
		std::string textureText, SDL_Color textColor, int wrapWidth);
	bool loadFromRenderedTextCenteredWrapped(std::string filePath, int size,
		std::string textureText, SDL_Color textColor, int wrapWidth);

	// this method is used to edit a texture to make it look selected
	void setSelected(bool selected_in);

	// this is used to deallocate an ArcadeTexture object by freeing the SDL texture
	void myFree();

	// this method renders an ArcadeTexture to the screen using the underlying SDL texture
	void render(SDL_Renderer* renderer);

	bool isEntireSource;
	bool isEntireDestination;
private:
	//The actual texture
	SDL_Texture* texture ;

	//Renderer
	SDL_Renderer* texRen;

	// the font object
	TTF_Font* fontRen;

	// image rectangle destination
	SDL_Rect imageDestination;

	// rectangle crop
	SDL_Rect textureClip;

	// original image dimensions
	int fileWidth;
	int fileHeight;
};

#endif
