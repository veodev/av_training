import QtQuick 2.11
import QtQuick.Window 2.11
import QtMultimedia 5.9
import QtQuick.Controls 2.4

Item {
    id: item
    width: 640
    height: 480

    signal doVideoPositionChanged(int position);
    signal doVideoDurationChanged(int duration);

    Video {
        id: videoPlayer
        visible: true
        anchors.fill: parent;
        fillMode: VideoOutput.PreserveAspectFit;
        onPositionChanged: item.doVideoPositionChanged(videoPlayer.position);
        onDurationChanged: item.doVideoDurationChanged(videoPlayer.duration);
    }    

    function playVideo()
    {        
        videoPlayer.play();
    }

    function stopVideo()
    {
        videoPlayer.stop();
    }

    function pauseVideo()
    {
        videoPlayer.pause();
    }

    function setMedia(fileName)
    {
        videoPlayer.source = "file://" + fileName;        
    }

    function getVideoPlayerState()
    {
        return videoPlayer.playbackState;
    }
}
