/*
  ==============================================================================

    MPosDials.h
    Created: 22 Nov 2022 12:20:03pm
    Author:  Cardenes

  ==============================================================================
*/
float pi = 2 * std::acos(0.0f);
#pragma once

class MPosDials : public Component {
public:
    float centerX, centerY;
    MPosDials() 
        #ifdef JUCE_DEMO_RUNNER
            : AudioAppComponent(getSharedAudioDeviceManager(0, 2))
        #endif
    {

        setSize(800, 600);
        centerX = getWidth() / 2;
        centerY = getHeight() / 2;
    }
 

    void paint(Graphics& g) override {
        g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));




        g.setColour(Colours::grey);

        createDial(g, Point<float>(300, 300), 40, theta);
        createDial(g, Point<float>(500, 300), 40, theta);

            
    }
    void createDial(Graphics& g, Point<float>loc, float radius, float rotation) {
        g.setColour(Colours::grey);

        float yloc = loc.y;
        float xloc = loc.x;
        g.fillEllipse(xloc - radius, yloc - radius, radius * 2, radius * 2);

        // limit theta to values (better ways of doing this I know)
        rotation > 4 * pi / 3 ? rotation = 4 * pi / 3 : rotation = rotation;
        rotation < -1 * pi / 3 ? rotation = -1 * pi / 3 : rotation = rotation;

        // create the end points for the inside line
        auto toX = xloc + radius * std::cos(rotation);
        auto toY = yloc - radius * std::sin(rotation);


        // create and draw the center line
        Line<float> dialLine(xloc, yloc, toX, toY);
        g.setColour(getLookAndFeel().findColour(Slider::thumbColourId));
        g.drawLine(dialLine, 2.0f);
        // create a path for the dial indicator to follow
        Path outlinePath;
        outlinePath.addPieSegment(xloc - radius, //starting loc
            yloc - radius,
            radius * 2, //height and width
            radius * 2,
            -rotation + pi / 2, //since the docs for the pie segment start at 12:00, I have to do some math to compensate proper arc
            7 * pi / 6 - 2 * pi,
            1.5 // width of arc relative to circle
        );
        
        g.fillPath(outlinePath);
    }
 

    void mouseDown(const MouseEvent& e) override {
        mouseDrag(e);
    }
    void mouseDrag(const MouseEvent& e) override {
        // handle the mouse input events
        
        if (e.position.y > mposy + dial_sensitivity) {
            theta += radian_increment;
            mposy = e.position.y;
        }
        else if (e.position.y < mposy - dial_sensitivity) {
            theta -= radian_increment;
            mposy = e.position.y;
        }
        
        repaint();
    }

private:
    
    float mposy = 1;
    float theta = 4*pi/3;
    const float radian_increment = pi/6;
    const float dial_sensitivity = 20;
};