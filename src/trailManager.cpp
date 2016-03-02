#include "trailManager.hpp"


TrailManager::TrailManager(int texWidth, int texHeight, int trailCount, int trailBufferSize, float trailWidth)
{
    _texWidth = texWidth;
    _texHeight = texHeight;

    // opengl initialization :
    _glProgram = loadProgram("/shaders/trail.vs.glsl",
                                  "/shaders/trail.fs.glsl");

    glGenTextures(1, &_renderTexture);
    glBindTexture(GL_TEXTURE_2D, _renderTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texWidth, _texHeight, 0, GL_RGB, GL_FLOAT, (void*)0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    GLuint color_attachment[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, color_attachment);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _renderTexture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr<<"error on building framebuffer."<<std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // trail creation :
    for(int i = 0; i < trailCount; i++)
    {
        _trails.push_back(Trail(trailWidth, trailBufferSize));
        _trails[i].initGL();
    }
}

Camera& TrailManager::getCamera()
{
    return _camera;
}

void TrailManager::updateFromOpenCV(std::vector<int> markerId, std::vector<cv::Vec<double, 3>> currentMarkerPos)
{
    for(int i = 0; i < std::min(markerId.size(), _trails.size()); i++)
    {
        //add a point a the trail :
        _trails[i].pushBack(glm::vec3(currentMarkerPos[i][0], currentMarkerPos[i][1], currentMarkerPos[i][2] ));
        _trails[i].update(); //automaticaly erase points at the end of trails.
    }
}

void TrailManager::renderToTexture()
{
    glViewport(0, 0, _texWidth, _texHeight);
    //draw
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _glProgram.use();
        glUniformMatrix4fv( glGetUniformLocation(_glProgram.getGLId(), "ProjectionMatrix") , 1, false, glm::value_ptr(_camera.getProjectionMat()));
        glUniformMatrix4fv( glGetUniformLocation(_glProgram.getGLId(), "ViewMatrix") , 1, false, glm::value_ptr(_camera.getViewMat()));

    for(int i = 0; i < _trails.size(); i++)
        _trails[i].draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TrailManager::updateCameraPos(std::vector<glm::vec3> corners, float cameraHeight)
{
    glm::vec3 sum(0,0,0);
    for(int i = 0; i < corners.size(); i++)
        sum += corners[i];
    sum /= corners.size();

    _camera.setPosition(glm::vec3(sum.x, cameraHeight, sum.z));
    _camera.setOrthographicProjection(-WINDOW_WIDTH*0.5f, WINDOW_WIDTH*0.5f, -WINDOW_HEIGHT*0.5f, WINDOW_HEIGHT*0.5f);
}

void TrailManager::convertGlTexToCVMat(cv::Mat& cvMat)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

        //use fast 4-byte alignment (default anyway) if possible
        glPixelStorei(GL_PACK_ALIGNMENT, (cvMat.step & 3) ? 1 : 4);
        //set length of one complete row in destination data (doesn't need to equal img.cols)
        glPixelStorei(GL_PACK_ROW_LENGTH, cvMat.step/cvMat.elemSize());
        glReadPixels(0, 0, cvMat.cols, cvMat.rows, GL_BGR, GL_UNSIGNED_BYTE, cvMat.data);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
