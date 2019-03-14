import QtQuick 2.11
import QtQuick.Window 2.11
import QtMultimedia 5.9
import QtQuick.Controls 2.4

Item {
    id: item
    width: 352
    height: 288

    signal doCameraCaptureModeChanged(int mode);
    signal doDigitalZoomChanged(double zoom);
    signal doDurationChanged(int duration);    
    signal doVideoRecorderStatusChanged(int status);
    signal doImageSaved(string fileName);
    signal doReadyForCaptureChanged(bool ready);
    signal doRestartCameraPageForPhotoCapture();
    signal doRestartCameraPageForVideoCapture();

    Camera {
        id: camera;                        
        captureMode: Camera.StillImage;
        position: Camera.BackFace;
        viewfinder.resolution: Qt.size(352, 288);
        viewfinder.minimumFrameRate: 16;
        viewfinder.maximumFrameRate: 16;
        onCaptureModeChanged: item.doCameraCaptureModeChanged(camera.captureMode);
        onDigitalZoomChanged: item.doDigitalZoomChanged(camera.digitalZoom);
        onCameraStatusChanged: printCameraStatus(camera.cameraStatus);

        imageCapture {
            onImageCaptured: {
                photoPreview.source = preview;
            }
            onImageSaved: {
                item.doImageSaved(camera.imageCapture.capturedImagePath);                
//                camera.stop();
//                camera.start();
//                camera.unlock();
            }
            onReadyForCaptureChanged: item.doReadyForCaptureChanged(camera.imageCapture.ready);
        }

        videoRecorder {
            audioEncodingMode: CameraRecorder.ConstantBitRateEncoding;
            audioSampleRate: 16000;
            mediaContainer: "mp4";            
            resolution: Qt.size(352, 288);
            videoCodec: "h264";
            onDurationChanged: item.doDurationChanged(camera.videoRecorder.duration);
            onRecorderStatusChanged: {
                console.log("=== VIDEO RECORDER STATUS: ", camera.videoRecorder.recorderStatus);
                item.doVideoRecorderStatusChanged(camera.videoRecorder.recorderStatus);
            }
        }        
    }

    VideoOutput {
        visible: true
        width: 600
        height: 400
        fillMode: VideoOutput.PreserveAspectFit;
        source: camera;
    }

    Item {
        property alias source : photoPreview.source
        signal closed

        Image {
            id: photoPreview
            anchors.fill : parent
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                parent.closed();
            }
        }
    }

    function zoomIn()
    {        
        console.log("=== ZOOM IN");
        camera.digitalZoom = camera.digitalZoom + 1.0;
    }

    function zoomOut()
    {        
        console.log("=== ZOOM OUT");
        camera.digitalZoom = camera.digitalZoom - 1.0;
    }

    function singleFocus()
    {
        if (camera.lockStatus == Camera.Unlocked) {
            console.log("=== SEARCH AND LOCK");
            camera.searchAndLock();
        }
        else {
            console.log("=== UNLOCK");
            camera.unlock();
        }
    }

    function setPhotoCaptureMode()
    {        
        if (camera.videoRecorder.recorderStatus == CameraRecorder.RecordingStatus) {
            camera.videoRecorder.stop();
        }
        item.doRestartCameraPageForPhotoCapture()
//        camera.setCaptureMode(Camera.CaptureStillImage);
//        camera.stop();
//        camera.start();
//        camera.unlock();
        console.log("=== SET PHOTO CAPTURE MODE");

    }

    function setVideoCaptureMode()
    {
        item.doRestartCameraPageForVideoCapture();
//        camera.setCaptureMode(Camera.CaptureVideo);
//        camera.stop();
//        camera.start();
//        camera.unlock();
        console.log("=== SET VIDEO CAPTURE MODE");

    }

    function captureImage(location)
    {
        if (camera.cameraStatus == Camera.ActiveStatus) {
            console.log("=== CAPTURE IMAGE");
            camera.imageCapture.captureToLocation(location);
        }
    }

    function captureVideo(location)
    {
        if (camera.videoRecorder.recorderStatus == CameraRecorder.LoadedStatus && camera.cameraStatus == Camera.ActiveStatus) {
            console.log("=== CAPTURE VIDEO");
            camera.videoRecorder.setOutputLocation(location);
            camera.videoRecorder.record();
            camera.unlock();
        }
    }

    function stopCaptureVideo()
    {
        if (camera.videoRecorder.recorderStatus == CameraRecorder.RecordingStatus) {
            console.log("=== STOP CAPTURE VIDEO");
            camera.videoRecorder.stop();                        
            camera.stop();
            camera.start();
            camera.unlock();
        }
    }

    function cameraStart()
    {
        console.log("=== CAMERA START");
        camera.start();        
    }

    function cameraStop()
    {
        console.log("=== CAMERA STOP");
        camera.stop();        
    }

    function printCameraStatus(status) {
        var strStatus;
        switch (status) {
        case 0:
            strStatus = "UNAVAILABLE STATUS";
            break;
        case 1:
            strStatus = "UNLOADED STATUS";
            break;
        case 2:
            strStatus = "LOADING STATUS";
            break;
        case 3:
            strStatus = "UNLOADING STATUS";
            break;
        case 4:
            strStatus = "LOADED STATUS";
            break;
        case 5:
            strStatus = "STANDBY STATUS";
            break;
        case 6:
            strStatus = "STARTING STATUS";
            break;
        case 7:
            strStatus = "STOPPING STATUS";
            break;
        case 8:
            strStatus = "ACTIVE STATUS";
            break;
        }
        console.log("=== CAMERA STATUS: ", strStatus);
    }

    function setPhotoCaptureModeAfterRestart()
    {
        camera.setCaptureMode(Camera.CaptureStillImage);
        console.log("=== SET PHOTO CAPTURE MODE AFTER RESTART");

    }

    function setVideoCaptureModeAfterRestart()
    {
        camera.setCaptureMode(Camera.CaptureVideo);
        console.log("=== SET VIDEO CAPTURE MODE AFTER RESTART");

    }
}
