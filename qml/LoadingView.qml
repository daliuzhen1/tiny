import QtQuick 2.0

AnimatedImage
{
    id: loading
    source: "../resource/loading.gif"
    visible: false
    function startLoading()
    {
        visible = true
    }
    function finishLoading()
    {
        visible = false
    }
}
