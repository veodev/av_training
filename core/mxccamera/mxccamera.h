#ifndef MXC_CAMERA
#define MXC_CAMERA

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#define BUFFER_NUM 3

struct Buffer
{
    unsigned char* start;
    unsigned int offset;
    unsigned int length;
};

class MxcCamera : public QObject
{
    Q_OBJECT

public:
    explicit MxcCamera(QObject* parent = 0);

    bool start();
    bool stop();

    bool printCaps();

private:
    bool init();
    bool release();
    bool openDevice();
    void closeDevice();
    bool initMmap();
    bool freeMmap();
    bool setStreamParm();
    bool setPixelFormat();
    bool setCrop();
    bool setInput();
    bool setRotation();
    bool captureFrame();
    bool tryRestartCapture();
    void resetBuffers();

signals:
    void capturedImage(QSharedPointer<QImage>);

private slots:
    void timeout();

private:
    QTimer _timer;
    struct Buffer buffers[BUFFER_NUM];
    int _fd;
    int _inWidth;
    int _inHeight;
    unsigned int _outWidth;
    unsigned int _outHeight;
    int _top;
    int _left;
    int _input;
    int _captureCount;
    int _rotate;
    unsigned int _capFmt;
    int _cameraFramerate;
    int _captureMode;
    int _count;
};

#endif  // MXC_CAMERA
