A simple client for www.douban.com

Github: www.github.com/tnrazy/dbfm

------------------------------------------------------------------------

![preview](https://raw.github.com/tnrazy/dbfm/master/preview.png)

------------------------------------------------------------------------

Copy conf/* to $XDG_CONFIG_HOME/dbfm

Config

	# Alsa driver short name "alsa" or "pulse"(make sure pulseaudio has installed)
	driver=pulse

	# Douban.fm channels, curl "http://www.douban.com/j/app/radio/channels" | jsonpp or cat $XDG_CONFIG_HOME/dbfm/channels
	channel=-3

	# Log file
	log=/tmp/dbfm.log

	# Error log
	err=/tmp/dbfm.log

	# Download directory
	download=/media/Music/download

