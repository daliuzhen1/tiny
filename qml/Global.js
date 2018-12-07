.pragma library
var childViews = []
function addChildView(childViewName, childView)
{
    console.log(childViewName)
    childViews.push({"childViewName" : childViewName, "childView" : childView})
    console.log(childViews.length)
}

function getChildView(childViewName)
{
    console.log(childViews.length)
    for (var i = 0; i < childViews.length; i++)
    {
        console.log(childViewName)
        console.log(childViews[i].childViewName)
        if (childViews[i].childViewName == childViewName)
        {
            console.log(1111)
            return childViews[i].childView
        }
    }
}
