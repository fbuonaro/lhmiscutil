load("ext://git_resource", "git_checkout")

git_checkout( "git@github.com:fbuonaro/lhscriptutil.git#main", "./modules/lhscriptutil" )
load( "./modules/lhscriptutil/tilt/common/Tiltfile", "lhQuickBuildImage", "lhQuickBuildImageNoK8" )
load( "./modules/lhscriptutil/Tiltfile", "cfg" )

lhQuickBuildImageNoK8( "lhmiscutil", cfg )