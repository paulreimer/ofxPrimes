#include "ofxSimpleGuiContent.h"

map<int,ofImage> ofxSimpleGuiContent::_chrome;

//---------------------------------------------------------------------
ofxSimpleGuiContent::ofxSimpleGuiContent(string name, ofBaseDraws& content, float fixwidth)
: ofxSimpleGuiControl(name)
{
	this->content = &content;
	this->fixwidth  = fixwidth;
	this->textAlignment = ALIGN_CENTER;
	controlType = "Content";
	setup();
	initChrome();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiContent::setup()
{
	if (content == NULL)
		return;
	
	fixheight = fixwidth * content->getHeight()/content->getWidth();
	setSize(fixwidth,
			fixheight + 2*config->padding.y /*+ config->font.stringHeight(name)*/);

}

//---------------------------------------------------------------------
void
ofxSimpleGuiContent::drawWidget(float x, float y)
{
	if(content->getWidth() == 0 && content->getHeight() ==0) return;
	
	ofPoint textOffset(0,0); 
	switch (textAlignment)
	{
		case ALIGN_RIGHT:
			textOffset.x = width - config->font.stringWidth(name) - config->fontOffset.x;
			break;
		case ALIGN_CENTER:
			textOffset.x = (width - config->font.stringWidth(name))/2;
			break;
		default:
		case ALIGN_LEFT:
		case ALIGN_JUSTIFIED:
			textOffset.x = config->fontOffset.x;
			break;
	}

	setPos(x, y);
	setup();
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	ofEnableAlphaBlending();
	ofFill();
	//		glColor4f(0, 0, 0, 0.8f);
	//		ofRect(0, 0, width, fixheight);
	
	ofSetColor(0xffffff);
	content->draw(0, 0, width, fixheight);
	
	double o=0,topCaptionBorderHeight;
	
	map<int,ofImage>::iterator im_it;
	
	int textHeight = config->font.stringHeight(name);
	
	/*
	 im_it = chrome().find(CAPTION_BORDER_LEFT);
	 if (im_it != chrome().end())
	 {
	 ofImage& im = im_it->second;
	 o = im.width * height/im.height;
	 im.draw(x - padding.x, y - padding.y, width + 2*padding.x, height + 2*padding.y);
	 }
	 
	 im_it = chrome().find(CAPTION_BORDER_RIGHT);
	 if (im_it != chrome().end())
	 {
	 ofImage& im = im_it->second;
	 o = im.width * height/im.height;
	 im.draw(x - padding.x, y - padding.y, width + 2*padding.x, height + 2*padding.y);
	 }
	 */	
	im_it = chrome().find(CAPTION_BORDER_TOP);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.height * width/im.width;
		textOffset.y = o;
		topCaptionBorderHeight = o;
		
		im.draw(-config->padding.x, fixheight + config->padding.y, width + 2*config->padding.x, o);
	}

	im_it = chrome().find(CAPTION_FILL_MIDDLE);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		im.draw(-config->padding.x, fixheight + topCaptionBorderHeight, width + 2*config->padding.x, textHeight + 2*config->padding.y);
	}	
	
	im_it = chrome().find(CAPTION_BORDER_BOTTOM);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.height * width/im.width;
		
		im.draw(-config->padding.x, fixheight + textHeight + 2*config->padding.y + topCaptionBorderHeight, width + 2*config->padding.x, o);
	}
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	im_it = chrome().find(CAPTION_BORDER_TOP);

	setTextColor();
	config->font.drawString(name, textOffset.x, fixheight + config->padding.y + textOffset.y + config->fontOffset.y);
	ofDisableAlphaBlending();
	glPopMatrix();
}


//---------------------------------------------------------------------
inline map<int,ofImage>&
ofxSimpleGuiContent::chrome()
{
	return _chrome;
}

//---------------------------------------------------------------------
void
ofxSimpleGuiContent::resetChrome()
{
//		chrome()[BORDER_LEFT]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-left.png");
//		chrome()[BORDER_RIGHT]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-right.png");
	chrome()[BORDER_TOP]	.loadImage(config->chromeDir + "ofxSimpleGuiContent-border-top.png");
//		chrome()[BORDER_BOTTOM]	.loadImage(config->chromeDir + "chrome/ofxSimpleGuiControl-border-bottom.png");
	chrome()[FILL_MIDDLE]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-fill-middle.png");
	chrome()[CAPTION_BORDER_TOP]	.loadImage(config->chromeDir + "ofxSimpleGuiContent-caption-border-top.png");
	chrome()[CAPTION_FILL_MIDDLE]	.loadImage(config->chromeDir + "ofxSimpleGuiContent-caption-fill-middle.png");
	chrome()[CAPTION_BORDER_BOTTOM]	.loadImage(config->chromeDir + "ofxSimpleGuiContent-caption-border-bottom.png");

	ofTexture& tex = chrome()[FILL_MIDDLE].getTextureReference();
	tex.bind();
	glTexParameterf(tex.texData.textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	tex.unbind();

	tex = chrome()[CAPTION_FILL_MIDDLE].getTextureReference();
	tex.bind();
	glTexParameterf(tex.texData.textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	tex.unbind();
}
