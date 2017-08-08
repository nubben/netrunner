#ifndef COMPONENT_H
#define COMPONENT_H

#include <functional>
#include <memory>
#include <vector>
#include <GL/glew.h>

const unsigned int indices[6] = {
    0, 1, 2,
    0, 2, 3
};

struct rgba {
    unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int a;
};

// should have 2 classes after this
// non-rendererable component (wtf are these?)
// and renderable component
class Component {
public:
    virtual ~Component();
    // we're currently constructed with a pos
    // so recalculating it is only good for resize
    // the goal is to combine the resize and initial positioning
    void layout();
    void wrap(); // what resize should be really called but also calls updateParentSize
    void updateParentSize(); // on w/h change
    void setParent(std::shared_ptr<Component> passedParent); // why even?
    // we don't need windowHeight because we prefer vertical pages
    // we shouldn't need to pass these... we should somehow get these from the parent
    // well we need windowWidth/windowHeight for coord conversion
    // we could create a property but then we'd have to update all components on resize
    // which we do already
    // since we update the window size before resize, why just not pass it
    // well no need to pass it if it's just a window* and then we'd already have it
    // but we may hang onto the variables to know what the state is set up for
    // why pass it here? on resize we call layout no this...
    virtual void resize(const int passedWindowWidth, const int passedWindowHeight);
    
    // vab container has handle, shaderProtoType, vbo, ebo (const)
    // we could just have 2 global vabs on for each shaderProto
    // but then we'd might need to vbos each time (on top of the texture)
    void initTheElementBufferObject() const;
    GLuint CreateVertexArrayObject() const;
    GLuint CreateVertexBufferObject() const;
    bool setPosition4(GLuint vertexBufferObject, int x0, int y0, int x1, int y1) const ;
    GLuint CreateTexture() const ;
    bool setTexture(GLuint textureNum, GLsizei w, GLsizei h, const unsigned char *texture) const ;
    
    void pointToViewport(float &rawX, float &rawY) const ;
    void distanceToViewport(float &rawX, float &rawY) const ;
    
    // HELP I'M LOOKING FOR YOUR OPINION
    // a ptr to window would be less memory
    // but it would increase Component's depenedencies
    // could use a base class and that would minimize the dependency
    // but is that really even a problem? does minimizing it help?
    /*
     class screenCoordinates {
         public:
             int windowWidth;
             int windowHeight;
     };
     */
    // possible badly named as these aren't bound to a window
    // how are these used?
    int windowWidth;
    int windowHeight;
    bool boundToPage = true;
    bool verticesDirty = false;
    std::shared_ptr<Component> parent = nullptr;
    std::shared_ptr<Component> previous = nullptr;
    std::vector<std::shared_ptr<Component>> children;
    // current placement on screen
    float x = 0.0f;
    float y = 0.0f;
    // text flow (needed here because parent elements of textnode need to pass this info)
    int endingX = 0;
    int endingY = 0;
    // height/width on screen
    float height = 0.0f;
    float width = 0.0f;
    // calculated minw/h (hey we can't wrap smaller than)
    unsigned int calcMinWidth = 0; // widest glyph we have
    unsigned int calcMinHeight = 0; // tallest glyph we have (thin aspect ratio) or ?? (because we scroll, and act like inifinite vertically) (we don't vertically wrap)
    unsigned int calcMaxWidth = 0; // windowWidth or larger (max rs)
    unsigned int calcMaxHeight = 0; // windowHeight or larger (max rs)
    // requested
    float reqHeight = 0.0f;
    float reqWidth = 0.0f;
    unsigned int reqMinHeight = 0;
    unsigned int reqMinWidth = 0;
    unsigned int reqMaxHeight = 0;
    unsigned int reqMaxWidth = 0;
    // margin
    unsigned int topMargin = 0;
    unsigned int leftMargin = 0;
    unsigned int rightMargin = 0;
    unsigned int bottomMargin = 0;
    // padding
    unsigned int topPadding = 0;
    unsigned int leftPadding = 0;
    unsigned int rightPadding = 0;
    unsigned int bottomPadding = 0;
    // x + left margin + left padding = start x for texture
    // size preference (could also prefer t/b, l/r)
    bool growRight = false; // if less than windowWidth, why not be greedy?
    bool growLeft = false;
    bool growTop = false; // if top/bott grow from center
    bool growBottom = false;
    // color
    rgba color;
    bool isVisibile = true;
    bool isInline = false; // text-only thing but there maybe other non-text inline
    bool isPickable = true;
    bool textureSetup = false;
    // probably shoudl be vector (or maybe use the event emitter)
    std::function<void(int x, int y)> onMousedown = nullptr;
    std::function<void(int x, int y)> onMouseup = nullptr;
    std::function<void(int x, int y)> onMousemove = nullptr;
    std::function<void(int x, int y)> onWheel = nullptr;
    std::function<void(int key, int scancode, int action, int mods)> onKeyup = nullptr;
    std::function<void()> onClick = nullptr;
    std::function<void()> onFocus = nullptr;
    std::function<void()> onBlur = nullptr;
};

#endif
