#ifndef _CONTEXT_HPP
#define _CONTEXT_HPP

class GraphicsContext {
public:

    virtual void init() = 0;

    virtual void swap_buffers() = 0;

};
#endif