#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <asm/types.h>
#include <linux/videodev.h>
#include <sys/mman.h>
#include <cstring>
#include <malloc.h>
#include <cerrno>

#include <QThread>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QDateTime>

#include "mxccamera.h"
#include "debug.h"

static int xioctl(int fd, unsigned long int request, void* arg)
{
    int r;

    do {
        r = ioctl(fd, request, arg);
        QThread::msleep(10);
    } while (-1 == r && EINTR == errno);

    return r;
}


MxcCamera::MxcCamera(QObject* parent)
    : QObject(parent)
    , _fd(-1)
    , _inWidth(720)
    , _inHeight(576)
    , _outWidth(720)
    , _outHeight(576)
    , _top(0)
    , _left(0)
    , _input(0)
    , _captureCount(1)
    , _rotate(0)
    , _capFmt(V4L2_PIX_FMT_RGB565)
    , _cameraFramerate(25)
    , _captureMode(0)
{
    _timer.setInterval(1000 / (_cameraFramerate * 2));
    ASSERT(connect(&_timer, &QTimer::timeout, this, &MxcCamera::timeout));
    resetBuffers();
}

bool MxcCamera::start()
{
    if (!init()) {
        QWARNING << "Cannot init camera";
        return false;
    }

    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(_fd, VIDIOC_STREAMON, &type) < 0) {
        QWARNING << "VIDIOC_STREAMON error";
        return false;
    }

    QThread::msleep(100);
    _timer.start();

    return true;
}

bool MxcCamera::stop()
{
    _timer.stop();

    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(_fd, VIDIOC_STREAMOFF, &type) < 0) {
        QWARNING << "VIDIOC_STREAMOFF error";
        return false;
    }

    release();

    return true;
}

bool MxcCamera::printCaps()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_capability caps;
    ::memset(&caps, 0, sizeof(caps));
    if (-1 == xioctl(_fd, VIDIOC_QUERYCAP, &caps)) {
        QWARNING << "Querying Capabilities";
        return false;
    }

    QDEBUG << QString("Driver Caps:\n"
                      " Driver: \"%1\"\n"
                      " Card: \"%2\"\n"
                      " Bus: \"%3\"\n"
                      " Version: %4.%5\n"
                      " Device capabilities: %6\n")
                  .arg((char*) caps.driver)
                  .arg((char*) caps.card)
                  .arg((char*) caps.bus_info)
                  .arg((caps.version >> 16) & 0xff)
                  .arg((caps.version >> 24) & 0xff)
                  .arg(caps.capabilities);
    QDEBUG << "  Is a video capture device - " << ((caps.capabilities & V4L2_CAP_VIDEO_CAPTURE) ? "yes" : "no");
    QDEBUG << "  Is a video output device - " << ((caps.capabilities & V4L2_CAP_VIDEO_OUTPUT) ? "yes" : "no");
    QDEBUG << "  Can do video overlay - " << ((caps.capabilities & V4L2_CAP_VIDEO_OVERLAY) ? "yes" : "no");
    QDEBUG << "  Is a raw VBI capture device - " << ((caps.capabilities & V4L2_CAP_VBI_CAPTURE) ? "yes" : "no");
    QDEBUG << "  Is a raw VBI output device - " << ((caps.capabilities & V4L2_CAP_VBI_OUTPUT) ? "yes" : "no");
    QDEBUG << "  Is a sliced VBI capture device - " << ((caps.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE) ? "yes" : "no");
    QDEBUG << "  Is a sliced VBI output device - " << ((caps.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT) ? "yes" : "no");
    QDEBUG << "  RDS data capture - " << ((caps.capabilities & V4L2_CAP_RDS_CAPTURE) ? "yes" : "no");
    QDEBUG << "  Can do video output overlay - " << ((caps.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY) ? "yes" : "no");
    QDEBUG << "  Can do hardware frequency seek - " << ((caps.capabilities & V4L2_CAP_HW_FREQ_SEEK) ? "yes" : "no");
    QDEBUG << "  Is an RDS encoder - " << ((caps.capabilities & V4L2_CAP_RDS_OUTPUT) ? "yes" : "no");
    QDEBUG << "  has a tuner - " << ((caps.capabilities & V4L2_CAP_TUNER) ? "yes" : "no");
    QDEBUG << "  has audio support - " << ((caps.capabilities & V4L2_CAP_AUDIO) ? "yes" : "no");
    QDEBUG << "  is a radio device - " << ((caps.capabilities & V4L2_CAP_RADIO) ? "yes" : "no");
    QDEBUG << "  has a modulator - " << ((caps.capabilities & V4L2_CAP_MODULATOR) ? "yes" : "no");
    QDEBUG << "  read/write systemcalls - " << ((caps.capabilities & V4L2_CAP_READWRITE) ? "yes" : "no");
    QDEBUG << "  async I/O - " << ((caps.capabilities & V4L2_CAP_ASYNCIO) ? "yes" : "no");
    QDEBUG << "  streaming I/O ioctls - " << ((caps.capabilities & V4L2_CAP_STREAMING) ? "yes" : "no");

    struct v4l2_cropcap cropcap;
    ::memset(&cropcap, 0, sizeof(cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_CROPCAP, &cropcap)) {
        QWARNING << "Querying Cropping Capabilities";
        return false;
    }

    QDEBUG << QString("Camera Cropping:\n"
                      " Bounds: %1x%2+%3+%4\n"
                      " Default: %5x%6+%7+%8\n"
                      " Aspect: %9/%10\n")
                  .arg(cropcap.bounds.width)
                  .arg(cropcap.bounds.height)
                  .arg(cropcap.bounds.left)
                  .arg(cropcap.bounds.top)
                  .arg(cropcap.defrect.width)
                  .arg(cropcap.defrect.height)
                  .arg(cropcap.defrect.left)
                  .arg(cropcap.defrect.top)
                  .arg(cropcap.pixelaspect.numerator)
                  .arg(cropcap.pixelaspect.denominator);

    int support_rgbp = 0;

    struct v4l2_fmtdesc fmtdesc;
    ::memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char fourcc[5] = {0};
    QDEBUG << " FMT : CE Desc\n--------------------\n";
    while (0 == xioctl(_fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
        strncpy(fourcc, (char*) &fmtdesc.pixelformat, 4);
        if (fmtdesc.pixelformat == V4L2_PIX_FMT_RGB565) {
            support_rgbp = 1;
        }
        char c = ((fmtdesc.flags & 1) != 0) ? 'C' : ' ';
        char e = ((fmtdesc.flags & 2) != 0) ? 'E' : ' ';
        QDEBUG << QString(" %1: %2%3 %4\n").arg(fourcc).arg(c).arg(e).arg((char*) fmtdesc.description);
        fmtdesc.index++;
    }

    if (support_rgbp == 0) {
        QDEBUG << "Doesn't support RGBP.\n";
    }

    return true;
}

bool MxcCamera::init()
{
    resetBuffers();

    if (!openDevice()) {
        qDebug() << "MXCCAMERA: open failed!";
        return false;
    }
    if (!setStreamParm()) {
        qDebug() << "MXCCAMERA: setStreamParm failed!";
        return false;
    }
    if (!setInput()) {
        qDebug() << "MXCCAMERA: setInput failed!";
        return false;
    }
    if (!setCrop()) {
        qDebug() << "MXCCAMERA: setCrop failed!";
        return false;
    }
    if (!setPixelFormat()) {
        qDebug() << "MXCCAMERA: setPixelFormat failed!";
        return false;
    }
    if (!setRotation()) {
        qDebug() << "MXCCAMERA: setRotation failed!";
        return false;
    }
    if (!initMmap()) {
        qDebug() << "MXCCAMERA: initMmap failed!";
        return false;
    }
    return true;
}

bool MxcCamera::release()
{
    freeMmap();
    closeDevice();

    return true;
}

bool MxcCamera::openDevice()
{
    const char* v4lDevice = "/dev/video0";

    if ((_fd = open(v4lDevice, O_RDWR | O_NONBLOCK, 0)) < 0) {
        QWARNING << QString("Cannot open '%1'").arg(v4lDevice);
        return false;
    }

    return true;
}

void MxcCamera::closeDevice()
{
    close(_fd);
    _fd = -1;
}

bool MxcCamera::initMmap()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_requestbuffers requestBuffers;
    memset(&requestBuffers, 0, sizeof(requestBuffers));
    requestBuffers.count = BUFFER_NUM;
    requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestBuffers.memory = V4L2_MEMORY_MMAP;
    if (xioctl(_fd, VIDIOC_REQBUFS, &requestBuffers) < 0) {  // функция VIDIOC_REQBUFS позволяет проинициализировать буфер памяти внутри устройства.
        QWARNING << "VIDIOC_REQBUFS error";
        return false;
    }
    QDEBUG << "request buffers count:" << requestBuffers.count;

    struct v4l2_buffer buf;
    for (unsigned int i = 0; i < BUFFER_NUM; ++i) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.index = i;
        if (xioctl(_fd, VIDIOC_QUERYBUF, &buf) < 0) {  // Функция VIDIOC_QUERYBUF позволяет считать параметры буфера, которые будут использоваться для создания memory-mapping области.
            QWARNING << "VIDIOC_QUERYBUF error";
            return false;
        }
        buffers[i].length = buf.length;
        buffers[i].offset = buf.m.offset;
        buffers[i].start = reinterpret_cast<unsigned char*>(mmap(nullptr, buffers[i].length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buffers[i].offset));
        memset(buffers[i].start, 0xFF, buffers[i].length);
    }
    for (unsigned int i = 0; i < BUFFER_NUM; ++i) {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        buf.m.offset = buffers[i].offset;
        if (xioctl(_fd, VIDIOC_QBUF, &buf) < 0) {  // Функция VIDIOC_QBUF ставит буфер в очередь обработки драйвером устройства.
            QWARNING << "VIDIOC_QBUF error";
            return false;
        }
    }

    return true;
}

bool MxcCamera::freeMmap()
{
    for (auto& buffer : buffers) {
        munmap(buffer.start, buffer.length);
    }

    return true;
}

bool MxcCamera::setStreamParm()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_streamparm streamparm;
    ::memset(&streamparm, 0x00, sizeof(v4l2_streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    streamparm.parm.capture.timeperframe.numerator = 1;
    streamparm.parm.capture.timeperframe.denominator = _cameraFramerate;
    streamparm.parm.capture.capturemode = _captureMode;
    if (xioctl(_fd, VIDIOC_S_PARM, &streamparm) < 0) {
        QWARNING << "VIDIOC_S_PARM error";
        return false;
    }

    return true;
}

bool MxcCamera::setPixelFormat()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_format fmt;
    ::memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = _outWidth;
    fmt.fmt.pix.height = _outHeight;
    fmt.fmt.pix.bytesperline = _outWidth;
    fmt.fmt.pix.pixelformat = _capFmt;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (xioctl(_fd, VIDIOC_S_FMT, &fmt) < 0) {
        QWARNING << "VIDIOC_S_FMT: Setting pixel format error";
        return false;
    }

    if (xioctl(_fd, VIDIOC_G_FMT, &fmt) < 0) {
        QWARNING << "VIDIOC_S_FMT: Getting pixel format error";
        return false;
    }
    char fourcc[5] = {0};
    strncpy(fourcc, (char*) &fmt.fmt.pix.pixelformat, 4);
    QDEBUG << QString("Selected Camera Mode:\n"
                      " Width: %1\n"
                      " Height: %2\n"
                      " PixFmt: %3\n"
                      " Field: %4\n"
                      " Pixel format = %5")
                  .arg(fmt.fmt.pix.width)
                  .arg(fmt.fmt.pix.height)
                  .arg(fourcc)
                  .arg(fmt.fmt.pix.field)
                  .arg(fmt.fmt.pix.pixelformat);

    return true;
}

bool MxcCamera::setCrop()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_crop crop;
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(_fd, VIDIOC_G_CROP, &crop) < 0) {
        QWARNING << "VIDIOC_G_CROP error";
        return false;
    }
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c.width = _inWidth;
    crop.c.height = _inHeight;
    crop.c.top = _top;
    crop.c.left = _left;
    if (ioctl(_fd, VIDIOC_S_CROP, &crop) < 0) {
        QWARNING << "VIDIOC_S_CROP error";
        return false;
    }

    return true;
}

bool MxcCamera::setInput()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    if (ioctl(_fd, VIDIOC_S_INPUT, &_input) < 0) {
        QWARNING << "VIDIOC_S_INPUT error";
        return false;
    }

    return true;
}

bool MxcCamera::setRotation()
{
    if (-1 == _fd) {
        QWARNING << "video device is not opened";
        return false;
    }

    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_PRIVATE_BASE + 0;
    ctrl.value = _rotate;
    if (xioctl(_fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        QWARNING << "VIDIOC_S_CTRL: set ctrl error";
        return false;
    }

    return true;
}

void imageCleanupHandler(void* info)
{
    auto* ptr = reinterpret_cast<uchar*>(info);
    delete ptr;
}

bool MxcCamera::captureFrame()
{
    if (-1 == _fd) {
        qDebug() << "video device is not opened";
        return false;
    }

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(_fd, VIDIOC_DQBUF, &buf) < 0) {  // Функция VIDIOC_DQBUF освобождает буфер из очереди обработки драйвера.
        QWARNING << "VIDIOC_DQBUF error";
        return false;
    }
    const unsigned int size = _outWidth * _outHeight * 2;
    auto* ptr = new uchar[size];
    memcpy(ptr, buffers[buf.index].start, size);
    emit capturedImage(QSharedPointer<QImage>(new QImage(ptr, _outWidth, _outHeight, QImage::Format_RGB16, imageCleanupHandler, ptr)));

    if (xioctl(_fd, VIDIOC_QBUF, &buf) < 0) {  // Функция VIDIOC_QBUF ставит буфер в очередь обработки драйвером устройства.
        QWARNING << "VIDIOC_QBUF error";
        return false;
    }

    return true;
}

bool MxcCamera::tryRestartCapture()
{
    QThread::msleep(100);
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(_fd, VIDIOC_STREAMOFF, &type) < 0) {
        QWARNING << "VIDIOC_STREAMOFF error";
        return false;
    }

    QThread::msleep(100);
    release();
    QThread::msleep(100);
    init();
    QThread::msleep(100);

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(_fd, VIDIOC_STREAMON, &type) < 0) {
        QWARNING << "VIDIOC_STREAMON error";
        return false;
    }
    QThread::msleep(100);

    return true;
}

void MxcCamera::resetBuffers()
{
    for (auto& buffer : buffers) {
        buffer.length = 0;
        buffer.offset = 0;
        buffer.start = nullptr;
    }
}

void MxcCamera::timeout()
{
    if (!captureFrame()) {
        tryRestartCapture();
    }
}
